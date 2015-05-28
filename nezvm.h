#include <stdint.h>

#ifndef NEZVM_H
#define NEZVM_H

#define NEZVM_DEBUG 0
#define NEZVM_PROFILE 0

#define NEZ_IR_MAX 22
#define NEZ_IR_EACH(OP)\
	OP(EXIT)\
	OP(SUCC)\
	OP(FAIL)\
	OP(JUMP)\
	OP(CALL)\
	OP(RET)\
	OP(IFFAIL)\
	OP(CHAR)\
	OP(CHARMAP)\
	OP(STRING)\
	OP(ANY)\
	OP(PUSH)\
	OP(POP)\
	OP(PEEK)\
	OP(STORE)\
	OP(NOTCHAR)\
	OP(NOTSTRING)\
	OP(OPTIONALCHARMAP)\
	OP(OPTIONALSTRING)\
	OP(ZEROMORECHARMAP)

typedef struct NezVMInstruction {
	unsigned short op : 5;
	short arg : 11;
} NezVMInstruction;

enum nezvm_opcode {
#define DEFINE_ENUM(NAME) NEZVM_OP_##NAME,
  NEZ_IR_EACH(DEFINE_ENUM)
#undef DEFINE_ENUM
  NEZVM_OP_ERROR = -1
};

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

void nez_PrintErrorInfo(const char *errmsg);

const char *get_opname(short opcode);

NezVMInstruction *nez_LoadMachineCode(ParsingContext context,
                                      const char *fileName,
                                      const char *nonTerminalName);
void nez_DisposeInstruction(NezVMInstruction *inst, long length);

void nez_Parse(ParsingContext context, NezVMInstruction *inst);
void nez_ParseStat(ParsingContext context, NezVMInstruction *inst);

#endif
