#include "converter.h"
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>

int main(int argc, char **argv) {
  printf("%d\n",ParseArguments(argc, argv));
  printf("%d:%s\n", GetConfig()->convert_to_bin, GetConfig()->input_filename);
}

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

