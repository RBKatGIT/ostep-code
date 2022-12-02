#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// At least one writer make progress 
// AFter that it might be writer ir reader which 
// Below pattern is also called Light Switch Pattern
// #include "common.h"
// #include "common_threads.h"

// #ifdef linux
// #include <semaphore.h>
// #elif __APPLE__
// #include "zemaphore.h"
// #endif
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

// You can add a turnstile for the readers and allow writers to lock it.
// The writers have to pass through the same turnstile, but they should check the
// roomEmpty semaphore while they are inside the turnstile. If a writer gets stuck
// in the turnstile it has the effect of forcing the readers to queue at the turnstile.
// Then when the last reader leaves the critical section, we are guaranteed that at
// least one writer enters next (before any of the queued readers can proceed).

typedef struct _rwlock_t {
    LightSwitch readSwitch;
    sem_t turnstile; 
    sem_t roomEmpty;
    int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    sem_init(&lock->turnstile, 0, 1); 
    sem_init(&lock->roomEmpty, 0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->turnstile);
    sem_post(&lock->turnstile);
    lock->readSwitch.lock(lock->roomEmpty);
}

void rwlock_release_readlock(rwlock_t *lock) {
    lock->readSwitch.unlock(lock->roomEmpty);
}

/*
If a writer arrives while there are readers in the room, it will block at Line 2,
which means that the turnstile will be locked. This will bar readers from entering
while a writer is queued. When the last reader leaves, it signals roomEmpty, unblocking the waiting
writer. The writer immediately enters its critical section, since none of the
waiting readers can pass the turnstile.
When the writer exits it signals turnstile, which unblocks a waiting thread,
which could be a reader or a writer. Thus, this solution guarantees that at least
one writer gets to proceed, but it is still possible for a reader to enter while
there are writers queued.*/
void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->turnstile);
    sem_wait(&lock->roomEmpty);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->turnstile);
    sem_post(&lock->roomEmpty);
}

////////////////////

// typedef struct _rwlock_t {
//     sem_t turnstile;
//     sem_t roomEmpty;
//     sem_t reader_lock; // to protect readers variable
//     int readers;
// } rwlock_t;

// void rwlock_init(rwlock_t *lock) {
//     lock->readers = 0;
//     sem_init(&lock->turnstile, 0, 1); 
//     sem_init(&lock->roomEmpty, 0, 1); 
//     sem_init(&lock->reader_lock, 0, 1); 

// }

// void rwlock_acquire_readlock(rwlock_t *lock) {
//     sem_wait(&lock->turnstile);
//     sem_post(&lock->turnstile);

//     lock->readers++;
//     sem_wait(&lock->reader_lock);
//     if (lock->readers == 1)
// 	    sem_wait(&lock->roomEmpty);
//     sem_post(&lock->reader_lock);
// }

// void rwlock_release_readlock(rwlock_t *lock) {
//     sem_wait(&lock->reader_lock);
//     lock->readers--;
//     if (lock->readers == 0)
// 	    sem_post(&lock->roomEmpty);
//     sem_post(&lock->reader_lock);
// }

// void rwlock_acquire_writelock(rwlock_t *lock) {
//     sem_wait(&lock->turnstile);
//     sem_wait(&lock->roomEmpty);
// }

// void rwlock_release_writelock(rwlock_t *lock) {
//     sem_post(&lock->turnstile);
//     sem_post(&lock->roomEmpty);
// }

int read_loops;
int write_loops;
int counter = 0;

rwlock_t rwlock;

void *reader(void *arg) {
    int i;
    int local = 0;
    for (i = 0; i < read_loops; i++) {
	rwlock_acquire_readlock(&rwlock);
	local = counter;
	rwlock_release_readlock(&rwlock);
	printf("read %d\n", local);
    }
    printf("read done: %d\n", local);
    return NULL;
}

void *writer(void *arg) {
    int i;
    for (i = 0; i < write_loops; i++) {
	rwlock_acquire_writelock(&rwlock);
	counter++;
	rwlock_release_writelock(&rwlock);
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
    
    rwlock_init(&rwlock); 
    pthread_t c1, c2;
    pthread_create(&c1, NULL, reader, NULL);
    pthread_create(&c2, NULL, writer, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    printf("all done\n");
    return 0;
}
    

