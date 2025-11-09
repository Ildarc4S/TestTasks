#ifndef CONVERTER
#define CONVERTER

#include <stdbool.h>

#define ERROR_MESSAGE_LEN 256

typedef struct {
  bool convert_to_hex;  // flag -b
  bool convert_to_bin;  // flag -a
  bool show_help;       // flag -h
  bool error;
  char *input_filename;
  char *output_filename;
  char error_message[ERROR_MESSAGE_LEN];
} ConverterConfig;

ConverterConfig* GetConfig();

bool ParseArguments(int argc, char **argv);

#endif // CONVERTER
