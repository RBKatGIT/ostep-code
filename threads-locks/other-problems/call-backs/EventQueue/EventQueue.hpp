/*
https://dev.to/glpuga/multithreading-by-example-the-stuff-they-didn-t-tell-you-4ed8
*/

#include <condition_variable>
#include <functional>
#include <list>
#include <thread>
class EventQueue{

public:
    using CallBackfunction = std::function<void(int)>;
    using CallbackHandler = std::shared_ptr<CallBackfunction>;
    /*
        RAII Call backs - If a code subscribes and doesnt un subscrive we will 
        unsunsribe automatically if the returned handler from the below goes out of scope
        To implement this the code uses std::weak_ptr, which is a weak version of shared_ptr 
        that does not hold ownership over the pointed-to-data, but that allows us to:
        - Check if an associated shared_ptr still exists.
        - Get a copy of the associated shared_ptr if at least one other copy still exists.
    */
   /*
   Returns call back object. to unregister jusr destroy the object.
   */ 
    CallbackHandler SubscribeToEvent(const CallBackfunction & callback);

    /*
    - We want to make sure that the PostEvent is nonblocking
    - So the source thread who posts the event will just post and return
    - Triggering call back for all subscribers shoukld happen after
    - Also the Post event and the function that handles the call back function trigger shouldnt share the same lock.
    ::: No lock in the PostEvent- But this method needs to access some state of EventQueue in a thread safe manner
    - solution conditional variable
    */
     /** Broadcast an event to all subscribers */
    void PostEvent();
    EventQueue();

    ~EventQueue();

    // Disable all copy constructors and assighemnt opeeators it doesnt
    EventQueue(const EventQueue &) = delete;
    EventQueue &operator=(const EventQueue &) = delete;
    EventQueue(EventQueue &&) = delete;
    EventQueue &operator=(EventQueue &&) = delete;

private:
    std::atomic_bool pending_event_{false};
    std::atomic_bool halting_{false};
    std::condition_variable cv_;
    std::mutex mutex;
    std::chrono::milliseconds max_sleep_interval_{100};
    using WeakCallBackHandler = std::weak_ptr<CallBackfunction>;
    using SharedCallBackHandler = std::shared_ptr<CallBackfunction>;
    std::list<WeakCallBackHandler> subscribers_;
    std::thread forwarder_thread_;

    void run_forwardThread();
    // 1. Remove all weak pointers whose shared pointer has gone out of scope
    // 2. Iterate through the remaining subscribers and call back
    void triggerSubscribers();


    //  We need a seperate thread for ex

};

