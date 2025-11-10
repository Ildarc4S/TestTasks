#include "converter.h"

#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

ConverterConfig* GetConfig() {
  static ConverterConfig config;
  static bool initialized = 0;

  if (!initialized) {
    config.convert_to_bin = false;
    config.convert_to_hex = false;
    config.show_help = false;
    config.error = false;
    config.input_filename = NULL;
    config.output_filename = NULL;
    config.error_message[0] = '\0';
    initialized = true;
  }

  return &config;
}

void CleanConfig() {
  ConverterConfig* config = GetConfig();
  if (config->output_filename != NULL) {
    free(config->output_filename);
    config->output_filename = NULL;
  }
}

bool GenerateOutputFilename() {
  int func_result = true;
  ConverterConfig* config = GetConfig();
  if (config->input_filename != NULL) {
    size_t total_filename_len = strlen(config->input_filename) + FILE_FORMAT_LEN + 1;

    config->output_filename = malloc(total_filename_len);
    if (config->output_filename != NULL) {
      const char* format = config->convert_to_bin ? "%s.bin" : "%s.hex";
      snprintf(config->output_filename, total_filename_len, format, config->input_filename);
    } else {
      func_result = false;
    }
  } else {
    func_result = false;
  }

  return func_result;
}

bool ParseArguments(int argc, char **argv) {
  int func_result = true;
  int option_used = false;
  ConverterConfig* config = GetConfig();
  int opt;
  while((opt = getopt(argc, argv, "a:b:h")) != -1 && func_result) {
    switch(opt) {
      case 'a':
        if (!option_used) {
          config->convert_to_bin = true;
          config->input_filename = optarg;
          option_used = true;
        } else {
          func_result = false;
        }
        break;
      case 'b':
        if (!option_used) {
          config->convert_to_hex = true;
          config->input_filename = optarg;
          option_used = true;
        } else {
          func_result = false;
        }
        break;
      case 'h':
        if (!option_used) {
          config->show_help = true;
          option_used = true;
        } else {
          func_result = false;
        }
        break;
      case '?':
        func_result = false;
        break;
      case ':':
        func_result = false;
        break;
    }
  }

  if (optind < argc) {
    func_result = false;
  }

  if (config->input_filename == NULL) {
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

void SetError() {
}