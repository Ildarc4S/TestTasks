#include "converter_core.h"
#include "tests.h"

START_TEST(test_hex_char_to_value_digits) {
  ck_assert_int_eq(HexCharToValue('0'), 0);
  ck_assert_int_eq(HexCharToValue('1'), 1);
  ck_assert_int_eq(HexCharToValue('5'), 5);
  ck_assert_int_eq(HexCharToValue('9'), 9);
}
END_TEST

START_TEST(test_hex_char_to_value_lowercase) {
  ck_assert_int_eq(HexCharToValue('a'), 10);
  ck_assert_int_eq(HexCharToValue('b'), 11);
  ck_assert_int_eq(HexCharToValue('f'), 15);
}
END_TEST

START_TEST(test_hex_char_to_value_uppercase) {
  ck_assert_int_eq(HexCharToValue('A'), 10);
  ck_assert_int_eq(HexCharToValue('B'), 11);
  ck_assert_int_eq(HexCharToValue('F'), 15);
}
END_TEST

START_TEST(test_hex_char_to_value_invalid) {
  ck_assert_int_eq(HexCharToValue('g'), -1);
  ck_assert_int_eq(HexCharToValue('z'), -1);
  ck_assert_int_eq(HexCharToValue(' '), -1);
  ck_assert_int_eq(HexCharToValue('@'), -1);
  ck_assert_int_eq(HexCharToValue('G'), -1);
  ck_assert_int_eq(HexCharToValue('-'), -1);
}
END_TEST

START_TEST(test_value_to_hex_char_digits) {
  ck_assert_int_eq(ValueToHexChar(0), '0');
  ck_assert_int_eq(ValueToHexChar(5), '5');
  ck_assert_int_eq(ValueToHexChar(9), '9');
}
END_TEST

START_TEST(test_value_to_hex_char_letters) {
  ck_assert_int_eq(ValueToHexChar(10), 'A');
  ck_assert_int_eq(ValueToHexChar(11), 'B');
  ck_assert_int_eq(ValueToHexChar(15), 'F');
}
END_TEST

START_TEST(test_hex_to_byte_valid) {
  uint8_t result;

  ck_assert(HexToByte((HexPair){'0', '0'}, &result));
  ck_assert_int_eq(result, 0x00);

  ck_assert(HexToByte((HexPair){'F', 'F'}, &result));
  ck_assert_int_eq(result, 0xFF);

  ck_assert(HexToByte((HexPair){'A', '5'}, &result));
  ck_assert_int_eq(result, 0xA5);

  ck_assert(HexToByte((HexPair){'1', '2'}, &result));
  ck_assert_int_eq(result, 0x12);

  ck_assert(HexToByte((HexPair){'a', 'f'}, &result));
  ck_assert_int_eq(result, 0xAF);
}
END_TEST

START_TEST(test_hex_to_byte_invalid) {
  uint8_t result;

  ck_assert(!HexToByte((HexPair){'X', '5'}, &result));
  ck_assert(!HexToByte((HexPair){'1', 'Z'}, &result));
  ck_assert(!HexToByte((HexPair){' ', '5'}, &result));
  ck_assert(!HexToByte((HexPair){'1', '@'}, &result));
}
END_TEST

START_TEST(test_byte_to_hex) {
  HexPair pair;

  pair = ByteToHex(0x00);
  ck_assert_int_eq(pair.high, '0');
  ck_assert_int_eq(pair.low, '0');

  pair = ByteToHex(0xFF);
  ck_assert_int_eq(pair.high, 'F');
  ck_assert_int_eq(pair.low, 'F');

  pair = ByteToHex(0xA5);
  ck_assert_int_eq(pair.high, 'A');
  ck_assert_int_eq(pair.low, '5');

  pair = ByteToHex(0x1F);
  ck_assert_int_eq(pair.high, '1');
  ck_assert_int_eq(pair.low, 'F');

  pair = ByteToHex(0xB4);
  ck_assert_int_eq(pair.high, 'B');
  ck_assert_int_eq(pair.low, '4');
}
END_TEST

START_TEST(test_process_hex_pair_valid) {
  FILE* output_file = tmpfile();
  ck_assert_ptr_nonnull(output_file);

  bool result = ProcessHexPair((HexPair){'A', '5'}, 1, 1, output_file);
  ck_assert(result);

  rewind(output_file);
  uint8_t written_byte;
  size_t read_count = fread(&written_byte, 1, 1, output_file);
  ck_assert_int_eq(read_count, 1);
  ck_assert_int_eq(written_byte, 0xA5);

  fclose(output_file);
}
END_TEST

START_TEST(test_process_hex_pair_invalid) {
  FILE* output_file = tmpfile();
  ck_assert_ptr_nonnull(output_file);

  bool result = ProcessHexPair((HexPair){'X', '5'}, 1, 1, output_file);
  ck_assert(!result);

  fclose(output_file);
}
END_TEST

