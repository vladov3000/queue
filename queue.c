#include "queue.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

void queue_init(Queue* queue, int capacity) {
  int fd = shm_open("queue", O_RDWR | O_CREAT, 0777);
  assert(fd != -1);

  assert(shm_unlink("queue") == 0);

  assert(ftruncate(fd, capacity * 2) == 0);

  int   prot   = PROT_READ | PROT_WRITE;
  int   flags  = MAP_SHARED;
  char* memory = mmap(NULL, capacity * 2, prot, flags, fd, 0);
  assert(memory != MAP_FAILED);

  assert(mmap(memory,            capacity, prot, flags | MAP_FIXED, fd, 0) != MAP_FAILED);
  assert(mmap(&memory[capacity], capacity, prot, flags | MAP_FIXED, fd, 0) != MAP_FAILED);
  
  queue->memory   = memory;
  queue->start    = 0;
  queue->end      = 0;
  queue->capacity = capacity;
}

void* queue_front(Queue* queue) {
  return &queue->memory[queue->start];
}

void* queue_back(Queue* queue) {
  return &queue->memory[queue->end];
}

void queue_push(Queue* queue, int size) {
  queue->end = (queue->end + size) % queue->capacity;
}

void queue_pop(Queue* queue, int size) {
  queue->start = (queue->start + size) % queue->capacity;
}
