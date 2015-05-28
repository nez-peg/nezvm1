#include "libnez.h"
#include "nezvm.h"
#include <stdio.h>

char *loadFile(const char *filename, size_t *length);

ParsingContext nez_CreateParsingContext(const char *filename) {
  ParsingContext ctx = (ParsingContext)malloc(sizeof(struct ParsingContext));
  ctx->pos = ctx->input_size = 0;
  ctx->inputs = loadFile(filename, &ctx->input_size);
  ctx->stack_pointer_base =
      (StackEntry)malloc(sizeof(union StackEntry) * PARSING_CONTEXT_MAX_STACK_LENGTH);
  ctx->stack_pointer = &ctx->stack_pointer_base[0];
  ctx->stack_size = PARSING_CONTEXT_MAX_STACK_LENGTH;
  return ctx;
}

void nez_DisposeParsingContext(ParsingContext ctx) {
  free(ctx->call_table);
  for(int i = 0; i < ctx->set_table_size; i++) {
    free(ctx->set_table[i].set);
  }
  free(ctx->set_table);
  for(int i = 0; i < ctx->str_table_size; i++) {
    if(ctx->str_table[i].type == 1) {
      free(ctx->str_table[i].str);
    }
  }
  free(ctx->str_table);
  free(ctx->inputs);
  free(ctx->stack_pointer_base);
  free(ctx);
}