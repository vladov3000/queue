#include "queue.h"
#include <string.h>
#include <assert.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <immintrin.h>

void queue_init(Queue* queue, int capacity) {
  mach_vm_address_t address;
  {
    mach_port_t            task        = mach_task_self();
    mach_vm_size_t         size        = capacity;
    vm_address_t           mask        = 0;
    int                    flags       = VM_FLAGS_ANYWHERE;
    mem_entry_name_port_t  object      = MEMORY_OBJECT_NULL;
    memory_object_offset_t offset      = 0;
    boolean_t              copy        = FALSE;
    vm_prot_t              protection  = VM_PROT_READ | VM_PROT_WRITE;
    vm_inherit_t           inheritance = VM_INHERIT_SHARE;
    kern_return_t          error       = mach_vm_map(
      task, &address, size, mask, flags, object, offset, copy, protection, protection, inheritance
    );
    assert(error == KERN_SUCCESS);
  }

  {
    mach_port_t       task        = mach_task_self();
    mach_vm_address_t target      = address + capacity;
    mach_vm_size_t    size        = capacity;
    mach_vm_offset_t  mask        = 0;
    boolean_t         anywhere    = FALSE;
    mach_vm_address_t source      = address;
    boolean_t         copy        = FALSE;
    vm_prot_t         current;
    vm_prot_t         max;
    vm_inherit_t      inheritance = VM_INHERIT_SHARE;
    kern_return_t     error       = mach_vm_remap(
      task, &target, size, mask, anywhere, task, source, copy, &current, &max, inheritance
    );
    assert(error == KERN_SUCCESS);
  }
  
  queue->memory   = (char*) address;
  queue->start    = 0;
  queue->end      = 0;
  queue->capacity = capacity;
}

static int queue_full(int start, int end, int needed, int capacity) {
  return (end + needed) % capacity == start;
}

void queue_push_wait(Queue* queue, int size) {
  int end = queue->end;
  do {
    _mm_pause();
  } while(queue_full(__atomic_load_n(&queue->start, __ATOMIC_ACQUIRE), end, size, queue->capacity));
}

void* queue_back(Queue* queue) {
  return &queue->memory[queue->end];
}

void queue_push(Queue* queue, int size) {
  __atomic_store_n(&queue->end, (queue->end + size) % queue->capacity, __ATOMIC_RELEASE);
}

static int queue_empty(int start, int end, int needed, int capacity) {
  int size = (end - start + capacity) % capacity;
  return size < needed;
}

void queue_pop_wait(Queue* queue, int size) {
  int start = queue->start;
  do {
    _mm_pause();
  } while (queue_empty(start, __atomic_load_n(&queue->end, __ATOMIC_ACQUIRE), size, queue->capacity));
}

void* queue_front(Queue* queue) {
  return &queue->memory[queue->start];
}

void queue_pop(Queue* queue, int size) {
  __atomic_store_n(&queue->start, (queue->start + size) % queue->capacity, __ATOMIC_RELEASE);
}
