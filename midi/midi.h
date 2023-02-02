/**
 * This function accepts MIDI event packets from the hardware, using the 3 bytes
 * of information to trigger behaviour in the app through the
 * dispatch_to_instrument function
 */
int midi_accept();

/**
 * Event struct used to convey values
 */
struct gka_midi_event {
  char action;
  char key;
  char volume;
};

/**
 * This is the class that listens and is the go between for the hardware
 * instruments and the router to generate sound data entris
 */
class MidiService {
public:
  MidiService();
  static void setup();
  static void loop();
  static void teardown();
  static bool running;
};
