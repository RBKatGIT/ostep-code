/*
Property 1: if there is only one thread that is ready to run, the
scheduler has to let it run.
Property 2: if a thread is ready to run, then the time it waits until
it runs is bounded.
Property 3: if there are threads waiting on a semaphore when a
thread executes signal, then one of the waiting threads has to be
woken. (instead of the same thread continuing to run)
Property 4: if a thread is waiting at a semaphore, then the number
of threads that will be woken before it is bounded. - Like a que implementation of waiting threads

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>



typedef struct __ns_mutex_t {
    int room1 = 0;
    int room2 = 0;
    sem_t mutex, t1, t2;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    sem_init(&m->mutex,0,1);
    sem_init(&m->t1,0,1);
    sem_init(&m->t2,0,1);
}

// To acquire a lock , ech thread has to pass thogh two turnstiles. These turnstiles divide
// code in to three rooms
// Variable room1 is protected by the mutex.
// room 2 protection is carried out by t1 and t2 together

void ns_mutex_acquire(ns_mutex_t *m) {
    // First add this to room1 
    // protected by mutex
    sem_wait(&m->mutex);
    m->room1 += 1;
    sem_post(&m->mutex);

    // When we get t1 move from rom1 to room2
    sem_wait(&m->t1);
    m->room2 += 1;
    sem_wait(&m->mutex);
    m->room1 -= 1;

    // End of room 1
    // Start of room2

    if(m->room1 == 0) // If All recahed room2 then post a t2
    {
        sem_post(&m->mutex);
        sem_post(&m->t2);
    }
    else // else post a  t1 so other waiting threads can reach room2
    {
        sem_post(&m->mutex);
        sem_post(&m->t1);
    }
    sem_wait(&m->t2);
    m->room2 -= 1;
    // End of room2
    // As soon as a thread exits room2 it is ready to execute the critical session
}

void ns_mutex_release(ns_mutex_t *m) {
    if(m->room2 == 0)
        sem_post(&m->t1);
    else
        sem_post(&m->t2);
}


void *worker(void *arg) {
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    printf("parent: end\n");
    return 0;
}
/*
Explanation
Single thread case
When it gets to Line 8, it holds mutex and t1. Once it checks
room1, which is 0, it can release mutex and then open the second turnstile, t2.
As a result, it doesn’t wait at Line 17 and it can safely decrement room2 and
enter the critical section, because any following threads have to be queued on
t1. Leaving the critical section, it finds room2 = 0 and releases t1, which brings
us back to the starting state.

Multiple thread case
In that case, it is possible that when the lead thread gets to Line 54, other threads
have entered the waiting room and queued on t1. Since room1 > 0, the lead
thread leaves t2 locked and instead signals t1 to allow a following thread to
enter Room 2. Since t2 is still locked, neither thread can enter Room 3.
Eventually (because there are a finite number of threads), a thread will get
to Line 54 before another thread enters Room 1, in which case it will open t2,
allowing any threads there to move into Room 3. The thread that opens t2
continues to hold t1, so if any of the lead threads loop around, they will block
at Line 46.

As each thread leaves Room 3, it signals t2, allowing another thread to leave
Room 2. When the last thread leaves Room 2, it leaves t2 locked and opens
t1, which brings us back to the starting state.

*/


