#include <semaphore.h>
#include <iostream>
sem_t sem;
sem_init(&sem, 0, 1);


int main()
{
    std::cout<<"Test"<<std::endl;
}