#define GKA_GLOBAL_MEMORY_ERROR 13

#define memory_error(code, msg)                                                \
  do {                                                                         \
    fprintf(stderr, "Memory error %d %s:%d", code, __FILE__, __LINE__);        \
  } while (0);