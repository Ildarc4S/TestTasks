#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../include/converter.h"

START_TEST(test_validate_input_extension_hex_to_bin_valid) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.hex";

  ck_assert(ValidateInputExtension());
  CleanConfig();
}
END_TEST

START_TEST(test_validate_input_extension_bin_to_hex_valid) {
  ConverterConfig *config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "test.bin";

  ck_assert(ValidateInputExtension());
  CleanConfig();
}
END_TEST

START_TEST(test_validate_input_extension_hex_uppercase) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "TEST.HEX";

  ck_assert(!ValidateInputExtension());
}
END_TEST

START_TEST(test_validate_input_extension_bin_uppercase) {
  ConverterConfig *config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "TEST.BIN";

  ck_assert(!ValidateInputExtension());
}
END_TEST

START_TEST(test_validate_input_extension_wrong_extension_for_bin) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.txt";

  ck_assert(!ValidateInputExtension());
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file for -a option must have '.hex' extension");
  CleanConfig();
}
END_TEST

START_TEST(test_validate_input_extension_wrong_extension_for_hex) {
  ConverterConfig *config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "test.txt";

  ck_assert(!ValidateInputExtension());
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file for -b option must have '.bin' extension");
  CleanConfig();
}
END_TEST

START_TEST(test_validate_input_extension_no_extension) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test";

  ck_assert(!ValidateInputExtension());
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file must have an extension");
}
END_TEST

START_TEST(test_validate_input_extension_empty_filename) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "";

  ck_assert(!ValidateInputExtension());
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file must have an extension");
}
END_TEST

START_TEST(test_validate_input_extension_multiple_dots) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "archive.tar.hex";

  ck_assert(ValidateInputExtension());
  CleanConfig();
}
END_TEST

START_TEST(test_validate_input_extension_dot_at_end) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.";

  ck_assert(!ValidateInputExtension());
  ck_assert_str_eq(GetConfig()->error_message,
                   "Input file for -a option must have '.hex' extension");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_bin) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.hex";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "test.hex.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_hex) {
  ConverterConfig *config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "test.bin";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "test.bin.hex");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_with_path) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "/path/to/file.hex";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "/path/to/file.hex.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_relative_path) {
  ConverterConfig *config = GetConfig();
  config->convert_to_hex = true;
  config->input_filename = "../files/data.bin";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "../files/data.bin.hex");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_multiple_dots) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "archive.tar.hex";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "archive.tar.hex.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_no_extension) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test";

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "test.bin");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_empty_input) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = NULL;

  ck_assert(!GenerateOutputFilename());
  ck_assert_str_eq(GetConfig()->error_message, "Input filename is empty");
  CleanConfig();
}
END_TEST

START_TEST(test_generate_output_filename_memory_allocation) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;

  char long_filename[512];
  memset(long_filename, 'a', 500);
  long_filename[500] = '\0';
  strcat(long_filename, ".hex");

  config->input_filename = long_filename;

  ck_assert(GenerateOutputFilename());
  ck_assert_ptr_nonnull(config->output_filename);
  ck_assert_str_eq(
      config->output_filename + strlen(config->output_filename) - 8,
      ".hex.bin");

  CleanConfig();
}
END_TEST

START_TEST(test_validation_flow_success) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.hex";

  ck_assert(ValidateInputExtension());

  ck_assert(GenerateOutputFilename());
  ck_assert_str_eq(config->output_filename, "test.hex.bin");

  CleanConfig();
}
END_TEST

START_TEST(test_validation_flow_failure) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.txt";

  ck_assert(!ValidateInputExtension());

  CleanConfig();
}
END_TEST

START_TEST(test_clean_config) {
  ConverterConfig *config = GetConfig();
  config->convert_to_bin = true;
  config->input_filename = "test.hex";
  strcpy(config->error_message, "Some error");

  ck_assert(GenerateOutputFilename());
  ck_assert_ptr_nonnull(config->output_filename);

  CleanConfig();

  ck_assert_ptr_eq(config->output_filename, NULL);
  ck_assert(config->convert_to_bin == false);
  ck_assert(config->convert_to_hex == false);
  ck_assert(config->show_help == false);
  ck_assert_ptr_eq(config->input_filename, NULL);
  ck_assert_str_eq(config->error_message, "");
}
END_TEST

Suite *ValidationSuite(void) {
  Suite *s = suite_create("Validation");
  TCase *tc_extension = tcase_create("InputExtension");
  TCase *tc_output_filename = tcase_create("OutputFilename");
  TCase *tc_cleanup = tcase_create("Cleanup");

  tcase_add_test(tc_extension, test_validate_input_extension_hex_to_bin_valid);
  tcase_add_test(tc_extension, test_validate_input_extension_bin_to_hex_valid);
  tcase_add_test(tc_extension, test_validate_input_extension_hex_uppercase);
  tcase_add_test(tc_extension, test_validate_input_extension_bin_uppercase);
  tcase_add_test(tc_extension,
                 test_validate_input_extension_wrong_extension_for_bin);
  tcase_add_test(tc_extension,
                 test_validate_input_extension_wrong_extension_for_hex);
  tcase_add_test(tc_extension, test_validate_input_extension_no_extension);
  tcase_add_test(tc_extension, test_validate_input_extension_empty_filename);
  tcase_add_test(tc_extension, test_validate_input_extension_multiple_dots);
  tcase_add_test(tc_extension, test_validate_input_extension_dot_at_end);
  tcase_add_test(tc_extension, test_validation_flow_success);
  tcase_add_test(tc_extension, test_validation_flow_failure);

  tcase_add_test(tc_output_filename, test_generate_output_filename_bin);
  tcase_add_test(tc_output_filename, test_generate_output_filename_hex);
  tcase_add_test(tc_output_filename, test_generate_output_filename_with_path);
  tcase_add_test(tc_output_filename,
                 test_generate_output_filename_relative_path);
  tcase_add_test(tc_output_filename,
                 test_generate_output_filename_multiple_dots);
  tcase_add_test(tc_output_filename,
                 test_generate_output_filename_no_extension);
  tcase_add_test(tc_output_filename, test_generate_output_filename_empty_input);
  tcase_add_test(tc_output_filename,
                 test_generate_output_filename_memory_allocation);

  tcase_add_test(tc_cleanup, test_clean_config);

  suite_add_tcase(s, tc_extension);
  suite_add_tcase(s, tc_output_filename);
  suite_add_tcase(s, tc_cleanup);

  return s;
}
