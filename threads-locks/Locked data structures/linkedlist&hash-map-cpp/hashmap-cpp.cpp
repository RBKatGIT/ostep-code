/*
https://www.educative.io/courses/operating-systems-virtualization-concurrency-persistence/q24BE8nqzn0
Concurrent hash mpa is done using concurrent list
each list has its own lock
*/

/*
Another thread that uses linear probiong to fill other slots if there are
no free slots found in the first position
https://www.careercup.com/question?id=5813643672289280
*/
#include "linkedlist.hpp"

class HashMap
{
public:
    void Insert(int value)
    {
        auto index = abs(value) % BUCKETS;
        std::cout<<"Lets insert at "<<index<<std::endl;
        lists[index].Insert(value);

    }
    
    bool Lookup(int value)
    {
        auto index = abs(value) % BUCKETS;
        std::cout<<"Lets read from list at "<<index<<std::endl;
        return lists[index].LookUp(value);
    }

private:
    const static uint32_t BUCKETS = 101;
    LinkedList lists[BUCKETS];
};

int main()
{
    HashMap map;
    map.Insert(1);
    std::cout<<map.Lookup(1)<<std::endl;
}

