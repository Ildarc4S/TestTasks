#ifndef CONVERTER_CORE_H
#define CONVERTER_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define BYTE_SIZE 1
#define BYTE_COUNT 1

typedef struct {
  char high;
  char low;
} HexPair;

typedef bool (*ProcessFunc)(FILE *input_file, FILE *output_file);

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

#endif // CONVERTER_CORE_H
