#include "thread-pool.hpp"
#include <unistd.h>


void test_functin(std::function< void()> myfunction)
{
    myfunction();
}

int main()
{
    ThreadPool tpool(/*num_threads*/ 1, /*que_size*/ 2, /*is_blocking*/ true);
    tpool.Start();
    // sleep(1);
    // test_functin([]{
    //     for(int i = 0; i<10; i++)
    //     {
    //         std::cout<<"I am job 1\n";
    //         sleep(1);
    //     }
    // });
    tpool.QueueJob([]{
        for(int i = 0; i<10; i++)
        {
            std::cout<<"I am job 1\n";
            sleep(1);
        }
    });
    tpool.QueueJob([]{
        for(int i = 0; i<10; i++)
        {
            std::cout<<"I am job 2\n";
            sleep(2);
        }
    });
        tpool.QueueJob([]{
        for(int i = 0; i<10; i++)
        {
            std::cout<<"I am job 3\n";
            sleep(1);
        }
    });
    sleep(100);
    tpool.Stop();
}