#include <iostream>
#include <mutex>
#include <thread>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <list>
#include <atomic>
#include <vector>
#include <chrono>
#include <functional>
#include <condition_variable>

using namespace std;

struct event_task{
    epoll_event timeout;
    std::function<void()> task;
    event_task(epoll_event timeout, std::function<void()> task): timeout(timeout), task(task){};
};

epoll_event create_event(int ms)
{
    epoll_event timeout;
    timeout.events = EPOLLIN;
    // Tiden før "epfd" blir satt til klar, og kan brukes
    timeout.data.fd = timerfd_create(CLOCK_MONOTONIC, 0);
    // Setter opp tiden før "file descriptoren" er tilgjengelig
    //Tatt fra presentasjon
    itimerspec ts;
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    // Endrer tiden i "event"
    timerfd_settime(timeout.data.fd, 0, &ts, nullptr);

    return timeout;
}

// g++ main_epoll.cpp -lpthread -o main_epoll
//TODO add a Workers.hpp?
class Workers
{
    int thread_size;
    list<function<void()>> tasks;
    vector<thread> threads;
    mutex worker_mutex;
    condition_variable worker_cv;
    atomic<bool> stop_var{false};
    int epfd = epoll_create1(0);
    vector<epoll_event> buffer;
    mutex epoll_mutex;
    vector<event_task> events;
    //Starts all threads, used in start()
    void create_threads(int)
    {
        for (int i = 0; i < thread_size; i++)
        {
            threads.emplace_back([this] {
                thread_task();
            });
        }
        cout << buffer.size() <<endl;
        threads.emplace_back([this] {
            while (true)
            {
                // Venter på et event som finnes i den epollen vi lagde tidligere, gitt ved epfd
                // Vil gå gjennom alle eventsene, og gi tilgang når den mottar "file descriptorene"
                // -1 som argument, mener at den vil aldri stoppe å polle.
                auto count = epoll_wait(epfd, buffer.data(), buffer.size(), -1);
                {
                    unique_lock<mutex> lock(epoll_mutex);
                    for (int i = 0; i < count; i++)
                    {
                        cout << count << endl;
                        for (auto &event : events)
                        {
                            if (buffer[i].data.fd == event.timeout.data.fd)
                            {
                                post(event.task);
                                epoll_ctl(epfd, EPOLL_CTL_DEL, event.timeout.data.fd, nullptr);
                            }
                        }
                    }
                }
            }
        });
    };
    //Defined under class
    void thread_task();

public:
    Workers(int n) : thread_size(n){
        buffer.resize(50);
    };
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
    //Creates new threads for timeout functions
    void post_timeout(function<void()> task, int ms)
    {
        auto e = create_event(ms);
        event_task et {e, task};
        events.emplace_back(et);
        epoll_ctl(epfd, EPOLL_CTL_ADD, et.timeout.data.fd, &et.timeout);
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
    workers.start();
    for (int i = 0; i < 30; i++)
    {
        //Fix one thread monopoly on all tasks
        workers.post([i] {
            printf("Task %d Thread %ld\n", i + 1, this_thread::get_id());
            for (int j = 0; j < 100000; j++);
        });
    }

    // workers.post_timeout_sleep([] {
    //     cout << this_thread::get_id() << endl;
    // },5000);

    workers.post_timeout([] {
        printf("Task TimeoutThread1 Thread %ld\n", this_thread::get_id());
    },3000);

    workers.post_timeout([] {
        printf("Task TimeoutThread2 Thread %ld\n", this_thread::get_id());
    },500);

    this_thread::sleep_for(chrono::seconds(5));
    workers.stop();

    return 0;
}

