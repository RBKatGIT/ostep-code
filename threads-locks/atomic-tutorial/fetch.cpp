// C++ offers fetcch_add, fetch_sub, fech_and, fetch_or and fetch_xor

//  A counter using atomic fetch
// fetch_add will retrurn the previous value before add
#include <atomic>
#include <cstdio>
#include <iostream>

class Counter{
public:
    Counter():count(0){}  

    unsigned operator ++(int){
        return count.fetch_add(1);
    }  

    unsigned operator --(int){
        return count.fetch_sub(1);
    }   


private:
    std::atomic<int> count;
};

// The exchange operation is also something called cache coherent. 
// This means that, after an exchange, any operation following that reflects the value the exchange writes to the 
// variable.


int main()
{
    Counter counter;
    std::cout<<counter++<<std::endl;
    std::cout<<counter--<<std::endl;
    // Exchange - will just put the new value and return the old
    // std::atomic<int> i{1};
    // std::cout<<i.exchange(3)<<std::endl;
    // std::cout<<i.exchange(4)<<std::endl;
    // std::cout<<"Helo"<<std::endl;

}
