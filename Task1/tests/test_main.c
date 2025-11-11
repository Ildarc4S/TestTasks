#include "tests.h"

int main() {
  int number_failed;
  SRunner *sr;

  sr = srunner_create(NULL);
  srunner_add_suite(sr, ParserSuite());
  srunner_add_suite(sr, ConversionSuite());
  srunner_add_suite(sr, ValidationSuite());
  srunner_add_suite(sr, UtilsSuite());
  srunner_add_suite(sr, IntegrationSuite());

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}
