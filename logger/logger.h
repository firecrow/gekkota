/**
 * Some of the use cases, such as frames of a sound playing live, are longer
 * than can be easlily sent as terminal output when debugging, this is an
 * alternative to visualizing data, where moderate size data sets can be sent to
 * a file for examination later.
 */
struct Logger {
  FILE *file;
  bool opened = false;
  Logger() {
    if (!this->opened) {
      this->file = fopen("debug.log", "w");
      fprintf(
          this->file,
          "start of new debug log -----------------------------------\n"
      );
      printf("opened logger file %s\n", "debug.log");
      this->opened = true;
    }
  }
  ~Logger() {
    fclose(this->file);
    this->opened = false;
  }
};