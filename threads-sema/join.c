/*
The first problem is just to implement and test a solution to the fork/join problem, as described in the text. 
Even though this solution is described in the text, the act of typing it in on your own is worthwhile;
even Bach would rewrite Vivaldi, allowing one soon-to-be master to learn from an existing one. See fork-join.c 
for details. Add the call sleep(1) to the child to ensure it is working.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif

sem_t s;

void *child(void *arg) {
    sleep(2);
    printf("child\n");
    Sem_post(&s); // signal here: child is done
    return NULL;
}

int main(int argc, char *argv[]) {
    Sem_init(&s, 0); 
    printf("parent: begin\n");
    pthread_t c;
    Pthread_create(&c, NULL, child, NULL);
    Sem_wait(&s); // wait here for child
    printf("parent: end\n");
    return 0;
}
    
