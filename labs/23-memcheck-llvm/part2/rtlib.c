#include "rtlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

mem_log_t* head;

#define rtlib_output(args...) \
  do {                        \
    printf("RTLIB: ");        \
    printf(args);             \
  } while (0)

static bool log_contains_addr(mem_log_t* log, u_int8_t* addr) {
  return log->addr <= addr &&
         ((u_int64_t)log->addr + log->size) > (u_int64_t)addr;
}

void log_malloc(u_int8_t* p, size_t bytes) {
  mem_log_t* cur = head;
  // Malloc may be reusing a previously allocated block
  // that has been freed.
  while (cur != NULL) {
    if (log_contains_addr(cur, p)) {
      cur->state = ALLOC;
      return;
    }
    cur = cur->next;
  }

  cur = (mem_log_t*)malloc(sizeof(mem_log_t));
  cur->addr = p;
  cur->size = bytes;
  cur->next = NULL;
  cur->state = ALLOC;
  insert_log(&head, cur);
}

void log_free(u_int8_t* p) {
  mem_log_t* cur = head;
  while (cur != NULL) {
    if (log_contains_addr(cur, p)) {
      if (cur->state == FREE) {
        rtlib_output("Double free at address %p.\n", p);
        exit(0);
      }
      cur->state = FREE;
      return;
    }
    cur = cur->next;
  }
  rtlib_output("Freed unallocated memory at address %p.\n", p);
  exit(0);
}

void log_load(u_int8_t* p) {
  mem_log_t* cur = head;
  while (cur != NULL) {
    if (log_contains_addr(cur, p)) {
      if (cur->state == ALLOC || cur->state == STACK)
        return;
      else if (cur->state == FREE) {
        rtlib_output("Use (load) after free at address %p.\n", p);
        exit(0);
      }
    }
    cur = cur->next;
  }
  rtlib_output("Loaded from unallocated memory at address %p.\n", p);
  exit(0);
}

void log_store(u_int8_t* p) {
  mem_log_t* cur = head;
  while (cur != NULL) {
    if (log_contains_addr(cur, p)) {
      if (cur->state == ALLOC || cur->state == STACK)
        return;
      else if (cur->state == FREE) {
        rtlib_output("Use (store) after free at address %p.\n", p);
        exit(0);
      }
    }
    cur = cur->next;
  }
  rtlib_output("Stored at unallocated memory at address %p.\n", p);
  exit(0);
}

void log_stack(u_int8_t* p) {
  mem_log_t* cur = (mem_log_t*)malloc(sizeof(mem_log_t));
  cur->addr = p;
  cur->size = 1;
  cur->next = NULL;
  cur->state = STACK;
  insert_log(&head, cur);
}

void init_check() {
  head = NULL;
}

void exit_check() {
  mem_log_t* cur = head;
  mem_log_t* prev = NULL;
  while (cur != NULL) {
    if (cur->state == ALLOC) {
      rtlib_output("Memory leak at address %p.\n", cur->addr);
      exit(0);
    }
    prev = cur;
    cur = cur->next;
    free(prev);
  }
}

void insert_log(mem_log_t** l, mem_log_t* log) {
  mem_log_t* prev = *l;
  if (prev == NULL) {
    *l = log;
    log->next = NULL;
    return;
  }
  mem_log_t* next;
  while ((next = prev->next))
    prev = next;

  prev->next = log;
  log->next = NULL;
}

void print_log() {
  mem_log_t* prev = head;
  int i = 0;
  rtlib_output("****Printing Log****\n");
  while (prev != NULL) {
    i++;
    rtlib_output("Entry #%d:\n", i);
    rtlib_output("  log->addresss =%p\n", prev->addr);
    rtlib_output("  log->size     =%zu\n", prev->size);
    rtlib_output("  log->next     =%p\n", prev->next);
    prev = prev->next;
  }
  rtlib_output("******************\n");
}