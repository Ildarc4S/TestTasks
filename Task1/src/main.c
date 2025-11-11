#include <stdio.h>

#include "converter.h"

int main(int argc, char **argv) {
  if (!ParseArguments(argc, argv)) {
    ConverterConfig *config = GetConfig();
    fprintf(stderr, "Error: %s\n", config->error_message);
    fprintf(stderr, "Use: '%s -h' for help\n", argv[0]);
  } else {
    ConverterConfig *config = GetConfig();

    if (config->show_help) {
      PrintHelp(argv[0]);
    } else {
      if (!ValidateInputExtension()) {
        fprintf(stderr, "Error: %s\n", config->error_message);
      } else if (!GenerateOutputFilename()) {
        fprintf(stderr, "Error: %s\n", config->error_message);
      } else {
        if (!PerformConversion()) {
          fprintf(stderr, "Error: %s\n", config->error_message);
        }
        CleanConfig();
      }
    }
  }

  return 0;
}