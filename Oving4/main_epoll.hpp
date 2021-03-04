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
    function<void()> task;
    event_task(epoll_event timeout, std::function<void()> task): timeout(timeout), task(task){};
};

epoll_event create_event(int ms)
{
    epoll_event timeout;
    // Kan lese
    timeout.events = EPOLLIN;
    // Tiden før "epfd" blir satt til klar, og kan brukes
    timeout.data.fd = timerfd_create(CLOCK_MONOTONIC, 0);
    // Setter opp tiden før "file descriptoren" er tilgjengelig
    // Tatt fra presentasjon
    itimerspec ts;
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    // Endrer tiden i "event"
    timerfd_settime(timeout.data.fd, 0, &ts, nullptr);

    return timeout;
}

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
    int max_timeout = 1000;
    //Starter alle worker tråder
    void create_threads()
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
    // Starter epoll_thread som vil brukes for venting på "file descriptorer"
    void start_epoll_task(){
        threads.emplace_back([this] {
            while (!stop_var)
            {
                /* Venter på et event som finnes i den epollen vi lagde tidligere, gitt ved epfd
                Vil gå gjennom alle eventsene, og gi tilgang når den mottar "file descriptorene" 
                Siste argument er hvor lenge epoll vil vente før den våkner. Vil våkne dersom en 
                "file descriptor blir ledig". Bruker max_timeout sånn at alle "file descriptors" og tasks
                blir tilgjenglig før programmet stopper.
                Kan bruke -1, men vil da aldri stoppe å vente */
                auto count = epoll_wait(epfd, buffer.data(), buffer.size(), max_timeout+500);
                {
                    unique_lock<mutex> lock(epoll_mutex);
                    //Går gjennom bufferen for å se hvilke events som er ledig
                    for (int i = 0; i < count; i++)
                    {
                        for (auto &event : events)
                        {
                            // Finner oppgaven til den tilgjengelige "file descriptoren"
                            if (buffer[i].data.fd == event.timeout.data.fd)
                            {
                                post(event.task);
                                // Fjerner den tilgjengelige "file descriptoren"
                                epoll_ctl(epfd, EPOLL_CTL_DEL, event.timeout.data.fd, nullptr);
                            }
                        }
                    }
                }
            }
        });
    }

public:
    Workers(int n) : thread_size(n){
        buffer.resize(50);
    };
    //Starter alle tråder
    void start()
    {
        create_threads();
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
        // Sjekker at alle timeout oppgaver blir gjort. Stopper ikke mens vi venter på
        // at en "file descriptor" åpner.
        this_thread::sleep_for(chrono::milliseconds(max_timeout+1000));
        stop_var.exchange(true);
        worker_cv.notify_all();
        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    
    void post_timeout(function<void()> task, int ms)
    {
        // Starter epoll_thread bare 1 gang
        if(threads.size() == thread_size) start_epoll_task();
        // Ved ms = 0 blir ikke eventet oppfattet av epoll_wait(), usikker hvorfor.
        if(ms <= 0) ms = 1;
        // Brukes for å sjekke at alle timeout oppgaver er gjort;
        if(max_timeout < ms) max_timeout = ms;
        auto e = create_event(ms);
        // Setter sammen event og task
        event_task et {e, task};
        events.emplace_back(et);
        // Legger til et event som vil leses av wait() etter git antall millisekunder
        epoll_ctl(epfd, EPOLL_CTL_ADD, et.timeout.data.fd, &et.timeout);
    }
};
// Forklart i main_sleep.cpp
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
