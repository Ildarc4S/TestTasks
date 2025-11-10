#include "converter.h"

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