#include <iostream>
#include "EventQueue.hpp"

void result(int data)
{
    std::cout<<"Result available: "<< data<<std::endl;
}

void new_thread_()
{
    std::cout<<"New thread\n";
    EventQueue que;
    auto handler = que.SubscribeToEvent(result);
    std::cout<<"Handler"<<handler<<std::endl;

    que.PostEvent();
    while(true);
}

int main ()
{
    std::cout<<"HEllo"<<std::endl;
    // EventQueue queue;
    std::thread thread(new_thread_);
    // auto handle1 = queue.SubscribeToEvent([](){"Got trigger"})
    thread.join();
}