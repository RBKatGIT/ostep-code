#include "RateLimiter.hpp"
#include <unistd.h>

int main()
{
    RateLimiter rl(2,3000,1);
    for(int i =0 ; i<20; i++)
    {
        std::cout<<"Rate limit ::"<<rl.rateLimit()<<std::endl;
        sleep(1);
    }
}