using namespace std;

class AudioOutputService {
public:
  AudioOutputService();
  static AudioOutputService &instance;
  void Setup(struct gka_audio_params *gka_params);
  function<void(void)> getAction();
  void Teardown();
  static bool running;
  struct gka_output_objects hw_state;
  struct gka_audio_params gka_params;
};
