// Combined implementation from Posix thread page 110 and
// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
// After the implementation use this article to talk about thread pools
//  https://drive.google.com/file/d/1K5BpFVShu74G19ZkECV96fgHAqRhic_E/view 
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>

class ThreadPool
{
public:
    void Start();
    bool QueueJob(const std::function<void()>& job);
    void Stop();
    bool busy();
    ThreadPool(uint32_t num_threads, 
                uint32_t que_size,
                bool is_blocking);
private:
    void ThreadLoop();
    const uint32_t m_num_threads; 
    const uint32_t m_max_que_size;
    bool m_should_terminate = false;           // Tells threads to stop looking for jobs
    bool m_block_when_full = false;
    std::mutex m_queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable m_queue_not_full; // Allows threads to wait on new jobs or termination 
    std::condition_variable m_queue_not_empty;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()> > m_jobs_que;
};