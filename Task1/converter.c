#include "converter.h"
#include <stdio.h>

int CharToHex(char symbol) {
  int func_result = -1;
  if ('0' <= symbol && symbol <= '9') {
    func_result = symbol - '0';
  } else if ('a' <= symbol && symbol <= 'f') {
    func_result = symbol - 'a' + 10;
  } else if ('A' <= symbol && symbol <= 'F') {
    func_result = symbol - 'A' + 10;
  }

  return func_result;
}

char HexToChar(uint8_t hex_half) {
  char func_result = 'A' + hex_half - 10;
  if (hex_half < 10) {
    func_result = '0' + hex_half;
  }
  return func_result;
}

bool HexToByte(HexPair pair, uint8_t *result_byte) {
  bool func_result = false;

  int high_half = CharToHex(pair.high);
  int low_half = CharToHex(pair.low);

  if (high_half != -1 && low_half != -1) {
    *result_byte = (high_half << 4) | low_half;
    func_result = true;
  }

  return func_result;
}

HexPair ByteToHex(uint8_t byte) {
  uint8_t high_half = (byte >> 4) & 0x0F;
  uint8_t low_half = byte & 0x0F;

  return (HexPair){HexToChar(high_half), HexToChar(low_half)};
}

bool ProcessFiles(const char *input_filename, const char *output_filename,
                  const char *input_mode, const char *output_mode,
                  ProcessFunc process_func) {
  FILE *input_file = fopen(input_filename, input_mode);

  bool func_result = false;

  if (!input_file) {
    SetError("Cannot open input file: '%s'", input_filename);
  } else {
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    if (file_size == 0) {
      SetError("Input file is empty: '%s'", input_filename);
      fclose(input_file);
    } else {
      FILE *output_file = fopen(output_filename, output_mode);
      if (!output_file) {
        SetError("Cannot open output file: '%s'", output_filename);
        fclose(input_file);
      } else {
        func_result = process_func(input_file, output_file);
        fclose(input_file);
        fclose(output_file);
      }
    }
  }

  return func_result;
}

bool ProcessHexPair(HexPair pair, size_t row_index, size_t col_index,
                    FILE *output_file) {
  uint8_t byte;
  bool func_result = true;

  if (!HexToByte(pair, &byte)) {
    SetError("Invalid HEX pair at row %zu, col %zu: '%c%c'", row_index,
             col_index - 2, pair.high, pair.low);
    func_result = false;
  }
  if (func_result && fwrite(&byte, BYTE_SIZE, BYTE_COUNT, output_file) != 1) {
    SetError("Cannot write to output file");
    func_result = false;
  }

  return func_result;
}

bool ProcessHexToBin(FILE *input_file, FILE *output_file) {
  size_t col_index = 1;
  size_t row_index = 1;
  size_t hex_index = 0;
  HexPair pair = {0};
  bool func_result = true;

  int symbol;
  while (func_result && (symbol = fgetc(input_file)) != EOF) {
    if (symbol == ' ') {
      col_index++;
    } else if (symbol == '\n') {
      row_index++;
      col_index = 1;
    } else {

      if (hex_index == 0) {
        pair.high = symbol;
      } else {
        pair.low = symbol;
      }

      col_index++;
      hex_index++;

      if (hex_index == 2) {
        func_result =
            ProcessHexPair(pair, row_index, col_index - 2, output_file);
        hex_index = 0;
        pair = (HexPair){0};
      }
    }
  }

  if (!func_result && hex_index == 1) {
    SetError("Unpair HEX character at end of input file");
    func_result = false;
  }

  return func_result;
}

bool ProcessBinToHex(FILE *input_file, FILE *output_file) {
  uint8_t byte_buffer[512];
  char hex_buffer[1024];
  size_t hex_index = 0;
  size_t read_bytes_count;
  bool func_result = true;
  while (func_result &&
         (read_bytes_count =
              fread(byte_buffer, 1, sizeof(byte_buffer), input_file)) != 0) {
    for (size_t i = 0; i < read_bytes_count; ++i) {
      HexPair pair = ByteToHex(byte_buffer[i]);
      hex_buffer[hex_index++] = pair.high;
      hex_buffer[hex_index++] = pair.low;
    }

    if (fwrite(hex_buffer, BYTE_SIZE, hex_index, output_file) != hex_index) {
      SetError("Cannot write to output file");
      func_result = false;
    }
    hex_index = 0;
  }

  return func_result;
}

bool WriteHexToBin(const char *input_filename, const char *output_filename) {
  return ProcessFiles(input_filename, output_filename, "r", "wb",
                      ProcessHexToBin);
}

bool WriteBinToHex(const char *input_filename, const char *output_filename) {
  return ProcessFiles(input_filename, output_filename, "rb", "w",
                      ProcessBinToHex);
}