START_TEST(test_process_hex_pair_write_error) {
  FILE* output_file = fopen("/dev/null", "r");
  if (output_file) {
    bool result = ProcessHexPair((HexPair){'A', '5'}, 1, 1, output_file);
    ck_assert(!result);
    fclose(output_file);
  }
}
END_TEST

START_TEST(test_process_files_success) {
  FILE* test_input = fopen("test_input.txt", "w");
  fputs("A1 B2 C3", test_input);
  fclose(test_input);

  bool result = ProcessFiles("test_input.txt", "test_output.txt", "r", "w",
                             ProcessHexToBin);
  ck_assert(result);

  FILE* output = fopen("test_output.txt", "r");
  ck_assert_ptr_nonnull(output);
  fclose(output);

  remove("test_input.txt");
  remove("test_output.txt");
}
END_TEST

START_TEST(test_process_files_input_not_found) {
  bool result =
      ProcessFiles("nonexistent.txt", "output.txt", "r", "w", ProcessHexToBin);
  ck_assert(!result);
}
END_TEST

START_TEST(test_process_files_empty_input) {
  FILE* empty = fopen("empty.txt", "w");
  fclose(empty);

  bool result =
      ProcessFiles("empty.txt", "output.txt", "r", "w", ProcessHexToBin);
  ck_assert(!result);

  remove("empty.txt");
}
END_TEST

START_TEST(test_process_hex_to_bin_valid) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  fputs("A1 B2 C3\nD4 E5 F6", input);
  rewind(input);

  bool result = ProcessHexToBin(input, output);
  ck_assert(result);

  rewind(output);
  uint8_t buffer[6];
  size_t read = fread(buffer, 1, 6, output);
  ck_assert_int_eq(read, 6);
  ck_assert_int_eq(buffer[0], 0xA1);
  ck_assert_int_eq(buffer[1], 0xB2);
  ck_assert_int_eq(buffer[2], 0xC3);
  ck_assert_int_eq(buffer[3], 0xD4);
  ck_assert_int_eq(buffer[4], 0xE5);
  ck_assert_int_eq(buffer[5], 0xF6);

  fclose(input);
  fclose(output);
}
END_TEST

START_TEST(test_process_hex_to_bin_invalid_hex) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  fputs("A1 X2 C3", input);
  rewind(input);

  bool result = ProcessHexToBin(input, output);
  ck_assert(!result);

  fclose(input);
  fclose(output);
}
END_TEST

START_TEST(test_process_bin_to_hex_valid) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  uint8_t test_data[] = {0xA1, 0xB2, 0xC3, 0xD4};
  fwrite(test_data, 1, 4, input);
  rewind(input);

  bool result = ProcessBinToHex(input, output);
  ck_assert(result);

  rewind(output);
  char hex_buffer[9] = {0};
  size_t read_count = fread(hex_buffer, 1, 8, output);
  hex_buffer[8] = '\0';

  ck_assert_int_eq(read_count, 8);
  ck_assert_str_eq(hex_buffer, "A1B2C3D4");

  fclose(input);
  fclose(output);
}
END_TEST

START_TEST(test_hex_byte_conversion_roundtrip) {
  uint8_t original_byte = 0xA5;
  HexPair hex_pair = ByteToHex(original_byte);

  uint8_t converted_byte;
  bool success = HexToByte(hex_pair, &converted_byte);

  ck_assert(success);
  ck_assert_int_eq(converted_byte, original_byte);
}
END_TEST

START_TEST(test_byte_hex_conversion_roundtrip) {
  HexPair original_pair = {'B', '4'};
  uint8_t byte;

  if (HexToByte(original_pair, &byte)) {
    HexPair converted_pair = ByteToHex(byte);
    ck_assert_int_eq(converted_pair.high, original_pair.high);
    ck_assert_int_eq(converted_pair.low, original_pair.low);
  }
}
END_TEST

START_TEST(test_boundary_values) {
  uint8_t result;

  ck_assert(HexToByte((HexPair){'0', '0'}, &result));
  ck_assert_int_eq(result, 0x00);

  ck_assert(HexToByte((HexPair){'F', 'F'}, &result));
  ck_assert_int_eq(result, 0xFF);

  ck_assert(HexToByte((HexPair){'8', '0'}, &result));
  ck_assert_int_eq(result, 0x80);
}
END_TEST

START_TEST(test_process_files_output_open_error) {
  FILE* input = fopen("test_input.txt", "w");
  fputs("A1 B2", input);
  fclose(input);

  bool result = ProcessFiles("test_input.txt", "/invalid/path/output.txt", "r",
                             "w", ProcessHexToBin);
  ck_assert(!result);

  remove("test_input.txt");
}
END_TEST

START_TEST(test_process_hex_to_bin_unpaired_hex) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  fputs("A1 B2 C", input);
  rewind(input);

  bool result = ProcessHexToBin(input, output);
  ck_assert(!result);

  fclose(input);
  fclose(output);
}
END_TEST

