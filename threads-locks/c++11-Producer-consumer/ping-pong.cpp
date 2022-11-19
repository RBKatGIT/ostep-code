// pingPongConditionVariable.cpp

#include <condition_variable>
#include <iostream>
#include <atomic>
#include <thread>

bool dataReady{false};

std::mutex mutex_;
std::condition_variable condVar1;          // (1)
std::condition_variable condVar2;          // (2)

std::atomic<int> counter{0};
constexpr int countlimit = 10000;

void ping() {

    while(counter <= countlimit) 
    {
        {
            std::unique_lock<std::mutex> lck(mutex_);
            condVar1.wait(lck, []{return dataReady == false;});
            dataReady = true;
        }
        ++counter;
        // std::cout<<"Coubnter "<<counter<<"Data ready is "<<dataReady<<std::endl;
        condVar2.notify_all();              // (3)
  }
}

void pong() {

    while(counter < countlimit) 
    {  
        {
            std::unique_lock<std::mutex> lck(mutex_);
            // std::cout<<"Pong Waiting for cv2"<<std::endl;
            condVar2.wait(lck, []{return dataReady == true;});
            // std::cout<<"Pong Got cv2"<<std::endl;
            dataReady = false;
        }
        condVar1.notify_all();            // (3)
        // std::cout<<"Consumed "<<counter<<std::endl;
  }

}

int main(){

    auto start = std::chrono::system_clock::now();  

    std::thread t1(ping);
    std::thread t2(pong);

    t1.join();
    t2.join();
  
    std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
    std::cout << "Duration: " << dur.count() << " seconds" << std::endl;

}