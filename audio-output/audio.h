using namespace std;

/**
 * This is the class whose objects manage audio output devices.
 *
 * Presently this code initializes the engine and GPU processing code directly -
 * though this is expected to be re-parented as the system grows
 *
 * The class is responsible for
 * - initializing the audio output hardware properties
 * - initialize the engine code
 * - teardown and cleanup of resources
 *
 * It holds hardware specific information such as rate and period_size, this is
 * becuase what is asked of the hardware is not ultimately what is accepted,
 * it's possible to ask for a bitrate of a hardware device that is beyond what
 * it supports, in that case the properties persisted here are a reflection of
 * the hardware not configuration
 */
class AudioOutputService {
public:
  AudioOutputService();
  /**
   * Global instance referenced from the engine for access to live hardware
   * values updates post setup
   */
  static AudioOutputService &instance;
  /**
   * The setup function  calls the hardware setup code, presently found with
   * the engine code files
   */
  void Setup(struct gka_audio_params *gka_params);
  /**
   * This is the entry point that the rest of the application uses to run this
   * class's behaviour in a thread
   */
  function<void(void)> getAction();
  /**
   * Clearnup connections to hardware and reserved memory for processing
   */
  void Teardown();
  /**
   * Flag used by the engine to determine when to abort processing
   */
  static bool running;
  /**
   * Data related to the present hardware device, include a handle
   */
  struct gka_output_objects hw_state;
  /**
   * Properties that the hardware is currently using such as bit rate and period
   * size
   */
  struct gka_audio_params gka_params;
};
