#include "./logging.hpp"
#include <stdlib.h>
#include <stdio.h>

namespace ks {

  void err_exit(const char* msg) {
    fprintf(stderr, "error: %s\n", msg);
    exit(0);
  }

  void info(const char* msg) {
    fprintf(stdout, "info: %s\n", msg);
  }

  void error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
  }

}
