#include "converter.h"
#include "converter_core.h"
#include "utils.h"

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
  int func_result = true;
  int option_used = false;
  ConverterConfig *config = GetConfig();
  int opt;

  GetOptState state = InitGetOptState();

  while ((opt = GetOpt(argc, argv, "a:b:h", &state)) != -1 && func_result) {
    switch (opt) {
    case 'a':
      if (!option_used) {
        config->convert_to_bin = true;
        config->input_filename = state.optarg;
        option_used = true;
      } else {
        SetError("Only one option can be used at a time");
        func_result = false;
      }
      break;
    case 'b':
      if (!option_used) {
        config->convert_to_hex = true;
        config->input_filename = state.optarg;
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
      if (state.optopt == 'a' || state.optopt == 'b') {
        SetError("To use -%c option, you need a filename", state.optopt);
      } else {
        SetError("Unknown option: -%c", state.optopt);
      }
      func_result = false;
      break;
    }
  }

  if (func_result && state.optind < argc) {
    if (!option_used && state.optind < argc) {
      SetError("Invalid argument format");
    } else {
      SetError("Number of arguments is exceeded: %s", argv[state.optind]);
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

bool WriteHexToBin(const char *input_filename, const char *output_filename) {
  return ProcessFiles(input_filename, output_filename, "r", "wb",
                      ProcessHexToBin);
}

bool WriteBinToHex(const char *input_filename, const char *output_filename) {
  return ProcessFiles(input_filename, output_filename, "rb", "w",
                      ProcessBinToHex);
}

bool PerformConversion() {
  ConverterConfig *config = GetConfig();
  bool func_result;

  if (config->convert_to_bin) {
    func_result =
        WriteHexToBin(config->input_filename, config->output_filename);
  } else if (config->convert_to_hex) {
    func_result =
        WriteBinToHex(config->input_filename, config->output_filename);
  }

  return func_result;
}