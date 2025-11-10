#include "converter.h"

#include <stdarg.h>
#include <stdio.h>

void SetError(const char *format, ...) {
  ConverterConfig *config = GetConfig();

  va_list args;
  va_start(args, format);
  vsnprintf(config->error_message, ERROR_MESSAGE_LEN, format, args);
  va_end(args);
}
