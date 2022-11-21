#include <pthread.h>
#include <stdio.h>
#include<assert.h>
/*
Why is this SPSC 
- Signaling a thread only wakes them up; it is thus a hint that the state of the world has changed (in this case, 
that a value has been placed in the buffer), but there is no guarantee that when the woken thread runs,
 the state will still be as desired. 
 - Using only one conditional variable

 How to make it MPMC- change if to a while
*/
int loops; // must initialize somewhere...
pthread_cond_t  cond;
pthread_mutex_t mutex;

int buffer;
int count = 0; // initially, empty

void put(int value) {
  assert(count == 0);
  count = 1;
  buffer = value;
}

int get() {
  assert(count == 1);
  count = 0;
  return buffer;
}

void *producer(void *arg) {
  int i;
  for (i = 0; i < loops; i++){
    pthread_mutex_lock(&mutex);         // p1
    // while(count ==1) - for MPMC
    if (count == 1)                     // p2
      pthread_cond_wait(&cond, &mutex); // p3
    put(i);                             // p4
    pthread_cond_signal(&cond);         // p5
    pthread_mutex_unlock(&mutex);       // p6
  }
} 

void *consumer(void *arg) {
 int i;
 for(i = 0; i < loops; i++) {
   pthread_mutex_lock(&mutex);         // c1
    // while(count == 0) - for MPMC
   if (count == 0)                     // c2      
    pthread_cond_wait(&cond, &mutex);  // c3
    int tmp = get();                   // c4
    pthread_cond_signal(&cond);        // c5
    pthread_mutex_unlock(&mutex);      // c6
    printf("%d\n", tmp);
  }
}