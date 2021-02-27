#include <iostream>
#include <mutex>
#include <thread>
#include <list>
#include <atomic>
#include <vector>
#include <chrono>
#include <functional>
#include <condition_variable>

using namespace std;
// g++ main_sleep.cpp -lpthread -o main_sleep
class Workers
{
    int thread_size;
    list<function<void()>> tasks;
    vector<thread> threads;
    mutex worker_mutex;
    condition_variable worker_cv;
    atomic<bool> stop_var{false};
    //Starter alle tråder
    void create_threads(int)
    {
        for (int i = 0; i < thread_size; i++)
        {
            threads.emplace_back([this] {
                thread_task();
            });
        }
    };
    // Definert under klassen
    void thread_task();

public:
    Workers(int n) : thread_size(n){};

    void start()
    {
        create_threads(thread_size);
    };
    //Poster en task
    void post(function<void()> task)
    {
        {
            unique_lock<mutex> lock(worker_mutex);
            tasks.emplace_back(task);
        }
        worker_cv.notify_one();
    };
    //Stopper alle tråder og joiner dem
    void stop()
    {
        stop_var.exchange(true);
        worker_cv.notify_all();
        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    // Bruker eksisterende tråder
    void post_timeout_sleep(function<void()> task, int ms)
    {
        {
            unique_lock<mutex> lock(worker_mutex);
            tasks.emplace_back([ms, task, this] {
                this_thread::sleep_for(chrono::milliseconds(ms));
                // task();
                post(task);
            });
        }
        worker_cv.notify_one();
    }
    //Lager ny tråd som vil vente i steden for working trådene
    void post_timeout(function<void()> task, int ms)
    {
        threads.emplace_back([ms, task, this] { //Add "this" to arguments if task should be done by worker threads
            this_thread::sleep_for(chrono::milliseconds(ms));
            // task();
            post(task);
        });
    }
};

void Workers::thread_task()
{
    while (true)
    {
        function<void()> task;
        {
            unique_lock<mutex> lock(worker_mutex);
            while (tasks.empty())
            {
                //Dersom tasks er tom og trådene skal avslutte returnerer vi
                if (stop_var)
                    return;
                //Tråden settes på ventelisten
                worker_cv.wait(lock);
            }
            task = *tasks.begin();
            tasks.pop_front();
        }
        if (task)
            task();
    }
}

int main(void)
{
    printf("Main thread %ld\n", this_thread::get_id());
    Workers workers(4);
    workers.start();
    for (int i = 0; i < 30; i++)
    {
        //Fix one thread monopoly on all tasks
        workers.post([i] {
            printf("Task %d Thread %ld\n", i + 1, this_thread::get_id());
            for (int j = 0; j < 100000; j++)
                ;
        });
    }

    workers.post_timeout_sleep([] {
        cout << this_thread::get_id() << endl;
    },
                               5000);

    workers.post_timeout([] {
        printf("Task TimeoutThread1 Thread %ld\n", this_thread::get_id());
    },
                         3000);

    workers.post_timeout([] {
        printf("Task TimeoutThread2 Thread %ld\n", this_thread::get_id());
    },
                         500);

    this_thread::sleep_for(chrono::seconds(5));
    workers.stop();

    return 0;
}