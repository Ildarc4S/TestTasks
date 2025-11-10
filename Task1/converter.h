#ifndef CONVERTER
#define CONVERTER

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ERROR_MESSAGE_LEN 512
#define BYTE_SIZE 1
#define BYTE_COUNT 1
#define FILE_FORMAT_LEN 4 // .bin or .hex

typedef struct {
  bool convert_to_hex; // flag -b
  bool convert_to_bin; // flag -a
  bool show_help;      // flag -h
  char *input_filename;
  char *output_filename;
  char error_message[ERROR_MESSAGE_LEN];
} ConverterConfig;

typedef struct {
  char high;
  char low;
} HexPair;

typedef bool (*ProcessFunc)(FILE *input_file, FILE *output_file);

ConverterConfig *GetConfig();
void CleanConfig();

bool ParseArguments(int argc, char **argv);
bool GenerateOutputFilename();

void PrintHelp(const char *program_name);
void SetError(const char *format, ...);

int CharToHex(char symbol);
char HexToChar(uint8_t hex_half);
bool HexToByte(HexPair pair, uint8_t *result_byte);
HexPair ByteToHex(uint8_t byte);

bool ProcessHexPair(HexPair pair, size_t row_index, size_t col_index,
                    FILE *output_file);

bool ProcessFiles(const char *input_filename, const char *output_filename,
                  const char *input_mode, const char *output_mode,
                  ProcessFunc process_func);

bool ProcessHexToBin(FILE *input_file, FILE *output_file);
bool ProcessBinToHex(FILE *input_file, FILE *output_file);

bool WriteHexToBin(const char *input_file, const char *output_file);
bool WriteBinToHex(const char *input_file, const char *output_file);

#endif // CONVERTER
