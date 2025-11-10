#include "converter.h"

#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ConverterConfig *GetConfig() {
  static ConverterConfig config;
  static bool initialized = 0;

  if (!initialized) {
    config.convert_to_bin = false;
    config.convert_to_hex = false;
    config.show_help = false;
    config.input_filename = NULL;
    config.output_filename = NULL;
    config.error_message[0] = '\0';
    initialized = true;
  }

  return &config;
}

void CleanConfig() {
  ConverterConfig *config = GetConfig();
  if (config->output_filename != NULL) {
    free(config->output_filename);
    config->output_filename = NULL;
  }
}

bool ParseArguments(int argc, char **argv) {
  opterr = 0;
  int func_result = true;
  int option_used = false;
  ConverterConfig *config = GetConfig();
  int opt;

  while ((opt = getopt(argc, argv, "a:b:h")) != -1 && func_result) {
    switch (opt) {
    case 'a':
      if (!option_used) {
        config->convert_to_bin = true;
        config->input_filename = optarg;
        option_used = true;
      } else {
        SetError("Only one option can be used at a time");
        func_result = false;
      }
      break;
    case 'b':
      if (!option_used) {
        config->convert_to_hex = true;
        config->input_filename = optarg;
        option_used = true;
      } else {
        SetError("Only one option can be used at a time");
        func_result = false;
      }
      break;
    case 'h':
      if (!option_used) {
        config->show_help = true;
        option_used = true;
      } else {
        SetError("Only one option can be used at a time");
        func_result = false;
      }
      break;
    case '?':
      if (optopt == 'a' || optopt == 'b') {
        SetError("To use -%c option, you need a filename", optopt);
      } else {
        SetError("Unknown option: -%c", optopt);
      }
      func_result = false;
      break;
    }
  }

  if (func_result && optind < argc) {
    if (!option_used && optind < argc) {
      SetError("Invalid argument format");
    } else {
      SetError("Number of arguments is exceeded: %s", argv[optind]);
    }
    func_result = false;
  }

  if (func_result && argc == 1) {
    SetError("No arguments provided");
    func_result = false;
  }

  return func_result;
}

bool ValidateInputExtension() {
  ConverterConfig *config = GetConfig();
  bool func_result = false;
  const char *input_ext = strrchr(config->input_filename, '.');

  if (input_ext == NULL) {
    SetError("Input file must have an extension");
  } else if (config->convert_to_bin && strcmp(input_ext, ".hex") != 0) {
    SetError("Input file for -a option must have '.hex' extension");
  } else if (config->convert_to_hex && strcmp(input_ext, ".bin") != 0) {
    SetError("Input file for -b option must have '.bin' extension");
  } else {
    func_result = true;
  }

  return func_result;
}

bool GenerateOutputFilename() {
  int func_result = true;
  ConverterConfig *config = GetConfig();
  if (config->input_filename != NULL) {
    size_t total_filename_len =
        strlen(config->input_filename) + FILE_FORMAT_LEN + 1;

    config->output_filename = malloc(total_filename_len);
    if (config->output_filename != NULL) {
      const char *format = config->convert_to_bin ? "%s.bin" : "%s.hex";
      snprintf(config->output_filename, total_filename_len, format,
               config->input_filename);
    } else {
      SetError("Memory allocation failed for output filename");
      func_result = false;
    }
  } else {
    SetError("Input filename is empty");
    func_result = false;
  }

  return func_result;
}

void PrintHelp(const char *program_name) {
  printf("Usage: %s -a FILE | -b FILE\n", program_name);
  printf("Utility for converting between HEX and BIN formats.\n");

  printf("Options:\n");
  printf("  -a FILE   hex to bin conversion\n");
  printf("  -b FILE   bin to hex conversion\n");
  printf("  -h        show help\n");

  printf("Examples:\n");
  printf("%s -a file.hex  // create file file.hex.bin\n", program_name);
  printf("%s -b file.bin  // create file file.bin.hex\n", program_name);
}

void SetError(const char *format, ...) {
  ConverterConfig *config = GetConfig();

  va_list args;
  va_start(args, format);
  vsnprintf(config->error_message, ERROR_MESSAGE_LEN, format, args);
  va_end(args);
}