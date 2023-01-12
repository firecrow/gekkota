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