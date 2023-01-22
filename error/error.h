#define GKA_GLOBAL_MEMORY_ERROR 13
#define GKA_GLOBAL_INIT_ERROR 15

#define memory_error(code, msg)                                                \
  do {                                                                         \
    fprintf(stderr, "Memory error %d %s:%d", code, __FILE__, __LINE__);        \
  } while (0);

#define initilization_error(code, msg)                                         \
  do {                                                                         \
    fprintf(stderr, "Initilization error %d %s:%d", code, __FILE__, __LINE__); \
  } while (0);