#include <iostream>
#include "main_epoll.hpp"

using namespace std;
// g++ main_epoll.cpp -lpthread -o main_epoll
int main(void)
{
    printf("Main thread %ld\n", this_thread::get_id());
    Workers workers(4);
    Workers event_loop(1);
    event_loop.start();
    workers.start();
    for (int i = 0; i < 30; i++)
    {
        //Fix one thread monopoly on all tasks
        workers.post([i] {
            printf("Task %d Thread %ld\n", i + 1, this_thread::get_id());
            for (int j = 0; j < 100000; j++);
        });
    }

    workers.post_timeout([] {
        printf("Task TimeoutThread1 Thread %ld\n", this_thread::get_id());
    },3000);

    workers.post_timeout([] {
        printf("Task TimeoutThread2 Thread %ld\n", this_thread::get_id());
    },500);

    event_loop.post_timeout([] {
        printf("Task TimeoutThread3 Thread %ld\n", this_thread::get_id());
    },0);

    workers.stop();
    event_loop.stop();

    return 0;
}

