#include <pthread.h>
#include <iostream>

/*
Multiple CPUS can have an implemenetation based on local and global counter
https://www.educative.io/courses/operating-systems-virtualization-concurrency-persistence/B8O260xz04Y
*/
typedef struct __counter_t
{
    int value;
    pthread_mutex_t lock;
}counter_t;


void init(counter_t *counter)
{
    pthread_mutex_init(&counter->lock, NULL);
    counter->value = 0;
}

void increment(counter_t *counter)
{
    pthread_mutex_lock(&counter->lock);
    counter->value++;
    pthread_mutex_unlock(&counter->lock);
}

void decrement(counter_t *counter)
{
    pthread_mutex_lock(&counter->lock);
    counter->value--;
    pthread_mutex_unlock(&counter->lock);
}


int get(counter_t *c) {
  pthread_mutex_lock(&c->lock);
  int rc = c->value;
  pthread_mutex_unlock(&c->lock);
  return rc;
}

