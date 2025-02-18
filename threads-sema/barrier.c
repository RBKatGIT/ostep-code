#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

// #include "common_threads.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    // add semaphores and other information here
    sem_t sem1, sem2;
    int count;
    int thread_count;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    // initialization code goes here
    b->thread_count = num_threads;
    b->count = 0;
    sem_init(&b->sem1, 0, 1);
    sem_init(&b->sem2, 0, 0);

}

void barrier(barrier_t *b) {
    // barrier code goes here
    sem_wait(&b->sem1);
    b->count++;
    // printf("Count : %d\n",b->count);
    if(b->count < b->thread_count)
    {
        // printf("Post Sem1\n");
        sem_post(&b->sem1);
        // printf("Wait for Sem2\n");
        sem_wait(&b->sem2);
        // printf("Recieved sem2\n");
        // printf("Wait for sem1\n");
        sem_wait(&b->sem1);
    }
    if(b->thread_count == b->count)
    {
        b->thread_count--;
        b->count--;
        // printf("Post sem1\n");
        sem_post(&b->sem1);
        // printf("Post sem2\n");
        sem_post(&b->sem2);
    }
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of 
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);
    
    int i;
    for (i = 0; i < num_threads; i++) {
	t[i].thread_id = i;
	pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++) 
	pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}

