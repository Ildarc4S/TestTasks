#ifndef CONVERTER_H
#define CONVERTER_H

#include <stdbool.h>

#define ERROR_MESSAGE_LEN 512
#define FILE_FORMAT_LEN 4  // .bin or .hex

typedef struct {
  bool convert_to_hex;  // flag -b
  bool convert_to_bin;  // flag -a
  bool show_help;       // flag -h
  char *input_filename;
  char *output_filename;
  char error_message[ERROR_MESSAGE_LEN];
} ConverterConfig;

ConverterConfig *GetConfig();
void CleanConfig();
bool ParseArguments(int argc, char **argv);
bool ValidateInputExtension();
bool GenerateOutputFilename();
void PrintHelp(const char *program_name);
bool PerformConversion();

bool WriteHexToBin(const char *input_file, const char *output_file);
bool WriteBinToHex(const char *input_file, const char *output_file);

#endif  // CONVERTER_H
