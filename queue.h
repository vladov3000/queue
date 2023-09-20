#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
  char* memory;
  int   start;
  int   end;
  int   capacity;
} Queue;

void queue_init(Queue* queue, int capacity);

void queue_push_wait(Queue* queue, int size);

void* queue_back(Queue* queue);

void queue_push(Queue* queue, int size);

void queue_pop_wait(Queue* queue, int size);

void* queue_front(Queue* queue);

void queue_pop(Queue* queue, int size);

#endif
