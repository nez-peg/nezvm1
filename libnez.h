#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef LIBNEZ_H
#define LIBNEZ_H

#include "bitset.c"
typedef struct bitset_ptr_t {
  bitset_t* set;
  int jump;
} bitset_ptr_t;

struct nezvm_string {
  unsigned len;
  char text[1];
};

typedef struct nezvm_string_ptr_t {
  union {
    struct nezvm_string* str;
    char c;
  };
  int jump;
  int8_t type;
} nezvm_string_ptr_t;

union StackEntry {
  const char* pos;
  long jmp;
};

struct ParsingContext {
  char *inputs;
  size_t input_size;
  long pos;

  long bytecode_length;
  long startPoint;

  uint8_t call_table_size;
  uint8_t set_table_size;
  uint8_t str_table_size;
  int* call_table;
  bitset_ptr_t* set_table;
  nezvm_string_ptr_t* str_table;

  size_t stack_size;
  union StackEntry* stack_pointer;
  union StackEntry* stack_pointer_base;
};

typedef struct ParsingContext *ParsingContext;
typedef union StackEntry* StackEntry;

#define PARSING_CONTEXT_MAX_STACK_LENGTH 1024
ParsingContext nez_CreateParsingContext(const char *filename);
void nez_DisposeParsingContext(ParsingContext ctx);

#endif