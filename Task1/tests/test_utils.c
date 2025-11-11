#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "../include/converter.h"
#include "../include/utils.h"

START_TEST(test_set_error_basic) {
  SetError("Simple error message");
  ck_assert_str_eq(GetConfig()->error_message, "Simple error message");
  CleanConfig();
}
END_TEST

START_TEST(test_set_error_with_format) {
  SetError("Error in %s at line %d", "function", 42);
  ck_assert_str_eq(GetConfig()->error_message, "Error in function at line 42");
  CleanConfig();
}
END_TEST

START_TEST(test_set_error_with_multiple_args) {
  SetError("File '%s' has size %ld bytes", "test.txt", 1024L);
  ck_assert_str_eq(GetConfig()->error_message,
                   "File 'test.txt' has size 1024 bytes");
  CleanConfig();
}
END_TEST

START_TEST(test_set_error_empty) {
  SetError("");
  ck_assert_str_eq(GetConfig()->error_message, "");
  CleanConfig();
}
END_TEST

START_TEST(test_set_error_truncation) {
  char long_message[ERROR_MESSAGE_LEN * 2];
  memset(long_message, 'A', sizeof(long_message) - 1);
  long_message[sizeof(long_message) - 1] = '\0';

  SetError("%s", long_message);

  ck_assert_int_eq(strlen(GetConfig()->error_message), ERROR_MESSAGE_LEN - 1);
  CleanConfig();
}
END_TEST

START_TEST(test_init_getopt_state) {
  GetOptState state = InitGetOptState();

  ck_assert_ptr_eq(state.optarg, NULL);
  ck_assert_int_eq(state.optind, GETOPT_INITIAL_OPTIND);
  ck_assert_int_eq(state.optpos, GETOPT_INITIAL_OPTPOS);
  ck_assert_int_eq(state.optopt, GETOPT_INITIAL_OPTOPT);
}
END_TEST

START_TEST(test_getopt_simple_option) {
  char *argv[] = {"program", "-a", "file.hex", NULL};
  int argc = 3;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, 'a');
  ck_assert_str_eq(state.optarg, "file.hex");
  ck_assert_int_eq(state.optind, 3);
}
END_TEST

START_TEST(test_getopt_option_without_arg) {
  char *argv[] = {"program", "-h", NULL};
  int argc = 2;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "h", &state);
  ck_assert_int_eq(opt, 'h');
  ck_assert_ptr_eq(state.optarg, NULL);
  ck_assert_int_eq(state.optind, 2);
}
END_TEST

START_TEST(test_getopt_unknown_option) {
  char *argv[] = {"program", "-x", NULL};
  int argc = 2;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "ab", &state);
  ck_assert_int_eq(opt, '?');
  ck_assert_int_eq(state.optopt, 'x');
}
END_TEST

START_TEST(test_getopt_missing_argument) {
  char *argv[] = {"program", "-a", NULL};
  int argc = 2;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, '?');
  ck_assert_int_eq(state.optopt, 'a');
}
END_TEST

START_TEST(test_getopt_combined_options) {
  char *argv[] = {"program", "-ab", "file", NULL};
  int argc = 3;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "ab:", &state);
  ck_assert_int_eq(opt, 'a');
  ck_assert_ptr_eq(state.optarg, NULL);
  ck_assert_int_eq(state.optind, 1);
  ck_assert_int_eq(state.optpos, 2);

  opt = GetOpt(argc, argv, "ab:", &state);
  ck_assert_int_eq(opt, 'b');
  ck_assert_str_eq(state.optarg, "file");
  ck_assert_int_eq(state.optind, 3);
}
END_TEST

START_TEST(test_getopt_argument_attached) {
  char *argv[] = {"program", "-afile.hex", NULL};
  int argc = 2;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, 'a');
  ck_assert_str_eq(state.optarg, "file.hex");
  ck_assert_int_eq(state.optind, 2);
}
END_TEST

