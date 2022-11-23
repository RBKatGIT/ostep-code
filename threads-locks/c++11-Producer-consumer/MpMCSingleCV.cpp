#include <stdio.h>
#include <pthread.h>
#include <assert.h>

/*
Use two conditional variables one mutex 
*/
#define MAX 1000
int buffer[MAX];
int fill_ptr = 0;
int use_ptr  = 0;
int count    = 0;
int max;
int loops;
#define CMAX (10)
int consumers = 1;

void put (int value) {
  buffer[fill_ptr] = fill_ptr+1;
  fill_ptr = (fill_ptr + 1) % MAX;
  count++;
//   printf("Putting value %d at %d")
}

int get() {
  int tmp = buffer[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  count--;
  return tmp;
}

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
  int i;
  for (i = 0; i < 5; i++){
    pthread_mutex_lock(&mutex);           //p1
    while (count == MAX)                  //p2
      pthread_cond_wait(&cond, &mutex);  //p3
    put(i);                               //p4
    pthread_cond_broadcast(&cond);           //p5
    pthread_mutex_unlock(&mutex);         //p6
  }
}

void *consumer(void *arg) {
  int i;
//   for (i = 0; i<10;i++){ 
    while(true)
    {
        pthread_mutex_lock(&mutex);           //c1
        while (count == 0)                    //c2
        pthread_cond_wait(&cond, &mutex);   //c3
        int tmp = get();                      //c4
        pthread_cond_broadcast(&cond);          //c5
        pthread_mutex_unlock(&mutex);         //c6
        printf("%d\n", tmp);
    }
//   }
}



int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "usage: %s <buffersize> <consumers>\n", argv[0]);
	exit(1);
    }
    max   = atoi(argv[1]);
    // loops = atoi(argv[2]);
    consumers = atoi(argv[2]);
    assert(consumers <= CMAX);

    pthread_t pid, cid[CMAX], prod[CMAX];
    for (int i = 0; i < consumers; i++) {
	pthread_create(&prod[i], NULL, producer, (void *) (long long int) i); 
    }
    for (int i = 0; i < consumers; i++) {
	pthread_create(&cid[i], NULL, consumer, (void *) (long long int) i); 
    }
    for (int i = 0; i < consumers; i++) {
	pthread_join(prod[i], NULL); 
    }
    for (int i = 0; i < consumers; i++) {
	pthread_join(cid[i], NULL); 
    }
    return 0;
}