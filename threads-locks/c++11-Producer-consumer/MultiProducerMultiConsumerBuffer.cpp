#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <sys/syscall.h>
#include <unistd.h>



/*
Use two conditional variables one mutex 
*/
pthread_cond_t  empty = PTHREAD_COND_INITIALIZER, fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define MAX 1000
int buffer[MAX];
int fill_ptr = 0;
int use_ptr  = 0;
int count    = 0;
int max = 0;

void put (int value) {
  buffer[fill_ptr] = value;
  fill_ptr = (fill_ptr + 1) % MAX;
  count++;
  // printf("Put %d to %d\n", value, fill_ptr);

}

int get() {
  int tmp = buffer[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  count--;
  // printf("Returned %d from %d\n", tmp, use_ptr);

  return tmp;
}


void *producer(void *arg) {
  int i;
  for (i = 0; i < max; i++){
    pthread_mutex_lock(&mutex);           //p1
    while (count == MAX)                  //p2
      pthread_cond_wait(&empty, &mutex);  //p3
    // printf("Producer got mutex\n");
    put(i);                               //p4
    pthread_cond_signal(&fill);           //p5
    pthread_mutex_unlock(&mutex);         //p6
  }
}

void *consumer(void *arg) {
  int i;
  auto tid = pthread_self();
  printf("Created consumer %u\n", tid);
  // for (i = 0; i<100;i++){ 
    while(true)
    {
      pthread_mutex_lock(&mutex);           //c1
      // printf("%u got mutex\n", tid);
      while (count == 0)                    //c2
        pthread_cond_wait(&fill, &mutex);   //c3
      int tmp = get();                      //c4
      printf("consumer: %u got %d\n", tid, tmp);
      pthread_cond_signal(&empty);          //c5
      // printf("%u releasing mutex\n", tid);
      pthread_mutex_unlock(&mutex);         //c6
   }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "usage: %s <buffersize> <consumers>\n", argv[0]);
	exit(1);
    }
    max   = atoi(argv[1]);
    // loops = atoi(argv[2]);
    int consumers = atoi(argv[2]);
    // assert(consumers <= CMAX);

    pthread_t pid, cid[consumers];
    pthread_create(&pid, NULL, producer, NULL); 
    for (int i = 0; i < consumers; i++) {
	    pthread_create(&cid[i], NULL, consumer, (void *) (long long int) i); 
    }
    pthread_join(pid, NULL); 
    for (int i = 0; i < consumers; i++) {
	pthread_join(cid[i], NULL); 
    }
    return 0;
}