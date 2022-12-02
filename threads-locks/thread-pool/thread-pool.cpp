#include "thread-pool.hpp"
/*
For an efficient threadpool implementation, once threads are created according to num_threads, 
it's better not to create new ones or destroy old ones (by joining). There will be a performance penalty,
and it might even make your application go slower than the serial version.
Thus, we keep a pool of threads that can be used at any time (if they aren't already running a job).

Each thread should be running its own infinite loop, constantly waiting for new tasks to grab and run.
*/
ThreadPool::ThreadPool(uint32_t num_threads, uint32_t que_size, bool is_blocking):
            m_num_threads(num_threads), 
            m_max_que_size(que_size),
            m_block_when_full(is_blocking)
{}

void ThreadPool::Start() {
    // const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    m_threads.resize(m_num_threads);
    for (uint32_t i = 0; i < m_num_threads; i++) {
        m_threads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
    }
}

//The infinite loop function. This is a while (true) loop waiting for the task queue to open up.
void ThreadPool::ThreadLoop() {
    std::cout<<"Thread pool starting the loop\n";
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_queue_not_empty.wait(lock, [this] {
                return !m_jobs_que.empty() || m_should_terminate;
            });
            std::cout<<"ThraedLoop got a job to do"<<std::endl;

            if (m_should_terminate) {
                std::cout<<"Threadpool terminating\n";
                return;
            }
            job = m_jobs_que.front();
            m_jobs_que.pop();
            m_queue_not_full.notify_one();

        }
        std::cout<<"Thrae Loop caling the job"<<std::endl;
        job();
        std::cout<<"Thread Loop Finished the job"<<std::endl;
    }
}

// Add a new job to the pool; use a lock so that there isn't a data race.
bool ThreadPool::QueueJob(const std::function<void()>& job) {
    std::cout<<"Lets que job"<<std::endl;
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        if((m_jobs_que.size() == m_max_que_size) && 
            (m_block_when_full == true))
        {
            return -1;
        }
        std::cout<<"Lets wait for the que not full"<<std::endl;

        m_queue_not_full.wait(lock, [this] {
                return (m_jobs_que.size() < m_max_que_size) || m_should_terminate;
            });
        std::cout<<"Got the que not full"<<std::endl;

        if (m_should_terminate) {
            return false;
        }
        m_jobs_que.push(job);
        std::cout<<"Pushed to que"<<std::endl;

    }
    m_queue_not_empty.notify_one();
    return true;
}

bool ThreadPool::busy() {
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        poolbusy = m_jobs_que.empty();
    }
    return poolbusy;
}

void ThreadPool::Stop() {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_should_terminate = true;
    }
    m_queue_not_empty.notify_all();
    m_queue_not_full.notify_all();

    for (std::thread& active_thread : m_threads) {
        active_thread.join();
    }
    m_threads.clear();
}




