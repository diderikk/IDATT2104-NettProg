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
// TODO comment, epoll
class Workers
{
    int thread_size;
    list<function<void()>> tasks;
    vector<thread> threads;
    mutex worker_mutex;
    condition_variable worker_cv;
    atomic<bool> stop_var{false};
    //Starts all threads, used in start()
    void create_threads(int)
    {
        for (int i = 0; i < thread_size; i++)
        {
            threads.emplace_back([this] {
                thread_task();
            });
        }
    };
    //Defined under class
    void thread_task();

public:
    Workers(int n) : thread_size(n){};
    //Starts all threads, amount defined in constructor
    void start()
    {
        create_threads(thread_size);
    };
    //Post a task
    void post(function<void()> task)
    {
        {
            unique_lock<mutex> lock(worker_mutex);
            tasks.emplace_back(task);
        }
        worker_cv.notify_one();
    };
    //Stops all threads and joins them.
    void stop()
    {
        stop_var.exchange(true);
        worker_cv.notify_all();
        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    // Use existing threads for timeout functions
    void post_timeout_sleep(function<void()> task, int ms)
    {
        {
            unique_lock<mutex> lock(worker_mutex);
            tasks.emplace_back([ms, task] {
                this_thread::sleep_for(chrono::milliseconds(ms));
                task();
            });
        }
        worker_cv.notify_one();
    }
    //Creates new threads for timeout functions
    void post_timeout(function<void()> task, int ms)
    {
        //TODO join used threads
        threads.emplace_back([ms, task] {
            this_thread::sleep_for(chrono::milliseconds(ms));
            task();
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
                if (stop_var)
                    return;
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
    for (int i = 0; i < 30; i++)
    {
        //Fix one thread monopoly on all tasks
        workers.post([i] {
            printf("Task %d Thread %ld\n", i + 1, this_thread::get_id());
            for (int j = 0; j < 1000; j++);
        });
    }
    workers.start();

    workers.post_timeout_sleep([] {
        cout << this_thread::get_id() << endl;
    },5000);

    workers.post_timeout([] {
        printf("Task TimeoutThread1 Thread %ld\n", this_thread::get_id());
    },3000);

    workers.post_timeout([] {
        printf("Task TimeoutThread2 Thread %ld\n", this_thread::get_id());
    },500);

    this_thread::sleep_for(chrono::seconds(2));
    workers.stop();

    return 0;
}