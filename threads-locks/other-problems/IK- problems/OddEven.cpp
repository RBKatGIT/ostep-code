#include <iostream>
#include <thread>

int counter = 0;
std::mutex mutex;
std::condition_variable cv_odd, cv_even;

void printEven()
{
    for(int i = 1; i< 10; i=i+2)
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(counter % 2 == 1)
            cv_odd.wait(lock);
        counter++;
        std::cout<<"Thread 1: "<<counter<<std::endl;
        cv_even.notify_all();
    }
}

void printOdd()
{
    for(int i = 0; i< 10; i=i+2)
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(counter % 2 == 0)
            cv_even.wait(lock);
        counter++;
        std::cout<<"Thread 2: "<<counter<<std::endl;
        cv_odd.notify_all();
    }
}

int main()
{
    std::thread t1(printEven);
    std::thread t2(printOdd);
    t1.join();
    t2.join();
}