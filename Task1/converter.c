#include "converter.h"
#include <stdbool.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  ParseArguments(argc, argv);

  ConverterConfig* config = GetConfig();

  if (config->show_help) {
    PrintHelp(argv[0]);
  }

  GenerateOutputFilename();

  if (config->convert_to_bin) {
    WriteHexToBin(config->input_filename, config->output_filename);
  } else if (config->convert_to_hex) {
    WriteBinToHex(config->input_filename, config->output_filename);
  }

  CleanConfig();
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

char HexToChar(uint8_t hex_half) {
  char func_result = 'A' + hex_half - 10;
  if (hex_half < 10) {
    func_result = '0' + hex_half;
  }
  return func_result;
}

HexPair ByteToHex(uint8_t byte) {
  uint8_t high_half = (byte >> 4) & 0x0F;
  uint8_t low_half = byte & 0x0F;

  return (HexPair) {
    HexToChar(high_half),
    HexToChar(low_half)
  };
}

HexPair MakeHexPair(char high, char low) {
  return (HexPair){high, low};
}

bool WriteHexToBin(const char* input_file_name, const char* output_file_name) {
  FILE *input_file = fopen(input_file_name, "r");
  FILE *output_file = fopen(output_file_name, "wb");

  // Cheking file opening

  size_t col_index = 1;
  size_t row_index = 1;
  size_t hex_index = 0;
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
        uint8_t byte;

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

  uint8_t byte_buffer[512];
  char hex_buffer[1024];

  size_t hex_index = 0;
  size_t read_bytes_count;
  bool fail = false;

  while((read_bytes_count = fread(byte_buffer, 1, sizeof(byte_buffer), input_file)) != 0 && !fail) {
    for (size_t i = 0; i < read_bytes_count; ++i) {
      HexPair pair = ByteToHex(byte_buffer[i]);
      hex_buffer[hex_index++] = pair.high;
      hex_buffer[hex_index++] = pair.low;
    }

    if (fwrite(hex_buffer, BYTE_SIZE, hex_index, output_file) != hex_index) {
      fail = true;
    }
    hex_index = 0;
  }

  fclose(input_file);
  fclose(output_file);

  return fail;
}
