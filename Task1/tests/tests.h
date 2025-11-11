#ifndef TESTS_H
#define TESTS_H

#include <check.h>
#include <stdlib.h>

Suite *ParserSuite(void);
Suite *ConversionSuite(void);
Suite *ValidationSuite(void);
Suite *UtilsSuite(void);
Suite *IntegrationSuite(void);

#endif  // TESTS_H
