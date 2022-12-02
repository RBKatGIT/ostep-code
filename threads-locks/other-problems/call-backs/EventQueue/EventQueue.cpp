#include "EventQueue.hpp"
/*https://dev.to/glpuga/multithreading-by-example-the-stuff-they-didn-t-tell-you-4ed8*/

const uint32_t MAX_WAIT_TIME = 1;
// Post event never locks
void EventQueue::PostEvent()
{
    pending_event_ = true;
    cv_.notify_all();
}

/*
Need to run the below in a seperate thread so this wont bloks post events
This thread will run forever until called back using 
*/
void EventQueue::run_forwardThread()
{
    std::unique_lock<std::mutex> lock(mutex);
    while(!halting_)
    {
        // look below in notes to understand why do we nee both predicate and the timer
        cv_.wait_for(lock, max_sleep_interval_, [this](){return this->pending_event_ || this->halting_;});
        if(pending_event_) // An event had come, time to trigger the subscribers
        {
            pending_event_ = false; // order is importnat. if we reset after trigering all, we might loos some calls
            triggerSubscribers();
        }
    }
}

// It creates the original shared_ptr<CallbackFunction>. This shared pointer is the CallbackHandle that subscribeToEvent() will return to the caller.
// It creates a weak_ptr<CallbackFunction> associated to the shared_ptr created above, and stores that weak pointer in the internal list of subscribers. 
// This weak pointer will be used by EventQueue to track which subscribers have been destroyed before calling on their callbacks
// In the caller the usage should be like
// auto handler = event_que.SubscribeToEvent([](){
//     //do something
// });
// Once the handler goes out of scope, the event_queue instance will automatically remove the callback from the subscribers_ list with no need of any further 
// interaction from the subscriber.
EventQueue::CallbackHandler EventQueue::SubscribeToEvent(const EventQueue::CallBackfunction &callback)
{
    std::lock_guard<std::mutex> lock{mutex};
    auto handler = std::make_shared<EventQueue::CallBackfunction>(callback);
    subscribers_.emplace_back(std::weak_ptr<EventQueue::CallBackfunction> (handler));
    return handler;
}

void EventQueue::triggerSubscribers()
{
    // remove all invalid subscribers
    subscribers_.remove_if([](const EventQueue::WeakCallBackHandler& handler){
        return handler.expired();        
    });

    auto caller = [](const EventQueue::WeakCallBackHandler& weak_handler){
        auto handler = weak_handler.lock();
        if(handler) // We need to check here again- this handler can get deleted by the time we reach this line
            (*handler)(2); // There is still a crash possibility here, between lines 55 and 57, handler 
    };

    std::for_each(subscribers_.begin(), subscribers_.end(), caller);
}

EventQueue::~EventQueue()
{
    halting_ = true; // Order is important here
    cv_.notify_all();
    forwarder_thread_.join();
}

EventQueue::EventQueue()
{
    forwarder_thread_ = std::thread([this]()
    {run_forwardThread();
    });
}

/*
Note 1: 
In the above implementation postevent and nofication to the subscribers are detached from each other
But call back to suscribers are sequential and therefore coupled to each other- lets assume none of these
subscriber call backs take longer than whar they are expected

Note 2: 
Think about What are the threads at play in the above 
- PostEvent will be executed by multiple threads and it will temporarily access the internal states of the obeject
- If we have shared object then the object should provide interfaces to lock - or shoudl make use of more internal threads for 
protection task

Note 3: Shared vaiables
- If you have two asynchronous threads that we want to communicate to each other using a  variable then that 
canno be a plain variable - compiler will optimize it out
- This cannot be volatile - because it doesnt do anything to protect the access - it doesnt guarantee atomic access
https://stackoverflow.com/questions/4557979/when-to-use-volatile-with-multi-threading 

Use atomic for concurrenncy and volatile for special memory.

- atomic comes with a price
1. There's no in-register caching of an atomic variable. That means no single-cycle read/write access even for atomics of simple 
integer-like types, such as bool. You'll always go through the memory hierarchy to access them, and pay the price for it.
2. Accesses to an atomic trigger a synchronization process between the caches of different cores in the CPU to ensure a consistent 
event-ordering for all threads on the system. This is a punch right on the performance on any modern multi-core, 
deeply-pipelined processor.
3. For smaller datatypes (bool, for instance), std::atomic<T> may use an implementation that uses atomic load/store instructions 
available on most modern processors, but for larger or user-defined datatypes, std::atomic will basically attach a mutex to the variable 
and acquire/release around each access. This can be even more expensive.
So, and here I'm setting the text in bold again: use std::atomic for unprotected variables you share between two threads, 
and use them sparingly.

If you have lot of shared vaiables lika a que of subsribers then we need to use mutex
each time you lock/unlock a mutex memory access synchronization is guaranteed so that any variable you update within the scope of the lock 
will be visible with the updated value to any other thread that wants to read it later. - There fore we dont need atomic inside mutex

Note 4: Condition varaibles
predicate will solve both the spurious wake up and the missed event problems: - If there is an event when the thread is alraedy awake
the thread will miss it.

Note 5: why do we need a time out in line number 21: There is a non zer chance that 
    thread evaluates halting - false - about to sleep
    halting = true
    cv.notify
    now thread sleeps 
    i.e: We will miss this notification and will sleep for ever.- Timer is to narrow this time window of indefinite blocking
    one way to avoid this is to use mutex in teh destructor - cheaper solution is timer in cv check - Side effect
    thread wakes up periodically withno trigger and goes back to sleep - but with a reasonable value of the timer this should be ok

*/