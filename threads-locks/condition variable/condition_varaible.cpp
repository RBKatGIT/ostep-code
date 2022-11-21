#include <pthread.h>
#include <stdio.h>

/*
Example for conditional varaible
Below program will create a child program and then wait for a CV signal from it before exiting
the main.

TIP: ALWAYS HOLD THE LOCK WHILE SIGNALING


*/

int done = 0;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// This method will make done =1 and signal 
void thr_exit() {
  pthread_mutex_lock(&m);
  done = 1;
  pthread_cond_signal(&c);
  pthread_mutex_unlock(&m);
}


// Method needed for creating a thread
void* child(void * arg)
{
  printf("child\n");
  thr_exit();
  return NULL;
}

/*
This method will be called from main 
for waiting until we have got the cv
*/
void thr_join() {
  pthread_mutex_lock(&m);
  while (done == 0)   // Use  a while here instead of if to avoid spurious wake up
    pthread_cond_wait(&c, &m);  
  pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[]) {
  printf("parent: begin\n");
  pthread_t p;
  pthread_create(&p, NULL, child, NULL);
  thr_join();
  printf("parent: end\n");
  return 0; 
}

/*
Some important obeservation
1. Why do we need the variable done, just use conditional variable signal

void thr_exit() {
  Pthread_mutex_lock(&m);
  Pthread_cond_signal(&c);
  Pthread_mutex_unlock(&m);
}

void thr_join() {
    Pthread_mutex_lock(&m);
    Pthread_cond_wait(&c, &m);
    Pthread_mutex_unlock(&m);
}
Without it approach is broken. Imagine the case where the child runs immediately and calls thr_exit() immediately; 
in this case, the child will signal, but there is no thread asleep on the condition. 
When the parent runs, it will simply call wait and be stuck; no thread will ever wake it.
 From this example, you should appreciate the importance of the state variable done; it records the value the threads
are interested in knowing. The sleeping, waking, and locking all are built around it.

2. Why do we need a mutex to protect the done flag 

void thr_exit() {
  done = 1;
  Pthread_cond_signal(&c);
}

void thr_join() {
  if (done == 0)
    Pthread_cond_wait(&c);
}
Race condition:
The issue here is a subtle race condition. Specifically, if the parent calls thr_join() and then checks t
he value of done, it will see that it is 0 and thus try to go to sleep. But just before it calls wait to go to sleep, 
the parent is interrupted, and the child runs. The child changes the state variable done to 1 and signals, but no 
thread is waiting and thus no thread is woken. When the parent runs again, it sleeps forever, which is sad.
*/





