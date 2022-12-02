#include <algorithm>
#include <chrono>

#include "RateLimiter.hpp"

// Find the time elapsed from last update
// Add tokens accordingly
void RateLimiter::refillTokensLocked()
{
    auto cur_time = getCurrentTimeMS();
    auto elapsed_time_periods = (cur_time - lastUpdatedTime_)/intervalMS_;
    uint32_t tokes_to_add = tokensPerPeriod_ * elapsed_time_periods;
    if(tokes_to_add == 0)
        return;
    available_tokens_ = std::min(capacity_, available_tokens_ +  tokes_to_add);
    lastUpdatedTime_ = cur_time;
}

bool RateLimiter::rateLimit()
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Refill tokens
    refillTokensLocked();
    // See if we have an available token - Return true/false
    if(available_tokens_ > 0)
    {
        --available_tokens_;
        return true;
    }
    return false;
}

uint64_t RateLimiter::getCurrentTimeMS()
{
    using namespace std::chrono;
    auto current_time = system_clock::now().time_since_epoch();
    // convert time to millisecond
    return duration_cast<milliseconds>(current_time).count();
}
