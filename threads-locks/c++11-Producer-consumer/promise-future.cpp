/*
When we need only one time wake up promise - future is a better choice than cvs
No spurious wake ups and lost wake ups
*/
#include <future>
#include <vector>
#include <iostream>

std::vector<int> my_vec;
void PrepareWork(std::promise<void> prom)
{
    {
    my_vec.insert(my_vec.end(), {0,1,0,3});
    }
    std::cout<<"Sender data prepared."<<std::endl;
}


void CompleteWork(std::future<void> fut)
{
    std::cout<<"Worker waiting for conditional variable\n";
    fut.wait();
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
    std::promise<void> promise;
    std::future<void> fut = promise.get_future();
    std::thread t1(PrepareWork, std::move(promise));
    std::thread t2(CompleteWork, std::move(fut));
    t1.join();
    t2.join();
}


