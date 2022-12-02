/*
Problem:
We have a std::function that needs to be called after two different asyncronous tasks are finished.
We want to run both tasks at the same time and we do not know which one will finish first.

https://www.codeproject.com/Articles/1272894/Asynchronous-Multicast-Callbacks-in-C
problem with asynchronous call back
the call back function will be called by the producer and consumer thread,
better way is to have a que between them
*/

#include <iostream>
#include <functional>
#include <atomic>
#include <future>

typedef std::function<void()> CallBack;

void asyncCall(const CallBack & callback)
{
    std::async(std::launch::async, [callback](){
        callback();
    });
}

void startTask1(CallBack & callback)
{
    std::cout<<"Task 1"<<std::endl;
    asyncCall(callback);
}

void startTask2(CallBack & callback)
{
    std::cout<<"Task 2"<<std::endl;
    asyncCall(callback);
}

void StartTwoTasks(const CallBack & callback)
{
    auto counter = std::make_shared<std::atomic<int>> (2);
    CallBack parallel([counter, callback](){
        --(*counter);
        std::cout<<"In parallel"<<std::endl;
        if(counter->load() == 0)
        {
            std::cout<<"Calling callback"<<std::endl;
            callback();
        }
    });
    startTask1(parallel);
    startTask2(parallel);
}

int main()
{
    StartTwoTasks([](){
        std::cout<<"Finished"<<std::endl;
    });
}


