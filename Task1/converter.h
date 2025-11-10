#ifndef CONVERTER
#define CONVERTER

#include <stdbool.h>
#include <stdint.h>

#define ERROR_MESSAGE_LEN 256
#define BYTE_SIZE 1
#define BYTE_COUNT 1
#define FILE_FORMAT_LEN 4 // .bin or .hex

typedef struct {
  bool convert_to_hex;  // flag -b
  bool convert_to_bin;  // flag -a
  bool show_help;       // flag -h
  bool error;
  char *input_filename;
  char *output_filename;
  char error_message[ERROR_MESSAGE_LEN];
} ConverterConfig;

typedef struct {
  char high;
  char low;
} HexPair;

ConverterConfig* GetConfig();
void CleanConfig();

bool ParseArguments(int argc, char **argv);
bool GenerateOutputFilename();

void PrintHelp(const char *program_name);


int CharToHex(char symbol);
bool HexToByte(HexPair pair, uint8_t *result_byte);
bool WriteHexToBin(const char* input_file, const char* output_file);
bool WriteBinToHex(const char* input_file, const char* output_file);

HexPair MakeHexPair(char high, char low);

#endif // CONVERTER
