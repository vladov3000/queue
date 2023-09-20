#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

typedef struct {
  Queue* queue;
  int*   numbers;
  int    count;
} Context;

void* produce(void* context) {
  Queue* queue   = ((Context*) context)->queue;
  int*   numbers = ((Context*) context)->numbers;
  int    count   = ((Context*) context)->count;
  for (int i = 0; i < count; i++) {
    queue_push_wait(queue, sizeof(int));
    *(int*) queue_back(queue) = numbers[i];
    queue_push(queue, sizeof(int));
  }
  return NULL;
}

void* consume(void* context) {
  Queue* queue   = ((Context*) context)->queue;
  int*   numbers = ((Context*) context)->numbers;
  int    count   = ((Context*) context)->count;
  for (int i = 0; i < count; i++) {
    queue_pop_wait(queue, sizeof(int));
    assert(*(int*) queue_front(queue) == numbers[i]);
    queue_pop(queue, sizeof(int));
  }
  return NULL;
}

int main() {
  int capacity = 1 << 26;
  
  Queue* queue = malloc(sizeof(Queue));
  queue_init(queue, capacity);

  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom != -1);

  Context context;
  context.queue   = queue;
  context.count   = capacity / sizeof(int) - 1;
  context.numbers = malloc(sizeof(int) * context.count);
  assert(read(urandom, context.numbers, sizeof(int) * context.count) == sizeof(int) * context.count);

  printf("count: %d\n", context.count);

  pthread_t producer, consumer;
  assert(pthread_create(&producer, NULL, produce, &context) == 0);
  assert(pthread_create(&consumer, NULL, consume, &context) == 0);
  assert(pthread_join(producer, NULL) == 0);
  assert(pthread_join(consumer, NULL) == 0);
}
