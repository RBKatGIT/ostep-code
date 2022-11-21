#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

/*

https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf - Page 91
If a reader is in the critical section, it holds noWriters, but it doesn’t hold
noReaders. Thus if a writer arrives it can lock noReaders, which will cause
subsequent readers to queue.When the last reader exits, it signals noWriters, allowing any queued writers
to proceed.

When a writer is in the critical section it holds both noReaders and
noWriters. This has the (relatively obvious) effect of insuring that there are no
readers and no other writers in the critical section. In addition, writeSwitch
has the (less obvious) effect of allowing multiple writers to queue on noWriters,
but keeping noReaders locked while they are there. Thus, many writers can
pass through the critical section without ever signaling noReaders. Only when
the last writer exits can the readers enter.

*/
// #include "common.h"
// #include "common_threads.h"

// #ifdef linux
// #include <semaphore.h>
// #elif __APPLE__
// #include "zemaphore.h"
// #endif

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

typedef struct _rwlock_t {
    sem_t noReaders;
    sem_t noWriters;
    LightSwitch reader_switch; // to protect readers variable
    LightSwitch writer_switch; // to protect writers variable
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    sem_init(&lock->noReaders, 0, 1); 
    sem_init(&lock->noWriters, 0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->noReaders);
    lock->reader_switch.lock(lock->noWriters);
    sem_post(&lock->noReaders);
}

void rwlock_release_readlock(rwlock_t *lock) {
    lock->reader_switch.unlock(lock->noWriters);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    lock->writer_switch.lock(lock->noReaders);
    sem_wait(&lock->noWriters);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->noWriters);
    lock->writer_switch.unlock(lock->noReaders);
}

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
    

