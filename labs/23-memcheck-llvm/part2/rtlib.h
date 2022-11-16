#ifndef __RTLIB_H__
#define __RTLIB_H__

#include <stdio.h>

typedef enum { ALLOC, FREE, STACK } state_t;

typedef struct mem_log {
  struct mem_log* next;
  u_int8_t* addr;
  size_t size;
  state_t state;
} mem_log_t;

// Insert a mem_log_t * to a linked list of mem_log_t logs.
void insert_log(mem_log_t** l, mem_log_t* log);

// Logs every call to malloc: p is the pointer that malloc returns
// bytes is the size of the allocated memory.
void log_malloc(u_int8_t* p, size_t bytes);

// Logs every call to free: p is the pointer is being freed.
void log_free(u_int8_t* p);

// Checks whether a load instruction is safe.
void log_load(u_int8_t* p);

// Checks whether a store instruction is safe.
void log_store(u_int8_t* p);

// Logs memory that has been allocated to the stack. The user
// is not responsible for freeing this memory.
void log_stack(u_int8_t* p);

// Set up initial log state.
void init_check();

// Check whether all memory allocated through malloc has been freed.
// Free any memory that has been allocated for the logging functions.
void exit_check();

// Print current logs.
void print_log();

#endif