START_TEST(test_process_bin_to_hex_write_error) {
  FILE* input = tmpfile();
  FILE* output = fopen("/dev/null", "r");

  ck_assert_ptr_nonnull(input);
  if (output) {
    uint8_t test_data[] = {0xA1, 0xB2};
    fwrite(test_data, 1, 2, input);
    rewind(input);

    bool result = ProcessBinToHex(input, output);
    ck_assert(!result);

    fclose(input);
    fclose(output);
  } else {
    fclose(input);
  }
}
END_TEST

START_TEST(test_process_hex_pair_edge_positions) {
  FILE* output_file = tmpfile();
  ck_assert_ptr_nonnull(output_file);

  bool result = ProcessHexPair((HexPair){'F', 'F'}, 100, 50, output_file);
  ck_assert(result);

  rewind(output_file);
  uint8_t written_byte;
  size_t read_count = fread(&written_byte, 1, 1, output_file);
  ck_assert_int_eq(read_count, 1);
  ck_assert_int_eq(written_byte, 0xFF);

  fclose(output_file);
}
END_TEST

START_TEST(test_process_bin_to_hex_large_file) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  uint8_t large_data[600];
  for (int i = 0; i < 600; i++) {
    large_data[i] = i % 256;
  }
  fwrite(large_data, 1, 600, input);
  rewind(input);

  bool result = ProcessBinToHex(input, output);
  ck_assert(result);

  rewind(output);
  char hex_buffer[1201];
  size_t read_count = fread(hex_buffer, 1, 1200, output);
  hex_buffer[read_count] = '\0';

  ck_assert_int_eq(read_count, 1200);

  fclose(input);
  fclose(output);
}
END_TEST

START_TEST(test_process_hex_to_bin_empty_lines_and_spaces) {
  FILE* input = tmpfile();
  FILE* output = tmpfile();
  ck_assert_ptr_nonnull(input);
  ck_assert_ptr_nonnull(output);

  fputs("   A1   B2  \n\n   C3   D4  \n", input);
  rewind(input);

  bool result = ProcessHexToBin(input, output);
  ck_assert(result);

  rewind(output);
  uint8_t buffer[4];
  size_t read = fread(buffer, 1, 4, output);
  ck_assert_int_eq(read, 4);
  ck_assert_int_eq(buffer[0], 0xA1);
  ck_assert_int_eq(buffer[1], 0xB2);
  ck_assert_int_eq(buffer[2], 0xC3);
  ck_assert_int_eq(buffer[3], 0xD4);

  fclose(input);
  fclose(output);
}
END_TEST

Suite* ConversionSuite(void) {
  Suite* s = suite_create("Conversion");
  TCase* tc_core = tcase_create("Core");
  TCase* tc_file_processing = tcase_create("FileProcessing");
  ;

  tcase_add_test(tc_core, test_hex_char_to_value_digits);
  tcase_add_test(tc_core, test_hex_char_to_value_lowercase);
  tcase_add_test(tc_core, test_hex_char_to_value_uppercase);
  tcase_add_test(tc_core, test_hex_char_to_value_invalid);
  tcase_add_test(tc_core, test_value_to_hex_char_digits);
  tcase_add_test(tc_core, test_value_to_hex_char_letters);
  tcase_add_test(tc_core, test_hex_to_byte_valid);
  tcase_add_test(tc_core, test_hex_to_byte_invalid);
  tcase_add_test(tc_core, test_byte_to_hex);
  tcase_add_test(tc_core, test_process_hex_pair_valid);
  tcase_add_test(tc_core, test_process_hex_pair_invalid);
  tcase_add_test(tc_core, test_process_hex_pair_write_error);
  tcase_add_test(tc_core, test_hex_byte_conversion_roundtrip);
  tcase_add_test(tc_core, test_byte_hex_conversion_roundtrip);
  tcase_add_test(tc_core, test_boundary_values);
  tcase_add_test(tc_core, test_process_files_output_open_error);
  tcase_add_test(tc_core, test_process_hex_to_bin_unpaired_hex);
  tcase_add_test(tc_core, test_process_bin_to_hex_write_error);
  tcase_add_test(tc_core, test_process_hex_pair_edge_positions);
  tcase_add_test(tc_file_processing, test_process_bin_to_hex_large_file);
  tcase_add_test(tc_file_processing,
                 test_process_hex_to_bin_empty_lines_and_spaces);

  tcase_add_test(tc_file_processing, test_process_files_success);
  tcase_add_test(tc_file_processing, test_process_files_input_not_found);
  tcase_add_test(tc_file_processing, test_process_files_empty_input);
  tcase_add_test(tc_file_processing, test_process_hex_to_bin_valid);
  tcase_add_test(tc_file_processing, test_process_hex_to_bin_invalid_hex);
  tcase_add_test(tc_file_processing, test_process_bin_to_hex_valid);

  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_file_processing);

  return s;
}
