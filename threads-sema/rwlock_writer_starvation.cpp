#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Below pattern is also called Light Switch Pattern
// #include "common.h"
// #include "common_threads.h"

// #ifdef linux
// #include <semaphore.h>
// #elif __APPLE__
// #include "zemaphore.h"
// #endif

// We keep track of the number of readers in the room so 
// we can give special assignements for the first to arrive and
// the last to leave
// https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
/*
LightSwitch takes one parameter, a semaphore that it will check and possibly hold.
If the semaphore is locked, the calling thread blocks on semaphore and all
subsequent threads block on self.mutex. When the semaphore is unlocked,
the first waiting thread locks it again and all waiting threads proceed.
If the semaphore is initially unlocked, the first thread locks it and all subsequent 
threads proceed.
unlock has no effect until every thread that called lock also calls unlock.
When the last thread calls unlock, it unlocks the semaphore.
The first thread into a section locks a semaphore (or queues) and the last one out unlocks it.
*/

struct LightSwitch
{
    int counter = 0;
    sem_t mutex;
    LightSwitch()
    {
        sem_init(&mutex, 0,1);
    }

    void lock(sem_t &semaphore) // This semaphore will be locked at the firsta entry and held for subsequent 
    {
        sem_wait(&mutex);
        counter++;
        if(counter == 1)
            sem_wait(&semaphore);
        sem_post(&mutex);
    }

    void unlock(sem_t & semaphore)
    {
        sem_wait(&mutex);
        counter--;
        if(counter == 0)
            sem_post(&semaphore);
        sem_post(&mutex);
    }
};

/*
Problem : writer starvation - If a writer arrives while there are readers in the critical section, it might wait
in queue forever while readers come and go. As long as a new reader arrives
before the last of the current readers departs, there will always be at least one
reader in the room.
This situation is not a deadlock, because some threads are making progress,
but it is not exactly desirable. A program like this might work as long as the
load on the system is low, because then there are plenty of opportunities for the
writers. But as the load increases the behavior of the system would deteriorate
quickly 
*/

typedef struct _rwlock_t {
    LightSwitch readLightSwitch;
    sem_t roomEmpty; // for protecting the readers
    int readers = 0;
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    sem_init(&lock->roomEmpty, 0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    lock->readLightSwitch.lock(lock->roomEmpty);
}

void rwlock_release_readlock(rwlock_t *lock) {
    lock->readLightSwitch.unlock(lock->roomEmpty);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->roomEmpty);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->roomEmpty);
}



// typedef struct _rwlock_t {
//     sem_t roomEmpty;
//     sem_t readers_lock; // for protecting the readers
//     int readers;
// } rwlock_t;


// void rwlock_init(rwlock_t *lock) {
//     lock->readers = 0;
//     sem_init(&lock->readers_lock, 0, 1); 
//     sem_init(&lock->roomEmpty, 0, 1);  
//     // If  a reade arrive when writer is in progress it will block
//     // on roomEmpty, Subsequent readers will que on readers lock
// }
// // -The first reader that arrives will lock the roomEmpty 
// // and the subsequent readers dont have to wait for roomEmpty
// // Light switch pattern
// // -Only one reader can queue on room empty, while several writers 
// // may be qued on room empty
// // - when the reader signaly roomempty the room must be empty 

// void rwlock_acquire_readlock(rwlock_t *lock) {
//     sem_wait(&lock->readers_lock);
//     lock->readers++;
//     if (lock->readers == 1)
// 	    sem_wait(&lock->roomEmpty);
//     sem_post(&lock->readers_lock);
// }

// void rwlock_release_readlock(rwlock_t *lock) {
//     sem_wait(&lock->readers_lock);
//     lock->readers--;
//     if (lock->readers == 0)
// 	    sem_post(&lock->roomEmpty);
//     sem_post(&lock->readers_lock);
// }

// void rwlock_acquire_writelock(rwlock_t *lock) {
//     sem_wait(&lock->roomEmpty);
// }

// void rwlock_release_writelock(rwlock_t *lock) {
//     sem_post(&lock->roomEmpty);
// }

int read_loops;
int write_loops;
int counter = 0;

rwlock_t mutex;

void *reader(void *arg) {
    int i;
    int local = 0;
    for (i = 0; i < read_loops; i++) {
	rwlock_acquire_readlock(&mutex);
	local = counter;
	rwlock_release_readlock(&mutex);
	printf("read %d\n", local);
    }
    printf("read done: %d\n", local);
    return NULL;
}

void *writer(void *arg) {
    int i;
    for (i = 0; i < write_loops; i++) {
	rwlock_acquire_writelock(&mutex);
	counter++;
	rwlock_release_writelock(&mutex);
    }
    printf("write done\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "usage: rwlock readloops writeloops\n");
	exit(1);
    }
    read_loops = atoi(argv[1]);
    write_loops = atoi(argv[2]);
    
    rwlock_init(&mutex); 
    pthread_t c1, c2;
    pthread_create(&c1, NULL, reader, NULL);
    pthread_create(&c2, NULL, writer, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    printf("all done\n");
    return 0;
}
    

