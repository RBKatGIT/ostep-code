/*
Sender/receiver workflows are quite common for threads. In such a workflow, 
the receiver is waiting for the sender's notification before it continues to work.
 There are various ways to implement these workflows. With C++11,
 you can use condition variables or promise/future pairs; with C++20, you can use atomics.
 Conditinal variable issues - spurious wake ups/ lost wake ups
 Both can be avoided if we use prediate with conditional varibaler
*/

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mutex_;
std::condition_variable cond_;

std::vector<int> my_vec;

void PrepareWork()
{
    {
    std::lock_guard<std::mutex> lck(mutex_);
    my_vec.insert(my_vec.end(), {0,1,0,3});
    }
    std::cout<<"Sender data prepared."<<std::endl;
    cond_.notify_one();
}


void CompleteWork()
{
    std::cout<<"Worker waiting for conditional variable\n";
    std::unique_lock<std::mutex> lck(mutex_);
    cond_.wait(lck, []{return not my_vec.empty();});
    my_vec[2] =2;
    std::cout<<"Waiter complete"<<std::endl;
    for(auto i: my_vec)
    {
        std::cout<<i<<" ";
    }
    std::cout<<std::endl;
}

int main()
{
    std::thread t1(PrepareWork);
    std::thread t2(CompleteWork);
    t1.join();
    t2.join();
}

