int midi_accept();

struct gka_midi_event {
  char action;
  char key;
  char volume;
};

class MidiService {
public:
  MidiService();
  static void setup();
  static void loop();
  static void teardown();
  static bool running;
};
