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


