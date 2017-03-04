#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *increment();
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

int main() {
  int rc1, rc2;
  pthread_t t1, t2;

  /* Create threads */
  if ( (rc1 = pthread_create(&t1, NULL, &increment, NULL)) )
    printf("Thread creation failed: %d\n",rc1);

  if ( (rc1 = pthread_create(&t2, NULL, &increment, NULL)) )
    printf("Thread creation failed: %d\n",rc2);

  /* Wait until threads are complete before we continue, so we don't exit terminate this process
     before threads have completed */
  pthread_join(t1,NULL);
  pthread_join(t2,NULL);

  exit(EXIT_SUCCESS);
}

void *increment() {
  pthread_mutex_lock(&mutex);
  counter++;
  printf("Counter value: %d\n", counter);
  pthread_mutex_unlock(&mutex);
}
