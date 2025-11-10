#include "converter.h"

#include <stdio.h>

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
      if (!GenerateOutputFilename()) {
        fprintf(stderr, "Error: %s\n", config->error_message);
      } else {
        bool success;

        if (config->convert_to_bin) {
          success =
              WriteHexToBin(config->input_filename, config->output_filename);
        } else if (config->convert_to_hex) {
          success =
              WriteBinToHex(config->input_filename, config->output_filename);
        }

        if (!success) {
          fprintf(stderr, "Error: %s\n", config->error_message);
        }

        CleanConfig();
      }
    }
  }

  return 0;
}