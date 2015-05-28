#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include "nezvm.h"

int main(int argc, char *const argv[]) {
  ParsingContext context = NULL;
  NezVMInstruction *inst = NULL;
  const char *syntax_file = NULL;
  const char *input_file = NULL;
  const char *output_type = NULL;
  int opt;
  while ((opt = getopt(argc, argv, "p:i:t:o:c:h:")) != -1) {
    switch (opt) {
    case 'p':
      syntax_file = optarg;
      break;
    case 'i':
      input_file = optarg;
      break;
    case 't':
      output_type = optarg;
      break;
    }
  }
  if (syntax_file == NULL) {
    nez_PrintErrorInfo("not input syntaxfile");
  }
  context = nez_CreateParsingContext(input_file);
  inst = nez_LoadMachineCode(context, syntax_file, "File");
  if (!strcmp(output_type, "stat")) {
    nez_ParseStat(context, inst);
  }
  nez_DisposeInstruction(inst, context->bytecode_length);
  nez_DisposeParsingContext(context);
  return 0;
}
