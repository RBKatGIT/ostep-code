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


typedef struct _rwlock_t {
    sem_t turnstile;
    sem_t roomEmpty;
    sem_t reader_lock; // to protect readers variable
    int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    sem_init(&lock->turnstile, 0, 1); 
    sem_init(&lock->roomEmpty, 0, 1); 
    sem_init(&lock->reader_lock, 0, 1); 

}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->turnstile);
    sem_post(&lock->turnstile);

    lock->readers++;
    sem_wait(&lock->reader_lock);
    if (lock->readers == 1)
	    sem_wait(&lock->roomEmpty);
    sem_post(&lock->reader_lock);
}

void rwlock_release_readlock(rwlock_t *lock) {
    sem_wait(&lock->reader_lock);
    lock->readers--;
    if (lock->readers == 0)
	sem_post(&lock->roomEmpty);
    sem_post(&lock->reader_lock);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->turnstile);
    sem_wait(&lock->roomEmpty);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->turnstile);
    sem_post(&lock->roomEmpty);
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
    

