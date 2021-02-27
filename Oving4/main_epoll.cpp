#include <iostream>
#include "main_epoll.hpp"

using namespace std;

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

    workers.stop();

    return 0;
}

