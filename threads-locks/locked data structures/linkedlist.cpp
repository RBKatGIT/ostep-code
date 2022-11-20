#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
We do want to write the below code in such a way that even if a failure to malloc happens
we do release thre mutex
 Specifically, we can rearrange the code a bit so that the lock and release only surround 
 the actual critical section in the insert code, and that a common exit path is used in the 
 lookup code. The former works because part of the insert actually need not be locked; 
 assuming that malloc() itself is thread-safe, each thread can call into it without worrying about race 
 conditions or other concurrency bugs. Only when updating the shared list does a lock need to be held. 
 See the code excerpt below for the details of these modifications.

 As for the lookup routine, it is a simple code transformation to jump out of the main search loop 
 to a single return path. Doing so again reduces the number of lock acquire/release points in the code, 
 and thus decreases the chances of accidentally introducing bugs (such as forgetting to unlock before returning)
into the code.

Scaling linked list :

We can use hand over hand locking - enable more concurrency within a list
The idea is pretty simple. Instead of having a single lock for the entire list, 
you instead add a lock per node of the list. When traversing the list, the code firstgrabs the next node’s 
lock and then releases the current node’s lock, which inspires the name hand-over-hand.
Conceptually, a hand-over-hand linked list makes some sense; it enables a high degree of concurrency in list
 operations. However, in practice, it is hard to make such a structure faster than the simple single lock approach, 
 as the overheads of acquiring and releasing locks for each node of a list traversal is prohibitive.
 Perhaps some hybrid approach is worth investigating instead.
*/

typedef struct __node_t
{
    int key;
    __node_t * next;
}node_t;

typedef struct __list_t
{
    pthread_mutex_t lock;
    node_t * head;
}list_t;

void List_Init(list_t *L) {
  L->head = NULL;
  pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
  // synchronization not needed
  node_t *new_node = (node_t *) malloc(sizeof(node_t));
  if (new_node == NULL) {
    perror("malloc");
    return; 
  }
  new_node->key = key;
  // just lock critical section
  pthread_mutex_lock(&L->lock);
  new_node->next = L->head;
  L->head = new_node;
  pthread_mutex_unlock(&L->lock);
}

int List_Lookup(list_t *L, int key) {
  int rv = -1;
  pthread_mutex_lock(&L->lock);
  node_t *curr = L->head;
  while (curr) {
    if (curr->key == key) {
      rv = 0;
      break; 
    }
    curr = curr->next;
  } 
  pthread_mutex_unlock(&L->lock);
  return rv; // now both success and failure
}

int main()
{
    list_t*  list = (list_t*) malloc(sizeof(list_t));
}