START_TEST(test_getopt_no_more_options) {
  char *argv[] = {"program", "filename", NULL};
  int argc = 2;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, -1);
}
END_TEST

START_TEST(test_getopt_only_dash) {
  char *argv[] = {"program", "-", "file", NULL};
  int argc = 3;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, -1);
}
END_TEST

START_TEST(test_getopt_double_dash) {
  char *argv[] = {"program", "--", "file", NULL};
  int argc = 3;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, '?');
  ck_assert_int_eq(state.optopt, '-');
}
END_TEST

START_TEST(test_getopt_multiple_calls) {
  char *argv[] = {"program", "-a", "file1", "-b", "file2", NULL};
  int argc = 5;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:b:", &state);
  ck_assert_int_eq(opt, 'a');
  ck_assert_str_eq(state.optarg, "file1");

  opt = GetOpt(argc, argv, "a:b:", &state);
  ck_assert_int_eq(opt, 'b');
  ck_assert_str_eq(state.optarg, "file2");

  opt = GetOpt(argc, argv, "a:b:", &state);
  ck_assert_int_eq(opt, -1);
}
END_TEST

START_TEST(test_getopt_option_ordering) {
  char *argv[] = {"program", "file1", "-a", "file2", NULL};
  int argc = 4;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:", &state);
  ck_assert_int_eq(opt, -1);
}
END_TEST

START_TEST(test_getopt_state_preservation) {
  char *argv[] = {"program", "-a", "file1", "-x", "file2", NULL};
  int argc = 5;
  GetOptState state = InitGetOptState();

  int opt = GetOpt(argc, argv, "a:b:", &state);
  ck_assert_int_eq(opt, 'a');
  ck_assert_int_eq(state.optopt, 'a');
  int saved_optind = state.optind;
  int saved_optpos = state.optpos;
  int saved_optopt = state.optopt;

  opt = GetOpt(argc, argv, "a:b:", &state);
  ck_assert_int_eq(opt, '?');
  ck_assert_int_eq(state.optopt, 'x');

  ck_assert_int_eq(state.optind, saved_optind);
  ck_assert_int_eq(state.optpos, saved_optpos);
  ck_assert(state.optopt != saved_optopt);
}
END_TEST

Suite *UtilsSuite(void) {
  Suite *s = suite_create("Utils");
  TCase *tc_error = tcase_create("SetError");
  TCase *tc_getopt_init = tcase_create("InitGetOptState");
  TCase *tc_getopt = tcase_create("GetOpt");

  tcase_add_test(tc_error, test_set_error_basic);
  tcase_add_test(tc_error, test_set_error_with_format);
  tcase_add_test(tc_error, test_set_error_with_multiple_args);
  tcase_add_test(tc_error, test_set_error_empty);
  tcase_add_test(tc_error, test_set_error_truncation);

  tcase_add_test(tc_getopt_init, test_init_getopt_state);

  tcase_add_test(tc_getopt, test_getopt_simple_option);
  tcase_add_test(tc_getopt, test_getopt_option_without_arg);
  tcase_add_test(tc_getopt, test_getopt_unknown_option);
  tcase_add_test(tc_getopt, test_getopt_missing_argument);
  tcase_add_test(tc_getopt, test_getopt_combined_options);
  tcase_add_test(tc_getopt, test_getopt_argument_attached);
  tcase_add_test(tc_getopt, test_getopt_multiple_calls);
  tcase_add_test(tc_getopt, test_getopt_state_preservation);
  tcase_add_test(tc_getopt, test_getopt_no_more_options);
  tcase_add_test(tc_getopt, test_getopt_only_dash);
  tcase_add_test(tc_getopt, test_getopt_double_dash);
  tcase_add_test(tc_getopt, test_getopt_option_ordering);

  suite_add_tcase(s, tc_error);
  suite_add_tcase(s, tc_getopt_init);
  suite_add_tcase(s, tc_getopt);

  return s;
}
