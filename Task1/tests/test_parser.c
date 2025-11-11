#include "converter.h"
#include "tests.h"

START_TEST(test_parse_arguments_a_option) {
  char *argv[] = {"program", "-a", "test.hex", NULL};
  ck_assert(ParseArguments(3, argv));

  ConverterConfig *config = GetConfig();
  ck_assert(config->convert_to_bin == true);
  ck_assert_str_eq(config->input_filename, "test.hex");
}
END_TEST

START_TEST(test_parse_arguments_b_option) {
  char *argv[] = {"program", "-b", "test.bin", NULL};
  ck_assert(ParseArguments(3, argv));

  ConverterConfig *config = GetConfig();
  ck_assert(config->convert_to_hex == true);
  ck_assert_str_eq(config->input_filename, "test.bin");
}
END_TEST

START_TEST(test_parse_arguments_h_option) {
  char *argv[] = {"program", "-h", NULL};
  ck_assert(ParseArguments(2, argv));

  ConverterConfig *config = GetConfig();
  ck_assert(config->show_help == true);
}
END_TEST

START_TEST(test_parse_arguments_unknown_option) {
  char *argv[] = {"program", "-x", "test.txt", NULL};
  ck_assert(!ParseArguments(3, argv));
  ck_assert_str_eq(GetConfig()->error_message, "Unknown option: -x");
}
END_TEST

START_TEST(test_parse_arguments_missing_filename) {
  char *argv[] = {"program", "-a", NULL};
  ck_assert(!ParseArguments(2, argv));
}
END_TEST

START_TEST(test_parse_arguments_multiple_options_error) {
  char *argv[] = {"program", "-a", "test.hex", "-b", "test.bin", NULL};
  ck_assert(!ParseArguments(5, argv));
  ck_assert_str_eq(GetConfig()->error_message,
                   "Only one option can be used at a time");
}
END_TEST

START_TEST(test_parse_arguments_same_option_twice_error) {
  char *argv[] = {"program", "-a", "test1.hex", "-a", "test2.hex", NULL};
  ck_assert(!ParseArguments(5, argv));
  ck_assert_str_eq(GetConfig()->error_message,
                   "Only one option can be used at a time");
}
END_TEST

START_TEST(test_parse_arguments_help_before_other_option) {
  char *argv[] = {"program", "-h", "-h", NULL};
  bool result = ParseArguments(3, argv);
  ck_assert(!result);

  ConverterConfig *config = GetConfig();
  ck_assert_str_eq(config->error_message,
                   "Only one option can be used at a time");
  CleanConfig();
}
END_TEST

START_TEST(test_parse_arguments_no_arguments) {
  char *argv[] = {"program", NULL};
  ck_assert(!ParseArguments(1, argv));
  ck_assert_str_eq(GetConfig()->error_message, "No arguments provided");
}
END_TEST

START_TEST(test_parse_arguments_extra_arguments_error) {
  char *argv[] = {"program", "-a", "test.hex", "extra_arg", NULL};
  ck_assert(!ParseArguments(4, argv));
  ck_assert_str_eq(GetConfig()->error_message,
                   "Number of arguments is exceeded: extra_arg");
}
END_TEST

START_TEST(test_parse_arguments_only_dash) {
  char *argv[] = {"program", "-", NULL};
  ck_assert(!ParseArguments(2, argv));
}
END_TEST

START_TEST(test_parse_arguments_double_dash) {
  char *argv[] = {"program", "--", "filename", NULL};
  ck_assert(!ParseArguments(3, argv));
}
END_TEST

START_TEST(test_parse_arguments_combined_flags_invalid) {
  char *argv[] = {"program", "-ab", "test.hex", NULL};
  ck_assert(!ParseArguments(3, argv));
}
END_TEST

START_TEST(test_parse_arguments_option_without_value) {
  char *argv[] = {"program", "-a", "-b", NULL};
  ck_assert(ParseArguments(3, argv));
}
END_TEST

START_TEST(test_parse_arguments_empty_filename) {
  char *argv[] = {"program", "-a", "", NULL};
  ck_assert(ParseArguments(3, argv));
  ConverterConfig *config = GetConfig();
  ck_assert_str_eq(config->input_filename, "");
}
END_TEST

START_TEST(test_parse_arguments_space_in_filename) {
  char *argv[] = {"program", "-a", "file name.hex", NULL};
  ck_assert(ParseArguments(3, argv));
  ConverterConfig *config = GetConfig();
  ck_assert_str_eq(config->input_filename, "file name.hex");
}
END_TEST

Suite *ParserSuite(void) {
  Suite *s = suite_create("Parser");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_parse_arguments_a_option);
  tcase_add_test(tc_core, test_parse_arguments_b_option);
  tcase_add_test(tc_core, test_parse_arguments_h_option);
  tcase_add_test(tc_core, test_parse_arguments_unknown_option);
  tcase_add_test(tc_core, test_parse_arguments_missing_filename);
  tcase_add_test(tc_core, test_parse_arguments_multiple_options_error);
  tcase_add_test(tc_core, test_parse_arguments_help_before_other_option);
  tcase_add_test(tc_core, test_parse_arguments_same_option_twice_error);
  tcase_add_test(tc_core, test_parse_arguments_no_arguments);
  tcase_add_test(tc_core, test_parse_arguments_extra_arguments_error);
  tcase_add_test(tc_core, test_parse_arguments_only_dash);
  tcase_add_test(tc_core, test_parse_arguments_double_dash);
  tcase_add_test(tc_core, test_parse_arguments_combined_flags_invalid);
  tcase_add_test(tc_core, test_parse_arguments_option_without_value);
  tcase_add_test(tc_core, test_parse_arguments_empty_filename);
  tcase_add_test(tc_core, test_parse_arguments_space_in_filename);

  suite_add_tcase(s, tc_core);
  return s;
}
