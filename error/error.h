#define GKA_GLOBAL_MEMORY_ERROR 13
#define GKA_GLOBAL_INIT_ERROR 15

/**
 * These macros are mostly so that errors can have line numbers on them, because
 * the macros are evaluated at compile time they place the __CONSTANT_NAME__
 * values in a way that is local to the error which is useful
 */
#define memory_error(code, msg)                                                \
  do {                                                                         \
    fprintf(stderr, "Memory error %d %s:%d", code, __FILE__, __LINE__);        \
  } while (0);

#define initilization_error(code, msg)                                         \
  do {                                                                         \
    fprintf(stderr, "Initilization error %d %s:%d", code, __FILE__, __LINE__); \
  } while (0);