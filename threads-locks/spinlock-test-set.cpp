/*
Spin lock using test and set facility
author: Renu
resource: https://www.educative.io/courses/operating-systems-virtualization-concurrency-persistence/JYngp8PAvvK
*/

#include <iostream>
#include <atomic>

std::atomic_flag lock = ATOMIC_FLAG_INIT;
int main()
{
    bool result = lock.test_and_set();
    std::cout<<"Result : "<<result<<std::endl;
}

