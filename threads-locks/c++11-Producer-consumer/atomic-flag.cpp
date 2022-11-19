/*
Note - Below program needs c++ 20 to run
- reset - clear
- test and set - set and return the old value
- Below for c++ 20
wait
- notyify_one and notify_all 
- wait(boo) will wait till the next notification or spurious wake up , unblocks if the
atomic flag is not boo

Atomic flag 
    - is the only lock freea atomic
    - building block for higher thread abstraction

*/

/*
When we need only one time wake up promise - future is a better choice than cvs
No spurious wake ups and lost wake ups
*/
// threadSynchronisationAtomicFlag.cpp

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::vector<int> myVec{};

std::atomic_flag atomicFlag{};

void prepareWork() {

    myVec.insert(myVec.end(), {0, 1, 0, 3});
    std::cout << "Sender: Data prepared."  << std::endl;
    atomicFlag.test_and_set();                             // (1)
    atomicFlag.notify_one();   

}

void completeWork() {

    std::cout << "Worker: Waiting for data." << std::endl;
    atomicFlag.wait(false);                                // (2)
    myVec[2] = 2;
    std::cout << "Waiter: Complete the work." << std::endl;
    for (auto i: myVec) std::cout << i << " ";
    std::cout << std::endl;
    
}

int main() {

    std::cout << std::endl;

    std::thread t1(prepareWork);
    std::thread t2(completeWork);

    t1.join();
    t2.join();

    std::cout << std::endl;
  
}