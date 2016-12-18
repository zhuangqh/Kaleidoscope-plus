#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace ks {

  void error(const char* info) {
    fprintf(stderr, "error: %s\n", info);
    exit(0);
  }

}
