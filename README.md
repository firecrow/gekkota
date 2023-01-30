*Gekkota*
Gekkota is a sythesizer and audio composition program that uses algorithms to generate sound data on the gpu.

The core processing enging stores sounds in a segment structure to indicate points of transition such as frequency, volume, and eventally shape. These segments are used to calculate the audio frames using linear algebra.

The segments pack into a memory structure which is sent across to the gpu for parallel processing for a given period (usually 1 1/100th of a second or 1920 frames for 192khz).

**Code Organization**
* audio-segment - This the memomory structure and calculation code for the segments
* engine - This code calculates and processes the segments into audio data, on the host or gpu
* ui - This is the directory with the main executable
* title - This is a global object which contains information about the currently playing title and it's sound/song contents
* midi - This is the code which interfaces with midi controllers
* audio-output - This is the code which interfaces with the external audio output (ALSA)
* plot-visiaul - This is the mapping of audio data to a visual display of it
* instrument - This is where the midi events are handled and translated into internal sound events
* frontend-service - This is the code that intefaces with the front end windowing framework (wayland)