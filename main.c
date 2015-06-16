#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include "nezvm.h"

static void nez_ShowUsage(const char *file) {
  // fprintf(stderr, "Usage: %s -f nez_bytecode target_file\n", file);
  fprintf(stderr, "\nnezvm <command> optional files\n");
  fprintf(stderr, "  -p <filename> Specify an PEGs grammar bytecode file\n");
  fprintf(stderr, "  -i <filename> Specify an input file\n");
  fprintf(stderr, "  -o <filename> Specify an output file\n");
  fprintf(stderr, "  -t <type>     Specify an output type\n");
  fprintf(stderr, "  -h            Display this help and exit\n\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *const argv[]) {
  ParsingContext context = NULL;
  NezVMInstruction *inst = NULL;
  const char *syntax_file = NULL;
  const char *input_file = NULL;
  const char *output_type = NULL;
  const char *orig_argv0 = argv[0];
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
    case 'h':
      nez_ShowUsage(orig_argv0);
    default: /* '?' */
      nez_ShowUsage(orig_argv0);
    }
  }
  if (syntax_file == NULL) {
    nez_PrintErrorInfo("not input syntaxfile");
  }
  context = nez_CreateParsingContext(input_file);
  inst = nez_LoadMachineCode(context, syntax_file, "File");
  if(output_type == NULL) {
    nez_Match(context, inst);
  }
  else if (!strcmp(output_type, "stat")) {
    nez_ParseStat(context, inst, input_file);
  }
  nez_DisposeInstruction(inst, context->bytecode_length);
  nez_DisposeParsingContext(context);
  return 0;
}
