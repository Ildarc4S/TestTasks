#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/converter.h"

START_TEST(test_get_config_singleton) {
  ConverterConfig* config1 = GetConfig();
  ConverterConfig* config2 = GetConfig();

  ck_assert_ptr_eq(config1, config2);
}
END_TEST

START_TEST(test_get_config_initialization) {
  ConverterConfig* config = GetConfig();

  ck_assert(config->convert_to_bin == false);
  ck_assert(config->convert_to_hex == false);
  ck_assert(config->show_help == false);
  ck_assert_ptr_eq(config->input_filename, NULL);
  ck_assert_ptr_eq(config->output_filename, NULL);
  ck_assert_str_eq(config->error_message, "");
}
END_TEST

START_TEST(test_get_config_persistence) {
  ConverterConfig* config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.hex";

  ConverterConfig* config2 = GetConfig();
  ck_assert(config2->convert_to_bin == true);
  ck_assert_str_eq(config2->input_filename, "test.hex");

  CleanConfig();
}
END_TEST

START_TEST(test_print_help_output) {
  FILE* original_stdout = stdout;
  FILE* test_output = tmpfile();
  stdout = test_output;

  PrintHelp("test_program");

  stdout = original_stdout;

  rewind(test_output);
  char buffer[1024];
  size_t total_read = 0;
  char* output = malloc(1024);
  output[0] = '\0';

  while (fgets(buffer, sizeof(buffer), test_output)) {
    strcat(output, buffer);
    total_read += strlen(buffer);
  }

  ck_assert_ptr_nonnull(strstr(output, "Usage: test_program"));
  ck_assert_ptr_nonnull(strstr(output, "-a FILE"));
  ck_assert_ptr_nonnull(strstr(output, "-b FILE"));
  ck_assert_ptr_nonnull(strstr(output, "-h"));
  ck_assert_ptr_nonnull(strstr(output, "test_program -a file.hex"));
  ck_assert_ptr_nonnull(strstr(output, "test_program -b file.bin"));

  free(output);
  fclose(test_output);
}
END_TEST

START_TEST(test_write_hex_to_bin_success) {
  FILE* hex_file = fopen("test_hex_input.hex", "w");
  fputs("A1 B2 C3", hex_file);
  fclose(hex_file);

  bool result = WriteHexToBin("test_hex_input.hex", "test_bin_output.bin");
  ck_assert(result);

  FILE* output = fopen("test_bin_output.bin", "rb");
  ck_assert_ptr_nonnull(output);

  fseek(output, 0, SEEK_END);
  long size = ftell(output);
  ck_assert_int_gt(size, 0);
  fclose(output);

  remove("test_hex_input.hex");
  remove("test_bin_output.bin");
}
END_TEST

START_TEST(test_write_bin_to_hex_success) {
  FILE* bin_file = fopen("test_bin_input.bin", "wb");
  uint8_t data[] = {0xA1, 0xB2, 0xC3};
  fwrite(data, 1, 3, bin_file);
  fclose(bin_file);

  bool result = WriteBinToHex("test_bin_input.bin", "test_hex_output.hex");
  ck_assert(result);

  FILE* output = fopen("test_hex_output.hex", "r");
  ck_assert_ptr_nonnull(output);
  fclose(output);

  remove("test_bin_input.bin");
  remove("test_hex_output.hex");
}
END_TEST

START_TEST(test_write_hex_to_bin_file_not_found) {
  bool result = WriteHexToBin("nonexistent.hex", "output.bin");
  ck_assert(!result);

  const char* error_msg = GetConfig()->error_message;
  ck_assert_ptr_nonnull(strstr(error_msg, "Cannot open"));
  ck_assert_ptr_nonnull(strstr(error_msg, "nonexistent.hex"));

  CleanConfig();
}
END_TEST

START_TEST(test_write_bin_to_hex_file_not_found) {
  bool result = WriteBinToHex("nonexistent.bin", "output.hex");
  ck_assert(!result);

  const char* error_msg = GetConfig()->error_message;
  ck_assert_ptr_nonnull(strstr(error_msg, "Cannot open"));
  ck_assert_ptr_nonnull(strstr(error_msg, "nonexistent.bin"));

  CleanConfig();
}
END_TEST

