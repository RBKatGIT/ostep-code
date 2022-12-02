#include <thread>
#include <iostream>
#include <mutex>

/*
We do want to write the below code in such a way that even if a failure to malloc happens
we do release thre mutex
 Specifically, we can rearrange the code a bit so that the lock and release only surround 
 the actual critical section in the insert code, and that a common exit path is used in the 
 lookup code. The former works because part of the insert actually need not be locked; 
 assuming that malloc() itself is thread-safe, each thread can call into it without worrying about race 
 conditions or other concurrency bugs. Only when updating the shared list does a lock need to be held. 
 See the code excerpt below for the details of these modifications.

 As for the lookup routine, it is a simple code transformation to jump out of the main search loop 
 to a single return path. Doing so again reduces the number of lock acquire/release points in the code, 
 and thus decreases the chances of accidentally introducing bugs (such as forgetting to unlock before returning)
into the code.

Scaling linked list :

We can use hand over hand locking - enable more concurrency within a list
The idea is pretty simple. Instead of having a single lock for the entire list, 
you instead add a lock per node of the list. When traversing the list, the code firstgrabs the next node’s 
lock and then releases the current node’s lock, which inspires the name hand-over-hand.
Conceptually, a hand-over-hand linked list makes some sense; it enables a high degree of concurrency in list
 operations. However, in practice, it is hard to make such a structure faster than the simple single lock approach, 
 as the overheads of acquiring and releasing locks for each node of a list traversal is prohibitive.
 Perhaps some hybrid approach is worth investigating instead.
*/
class LinkedList
{
public:
    LinkedList();
    
    ~LinkedList(); 

    void Insert(int key);

    bool LookUp(int key);

private:
    typedef struct __node_t
    {
        int key;
        __node_t * next;
    }node_t;

    typedef struct __list_t
    {
        pthread_mutex_t lock;
        node_t * head;
    }list_t;
    node_t * head_;
    std::mutex mutex_;
};

