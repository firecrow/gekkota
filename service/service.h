struct Service {
  virtual void setup() = 0;
  virtual void_fptr getTask() = 0;
  virtual void teardown() = 0;
  static bool running;
};