/*
https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
https://github.com/mstump/queues/blob/master/include/mpmc-bounded-queue.hpp

It's not lockfree in the official meaning, just implemented by means of atomic RMW operations w/o mutexes. 
The cost of enqueue/dequeue is 1 CAS per operation. No amortization, just 1 CAS. No dynamic memory allocation/management during operation. Producers and consumers are separated from each other (as in the two-lock queue), i.e. do not touch the same data while queue is not empty.

- ring buffer - avoids maitaining a size variable which will be modified by poush and pop 
thus requiring concurrency
- To get the current position in the array, we can calculate a remainder of integer division of 
tail_ by Q_SIZE, but rather we define Q_SIZE as a power of 2 (32768 in our case), 
so we can use bitwise AND between Q_MASK and tail_, which is bit faster.
- how do we make the BOUNDEDBlocking que lock free?
    CPU provides atomic memory operations and barriers with which we can
    - Read Modify Write
    - Read compare and swap
*/

#include <atomic>
#include <assert.h>

template <typename T>
class mpmc_bounded_queue_t
{

public:
mpmc_bounded_queue_t(size_t size):
    _size(size),
    _mask(size-1),
    _buffer(reinterpret_cast<node_t * >(new aligned_node_t[size])),
    _head_seq(0),
    _tail_seq(0)
{
    // Check if power of two
    assert((_size != 0) && ((_size & (_size + 1)) == _size));

    // Populate initial values
    for(size_t i = 0; i<_size; i++)
    {
        _buffer[i].seq.store(i, std::memory_order_relaxed);
    }

}

~mpmc_bounded_queue_t()
{
    delete[] _buffer;
}

bool enqueue( const T & data)
{
    // _head_seq only wraps at MAX(_head_seq) instead we use a mask
    // to convert the sequence to an array index
    // this is why the ring buffer must be a size which is a power of 2. 
    // this also allows the sequence to double as a ticket/lock.
    size_t head_seq = _head_seq.load(std::memory_order_relaxed);

    for(;;)
    {
        // We will get the node at head_seq and see if its seq value is the same 
        // as head_seq - in that case it means this is an empty slot
        // Lets do calculations with a pointer to this node, to avoid copying
        node_t *node = &_buffer[head_seq & _mask];
        size_t seq = node->seq.load(std::memory_order_acquire);
        intptr_t dif = (intptr_t) seq - (intptr_t) head_seq;
        // if seq and head_seq are the same then this slot is empty
        if(dif == 0)
        {
            // Try to claim this spot, by moving head_seq to the next node so we
            // can fill this node
            // weak compare is faster, but can return spurious results
            // which in this instance is OK, because it's in the loop
            if(_head_seq.compare_exchange_weak(head_seq, head_seq+1, std::memory_order_relaxed))
            {
                // Now all that we have to do is put the data at the spot we got
                node->data = data;
                // Change the seq part of this data so tail knows this node is filled and accessible
                node->seq.store(head_seq+1, std::memory_order_release);
                return true;
            }
            else if (dif < 0) {
                // if seq is less than head seq then it means this slot is full and therefore the buffer is full
                return false;
            }
            else {
                // under normal circumstances this branch should never be taken
                head_seq = _head_seq.load(std::memory_order_relaxed);
            }
        }
    }
    // never taken
    return false;
}

bool dequeue(T & data)
{
    size_t tail_seq = _tail_seq.load(std::memory_order_relaxed);
    for(;;)
    {
        node_t*  node     = &_buffer[tail_seq & _mask];
        size_t   node_seq = node->seq.load(std::memory_order_acquire);
        intptr_t dif      = (intptr_t) node_seq - (intptr_t)(tail_seq + 1);

        if(dif == 0) // this slot has data, claim this data
        {
            if (_tail_seq.compare_exchange_weak(tail_seq, tail_seq + 1, std::memory_order_relaxed)) {
                // set the output
                data = node->data;
                // set the sequence to what the head sequence should be next time around
                node->seq.store(tail_seq + _mask + 1, std::memory_order_release);
                return true;
            }
        }
        else if (dif < 0) {
            // if seq is less than head seq then it means this slot is full and therefore 
            // the buffer is full
            return false;
        }
        else {
            // under normal circumstances this branch should never be taken
            tail_seq = _tail_seq.load(std::memory_order_relaxed);
        }
    }
    // never taken
    return false;

}


private:
    struct node_t 
    {
        T data;
        std::atomic<size_t> seq;
    };
    typedef typename std::aligned_storage<sizeof(node_t), 
                std::alignment_of<node_t>::value>::type aligned_node_t;

    const size_t _size;
    const size_t _mask;
    node_t* const _buffer;
    std::atomic<size_t> _head_seq;
    std::atomic<size_t> _tail_seq;

    mpmc_bounded_queue_t(const mpmc_bounded_queue_t &) {}
    void operator=(const mpmc_bounded_queue_t&){}
};

int main()
{
    return 0;
}

