#include "queue.h"
#include <stdlib.h>
#include <assert.h>

int main() {
  int capacity = 1 << 20;
  
  Queue* queue = malloc(sizeof(Queue));
  queue_init(queue, capacity);

  int  count   = capacity / sizeof(int);
  int* numbers = malloc(sizeof(int) * count);
  for (int i = 0; i < count; i++)
    numbers[i] = rand();
  
  for (int i = 0; i < count; i++) {
    *(int*) queue_back(queue) = numbers[i];
    queue_push(queue, sizeof(int));
  }

  for (int i = 0; i < count; i++) {
    assert(*(int*) queue_front(queue) == numbers[i]);
    queue_pop(queue, sizeof(int));
  }
}
