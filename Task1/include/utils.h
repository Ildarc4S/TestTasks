#ifndef UTILS_H
#define UTILS_H

typedef struct {
  char *optarg;
  int optind;
  int optpos;
  int optopt;
} GetOptState;

#define GETOPT_INITIAL_OPTIND 1
#define GETOPT_INITIAL_OPTPOS 1
#define GETOPT_INITIAL_OPTOPT 0

GetOptState InitGetOptState();
int GetOpt(int argc, char **argv, const char* optsring, GetOptState *state);

void SetError(const char *format, ...);

#endif // UTILS_H