START_TEST(test_perform_conversion_hex_to_bin) {
  ConverterConfig* config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test_input.hex";

  FILE* input = fopen("test_input.hex", "w");
  fputs("A1 B2 C3", input);
  fclose(input);

  bool gen_result = GenerateOutputFilename();
  ck_assert(gen_result);
  ck_assert_ptr_nonnull(config->output_filename);
  ck_assert_str_eq(config->output_filename, "test_input.hex.bin");

  bool result = PerformConversion();
  ck_assert(result);

  FILE* output = fopen("test_input.hex.bin", "rb");
  ck_assert_ptr_nonnull(output);

  fseek(output, 0, SEEK_END);
  long size = ftell(output);
  ck_assert_int_eq(size, 3);
  fclose(output);

  remove("test_input.hex");
  remove("test_input.hex.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_perform_conversion_bin_to_hex) {
  ConverterConfig* config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "test_input.bin";

  FILE* input = fopen("test_input.bin", "wb");
  uint8_t data[] = {0xA1, 0xB2};
  fwrite(data, 1, 2, input);
  fclose(input);

  bool gen_result = GenerateOutputFilename();
  ck_assert(gen_result);
  ck_assert_ptr_nonnull(config->output_filename);
  ck_assert_str_eq(config->output_filename, "test_input.bin.hex");

  bool result = PerformConversion();
  ck_assert(result);

  FILE* output = fopen("test_input.bin.hex", "r");
  ck_assert_ptr_nonnull(output);
  fclose(output);

  remove("test_input.bin");
  remove("test_input.bin.hex");
  CleanConfig();
}
END_TEST

START_TEST(test_perform_conversion_file_error) {
  ConverterConfig* config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "nonexistent.hex";

  bool gen_result = GenerateOutputFilename();
  ck_assert(gen_result);
  ck_assert_ptr_nonnull(config->output_filename);
  ck_assert_str_eq(config->output_filename, "nonexistent.hex.bin");

  bool result = PerformConversion();
  ck_assert(!result);

  ck_assert_str_eq(GetConfig()->error_message,
                   "Cannot open input file: 'nonexistent.hex'");

  CleanConfig();
}
END_TEST

START_TEST(test_full_workflow_hex_to_bin) {
  FILE* input = fopen("workflow_test.hex", "w");
  fputs("A1 B2 C3", input);
  fclose(input);

  char* argv[] = {"program", "-a", "workflow_test.hex", NULL};
  bool parse_result = ParseArguments(3, argv);
  ck_assert(parse_result);

  bool validation_result = ValidateInputExtension();
  ck_assert(validation_result);

  bool generation_result = GenerateOutputFilename();
  ck_assert(generation_result);

  bool conversion_result = PerformConversion();
  ck_assert(conversion_result);

  FILE* output = fopen("workflow_test.hex.bin", "rb");
  ck_assert_ptr_nonnull(output);
  fclose(output);

  remove("workflow_test.hex");
  remove("workflow_test.hex.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_full_workflow_bin_to_hex) {
  FILE* input = fopen("workflow_test.bin", "wb");
  uint8_t data[] = {0xA1, 0xB2, 0xC3};
  fwrite(data, 1, 3, input);
  fclose(input);

  char* argv[] = {"program", "-b", "workflow_test.bin", NULL};
  bool parse_result = ParseArguments(3, argv);
  ck_assert(parse_result);

  bool validation_result = ValidateInputExtension();
  ck_assert(validation_result);

  bool generation_result = GenerateOutputFilename();
  ck_assert(generation_result);

  bool conversion_result = PerformConversion();
  ck_assert(conversion_result);

  FILE* output = fopen("workflow_test.bin.hex", "r");
  ck_assert_ptr_nonnull(output);
  fclose(output);

  remove("workflow_test.bin");
  remove("workflow_test.bin.hex");
  CleanConfig();
}
END_TEST

START_TEST(test_full_workflow_with_validation_error) {
  FILE* input = fopen("workflow_test.txt", "w");
  fputs("A1 B2", input);
  fclose(input);

  char* argv[] = {"program", "-a", "workflow_test.txt", NULL};
  bool parse_result = ParseArguments(3, argv);
  ck_assert(parse_result);

  bool validation_result = ValidateInputExtension();
  ck_assert(!validation_result);
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file for -a option must have '.hex' extension");

  remove("workflow_test.txt");
  CleanConfig();
}
END_TEST

Suite* IntegrationSuite(void) {
  Suite* s = suite_create("Integration");
  TCase* tc_config = tcase_create("GetConfig");
  TCase* tc_help = tcase_create("PrintHelp");
  TCase* tc_wrappers = tcase_create("Wrappers");
  TCase* tc_perform_conversion = tcase_create("PerformConversion");
  TCase* tc_full_workflow = tcase_create("FullWorkflow");

  tcase_add_test(tc_config, test_get_config_singleton);
  tcase_add_test(tc_config, test_get_config_initialization);
  tcase_add_test(tc_config, test_get_config_persistence);

  tcase_add_test(tc_help, test_print_help_output);

  tcase_add_test(tc_wrappers, test_write_hex_to_bin_success);
  tcase_add_test(tc_wrappers, test_write_bin_to_hex_success);
  tcase_add_test(tc_wrappers, test_write_hex_to_bin_file_not_found);
  tcase_add_test(tc_wrappers, test_write_bin_to_hex_file_not_found);

  tcase_add_test(tc_perform_conversion, test_perform_conversion_hex_to_bin);
  tcase_add_test(tc_perform_conversion, test_perform_conversion_bin_to_hex);
  tcase_add_test(tc_perform_conversion, test_perform_conversion_file_error);

  tcase_add_test(tc_full_workflow, test_full_workflow_hex_to_bin);
  tcase_add_test(tc_full_workflow, test_full_workflow_bin_to_hex);
  tcase_add_test(tc_full_workflow, test_full_workflow_with_validation_error);

  suite_add_tcase(s, tc_config);
  suite_add_tcase(s, tc_help);
  suite_add_tcase(s, tc_wrappers);
  suite_add_tcase(s, tc_perform_conversion);
  suite_add_tcase(s, tc_full_workflow);

  return s;
}
