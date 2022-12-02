#include <thread>
#include <iostream>
#include <mutex>
#include "linkedlist.hpp"

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
LinkedList::LinkedList()
{
    head_ = nullptr;
}

LinkedList::~LinkedList()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto curr = head_;
    while (curr) {
        auto temp = curr->next;
        curr = curr->next;
        delete temp;
    } // end of while loop 
} 

void LinkedList::Insert(int key) {
    // synchronization not needed to create a new node
    node_t *new_node = new node_t();
    if (new_node == NULL) {
        perror("malloc");
        return; 
    }
    new_node->key = key;
    new_node->next = nullptr;
    // just lock critical section
    {
        std::lock_guard<std::mutex> lock(mutex_);
        new_node->next = head_;
        head_ = new_node;
    }
    std::cout<<"Finished inserting head at "<<head_<<std::endl;
}

bool LinkedList::LookUp(int key) {
bool rv = false;
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto curr = head_;
    while (curr) {
        if (curr->key == key) {
        rv = true;
        break; 
        }
        curr = curr->next;
    } // end of while loop 
}
std::cout<<"Finished looking for  "<<key<<std::endl;

return rv; // now both success and failure
}


// int main()
// {
//     LinkedList list;
//     list.Insert(1);
//     list.Insert(2);
//     list.Insert(3);
//     list.Insert(4);


//     std::cout<<list.LookUp(4)<<std::endl;

// }