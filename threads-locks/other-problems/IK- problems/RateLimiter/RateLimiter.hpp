#include <iostream>
#include <mutex>
// https://uplevel.interviewkickstart.com/resource/editorial/rc-freeform-323845-593764-124-626
// Token bucket algoriyj

class RateLimiter
{
public:
    RateLimiter(uint32_t capacity, 
                    uint32_t intervalMS, 
                    uint32_t tokensPerPeriod):
                    capacity_(capacity), 
                    intervalMS_(intervalMS),
                    tokensPerPeriod_(tokensPerPeriod),
                    available_tokens_(capacity)
    {
        lastUpdatedTime_ =  getCurrentTimeMS();  
    }

    // Return true if this request got a token
    bool rateLimit(); 

private:
    uint32_t capacity_; // Max tokens in the token bucket 
    uint32_t available_tokens_;
    uint32_t intervalMS_; // Every intervalMS in ms we can add new tokens to the bucket
    uint32_t tokensPerPeriod_; // How many tokens we will add in each period
    uint64_t lastUpdatedTime_; // in milliseconds
    std::mutex mutex_;
    uint64_t getCurrentTimeMS();
    void refillTokensLocked();
};


