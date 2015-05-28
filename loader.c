#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "nezvm.h"

typedef struct byteCodeInfo {
  int pos;
  uint8_t version0;
  uint8_t version1;
  uint32_t filename_length;
  uint8_t *filename;
  uint32_t pool_size_info;
  uint64_t bytecode_length;
} byteCodeInfo;

typedef struct ByteCodeLoader {
  char *input;
  byteCodeInfo *info;
  NezVMInstruction *head;
} ByteCodeLoader;

char *loadFile(const char *filename, size_t *length) {
  size_t len = 0;
  FILE *fp = fopen(filename, "rb");
  char *source;
  if (!fp) {
    nez_PrintErrorInfo("fopen error: cannot open file");
    return NULL;
  }
  fseek(fp, 0, SEEK_END);
  len = (size_t)ftell(fp);
  fseek(fp, 0, SEEK_SET);
  source = (char *)malloc(len + 1);
  if (len != fread(source, 1, len, fp)) {
    nez_PrintErrorInfo("fread error: cannot read file collectly");
  }
  source[len] = '\0';
  fclose(fp);
  *length = len;
  return source;
}

static short read16(char *inputs, byteCodeInfo *info) {
  uint16_t value = (uint8_t)inputs[info->pos++];
  value = (value) | ((uint8_t)inputs[info->pos++] << 8);
  return value;
}

static int read32(char *inputs, byteCodeInfo *info) {
  uint32_t value = 0;
  value = (uint8_t)inputs[info->pos++];
  value = (value) | ((uint8_t)inputs[info->pos++] << 8);
  value = (value) | ((uint8_t)inputs[info->pos++] << 16);
  value = (value) | ((uint8_t)inputs[info->pos++] << 24);
  return value;
}

static long read64(char *inputs, byteCodeInfo *info) {
  uint64_t value1 = read32(inputs, info);
  uint64_t value2 = read32(inputs, info);
  return value2 << 32 | value1;
}

static uint32_t Loader_Read32(ByteCodeLoader *loader) {
  return read32(loader->input, loader->info);
}

static uint16_t Loader_Read16(ByteCodeLoader *loader) {
  return read16(loader->input, loader->info);
}

static struct nezvm_string* Loader_ReadString(ByteCodeLoader *loader) {
  uint32_t len = Loader_Read16(loader);
  struct nezvm_string* str = (struct nezvm_string*)malloc(sizeof(*str) - 1 + len);
  str->len = len;
  for (uint32_t i = 0; i < len; i++) {
    str->text[i] = loader->input[loader->info->pos++];
  }
  return str;
}

void nez_EmitInstruction(NezVMInstruction* ir, ByteCodeLoader *loader, ParsingContext context) {
  switch(ir->op) {
    case NEZVM_OP_JUMP:
    case NEZVM_OP_IFFAIL: {
      ir->arg = Loader_Read16(loader);
      break;
    }
    case NEZVM_OP_CALL: {
      ir->arg = loader->input[loader->info->pos++];
      context->call_table[ir->arg] = Loader_Read32(loader);
      break;
    }
    case NEZVM_OP_CHAR: {
      ir->arg = loader->input[loader->info->pos++];
      break;
    }
    case NEZVM_OP_NOTCHAR: {
      ir->arg = loader->input[loader->info->pos++];
      context->str_table[ir->arg].c = loader->input[loader->info->pos++];
      context->str_table[ir->arg].jump = Loader_Read32(loader);
      context->str_table[ir->arg].type = 0;
      break;
    }
    case NEZVM_OP_CHARMAP: {
      ir->arg = loader->input[loader->info->pos++];
      int len = Loader_Read16(loader);
      context->set_table[ir->arg].set = (bitset_t *)malloc(sizeof(bitset_t));
      bitset_init(context->set_table[ir->arg].set);
      for (int i = 0; i < len; i++) {
        unsigned char c = loader->input[loader->info->pos++];
        bitset_set(context->set_table[ir->arg].set, c);
      }
      context->set_table[ir->arg].jump = Loader_Read32(loader);
      break;
    }
    case NEZVM_OP_OPTIONALCHARMAP:
    case NEZVM_OP_ZEROMORECHARMAP: {
      ir->arg = loader->input[loader->info->pos++];
      int len = Loader_Read16(loader);
      context->set_table[ir->arg].set = (bitset_t *)malloc(sizeof(bitset_t));
      bitset_init(context->set_table[ir->arg].set);
      for (int i = 0; i < len; i++) {
        unsigned c = loader->input[loader->info->pos++];
        bitset_set(context->set_table[ir->arg].set, c);
      }
      break;
    }
    case NEZVM_OP_STRING:
    case NEZVM_OP_NOTSTRING:
     {
      ir->arg = loader->input[loader->info->pos++];
      context->str_table[ir->arg].str = Loader_ReadString(loader);
      context->str_table[ir->arg].jump = Loader_Read32(loader);
      context->str_table[ir->arg].type = 1;
      break;
    }
  }
}

NezVMInstruction *nez_VM_Prepare(ParsingContext, NezVMInstruction *);

NezVMInstruction *nez_LoadMachineCode(ParsingContext context,
                                      const char *fileName,
                                      const char *nonTerminalName) {
  NezVMInstruction *inst = NULL;
  NezVMInstruction *head = NULL;
  size_t len;
  char *buf = loadFile(fileName, &len);
  byteCodeInfo info;
  info.pos = 0;

  /* load bytecode header */
  info.version0 = buf[info.pos++]; /* version info */
  info.version1 = buf[info.pos++];
  info.filename_length = read32(buf, &info); /* file name */
  info.filename = malloc(sizeof(uint8_t) * info.filename_length + 1);
  for (uint32_t i = 0; i < info.filename_length; i++) {
    info.filename[i] = buf[info.pos++];
  }
  info.filename[info.filename_length] = 0;

  /* call table size */
  context->call_table_size = read32(buf, &info);
  context->call_table = (int *)malloc(sizeof(int) * context->call_table_size);

  /* set table size */
  context->set_table_size = read32(buf, &info);
  context->set_table = (bitset_ptr_t *)malloc(sizeof(bitset_t) * context->set_table_size);

  /* str table size */
  context->str_table_size = read32(buf, &info);
  context->str_table = (nezvm_string_ptr_t *)malloc(sizeof(nezvm_string_ptr_t) * context->str_table_size);

  /* bytecode length */
  info.bytecode_length = read64(buf, &info);
  free(info.filename);

  /* 
  ** head is a tmporary variable that indecates the begining
  ** of the instruction sequence
  */
  head = inst = malloc(sizeof(*inst) * info.bytecode_length);
  memset(inst, 0, sizeof(*inst) * info.bytecode_length);

  /* init bytecode loader */
  ByteCodeLoader loader;
  loader.input = buf;
  loader.info = &info;
  loader.head = head;

  /* f_convert[] is function pointer that emit instruction */
  for (uint64_t i = 0; i < info.bytecode_length; i++) {
    inst->op = buf[info.pos++];
    nez_EmitInstruction(inst, &loader, context);
    inst++;
  }

#if PEGVM_DEBUG
  dump_NezVMInstructions(inst, info.bytecode_length);
#endif

  context->bytecode_length = info.bytecode_length;
  free(buf);
  return head;
}

void nez_DisposeInstruction(NezVMInstruction *ir, long length) {
  free(ir);
}
