#include <iostream>
#include <atomic>
#include <thread>
#include <unistd.h>

// CAS, RMW
// The compare exchange also called compare and swap (CAS) is the most powerful atomic operation available in C++. 
// In most cases, it allows an atomic comparison of the current value of an atomic variable, and if that comparison
// is true, it then attempts to store the desired value. Despite being an atomic operation, a compare exchange 
// can certainly fail, if another thread changes the value of the variable between the time the compare exchange 
// reads and before it writes. A compare exchange operation is otherwise known as a read-modify-write operation (RMW).

// Processors implement compare exchange in different ways. In some, strongly ordered processors, like x86 ones, 
//compare exchange is done in a single assembly instruction. What this means is that compare exchange operations only
// fail if another thread truly changed the value of the atomic variable before the compare exchange operation is 
// completed. On weakly ordered processors, compare exchange is implemented with two assembly instructions, usually 
// locked load and conditional store (LLCS) . 
// LLCS can fail transiently due to using two instructions, such as for a thread being context switched.
// Stack with atomic compare and exchange
// https://blog.devgenius.io/a-simple-guide-to-atomics-in-c-670fc4842c8b
class LFS {
public:
	struct node
	{
	    int data;
	    node*  next;
	    node(const int data) : data(data), next(nullptr) {}
	};
	void push(const int val)
	{
		// std::cout<<"\nPush request for "<<val<<std::endl;
		node* new_node = new node(val);
		new_node->next = _head.load();
		while(!_head.compare_exchange_weak(new_node->next, new_node))
		{
			// std::cout<<"Retry push"<<std::endl;
		}
		// std::cout<<"Done push head is set to "<<new_node<<std::endl;
	}

	int pop()
	{
		// std::cout<<"\nGot pop"<<std::endl;

		node* got = _head.load();
		node* nextn = nullptr;
		// std::cout<<"Pop has 'got' as "<<got<<std::endl;
		do {
			if(got == nullptr) {
				return false;
			}
			nextn = got->next;
		} while(!_head.compare_exchange_weak(got, nextn));
        // std::cout<<"Popping: "<<got->data<<std::endl;
        int m = got->data;
        delete got;
		return m;
	}
private:
	std::atomic<node*> _head{0};
};

int main()
{
    LFS  s;
            int m;

    std::thread t1 = std::thread([&]{
        for (int i = 0; i < 5; i++)
        {
            s.push(i);
        }
        
    });

		    std::thread t4 = std::thread([&]{
        for (int i = 0; i < 5; i++)
        {
            s.push(i);
        }
        
    });


    std::thread t2 = std::thread([&]{

        for (int i = 0; i < 5; i++)
        {
            std::cout<<"\n T2 : "<<s.pop()<<std::endl<<std::flush;
        }

    });
	std::thread t3 = std::thread([&]{

		for (int i = 0; i < 5; i++)
		{
			std::cout<<"\nT3 : "<<s.pop()<<std::endl<<std::flush;
		}

    });


	t1.join();
    t2.join();
	t3.join();
}