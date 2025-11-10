#include "converter.h"
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>

int main(int argc, char **argv) {
  printf("%d\n",ParseArguments(argc, argv));
  printf("%d:%s\n", GetConfig()->convert_to_bin, GetConfig()->input_filename);

  ConverterConfig* config = GetConfig();
  if (config->convert_to_bin && config->input_filename) {
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s.bin", config->input_filename);
    WriteHexToBin(config->input_filename, output_filename);
  }
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

bool HexToByte(HexPair pair, unsigned char *result_byte) {
  bool func_result = false;

  int high_half = CharToHex(pair.high);
  int low_half = CharToHex(pair.low);

  if (high_half != -1 && low_half != -1) {
    *result_byte = (high_half << 4) | low_half;
    func_result = true;
  }

  return func_result;
}

unsigned char HexToChar(unsigned char hex_half) {
  unsigned char func_result = 'A' + high_half -10;
  if (high_half < 10) {
    func_result = '0' + high_half;
  }
  return func_result;
}

HexPair ByteToHex(unsigned char byte) {
  unsigned char high_half = (byte >> 4) & 0x0F;
  unsigned char low_half = byte & 0x0F;

  return (HexPair) {
    HexToChar(high_half),
    HexToChar(low_half)
  }
}

HexPair MakeHexPair(char high, char low) {
  return (HexPair){high, low};
}

bool WriteHexToBin(const char* input_file_name, const char* output_file_name) {
  FILE *input_file = fopen(input_file_name, "r");
  FILE *output_file = fopen(output_file_name, "wb");

  // Cheking file opening

  int col_index = 1;
  int row_index = 1;
  int hex_index = 0;
  HexPair pair = {0};

  int symbol;
  while ((symbol = fgetc(input_file)) != EOF) {
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
        unsigned char byte;

        HexToByte(pair, &byte);
        fwrite(&byte, BYTE_SIZE, BYTE_COUNT, output_file);

        hex_index = 0;
        pair = (HexPair){0};
      }
    }
  }

  fclose(input_file);
  fclose(output_file);
}

bool WriteBinToHex(const char* input_filename, const char* output_filename) {
  FILE* input_file = fopen(input_filename, "rb");
  FILE *output_file = fopen(output_filename, "w");

  unsigned char byte_buffer[512];
  char hex_buffer[1024];
  size_t byte_buffer_size;
  while((byte_buffer_size = fread(byte_buffer, 1, sizeof(byte_buffer), input_file)) != 0) {
    for (size_t i = 0; i < byte_buffer_size; ++i) {
      HexPair pair = 
  }
} 
