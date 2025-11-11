#include "converter.h"
#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void SetError(const char *format, ...) {
  ConverterConfig *config = GetConfig();

  va_list args;
  va_start(args, format);
  vsnprintf(config->error_message, ERROR_MESSAGE_LEN, format, args);
  va_end(args);
}

GetOptState InitGetOptState() {
  return (GetOptState){NULL, GETOPT_INITIAL_OPTIND, GETOPT_INITIAL_OPTPOS, GETOPT_INITIAL_OPTOPT};
}

int GetOpt(int argc, char **argv, const char* optstring, GetOptState *state) {
  int func_result = 0;
  state->optarg = NULL;
  state->optopt = 0;

  if (state->optind >= argc) {
    func_result = -1;
    return -1;
  }

  char *current_arg = argv[state->optind];

  if (current_arg[0] != '-' || current_arg[1] == '\0') {
    return -1;
  }

  char optchar = current_arg[state->optpos];
  state->optopt = optchar;

  const char *pos = strchr(optstring, optchar);
  if (pos == NULL) {
    return '?';
  }

  if (pos[1] == ':') {
    if (current_arg[state->optpos + 1] != '\0') {
      state->optarg= &current_arg[state->optpos + 1];
      state->optind++;
      state->optpos = 1;
    } else {
      if (state->optind + 1 >= argc) {
        return '?';
      }
      state->optarg = argv[state->optind + 1];
      state->optind += 2;
      state->optpos = 1;
    }
  } else {
    if (current_arg[state->optpos + 1] != '\0') {
      state->optpos++;
    } else {
      state->optind++;
      state->optpos = 1; 
    }
  }
  return optchar;
}
