/*
https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton
 There are 3 ways to initialize thred safe initialization in c++
 1. constexpre
    initialized at compile time
    even user types can be constexpre

*/


#include <iostream>
#include <thread>
#include <mutex>
 
std::once_flag onceFlag;
struct myStruct
{
    constexpr myStruct(double val):val_(val) {}
    constexpr double getValue() const
    { 
        return val_;
    }
    double val_;
};

/*
using call_once and std::once_flag

*/


 
void do_once(){
  std::call_once(onceFlag, [](){ std::cout << "Only once." << std::endl; });
}
 

int main()
{
    constexpr myStruct st(2.5);
    std::thread t1(do_once);
    std::thread t2(do_once);
    std::thread t3(do_once);
    std::thread t4(do_once);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
