# Gekkota
Gekkota is a sythesizer and audio composition program that uses algorithms to generate sound data on the gpu.

The core processing enging stores sounds in a segment structure to indicate points of transition such as frequency, volume, and eventally shape. These segments are used to calculate the audio frames using linear algebra.

The segments pack into a memory structure which is sent across to the gpu for parallel processing for a given period (usually 1 1/100th of a second or 1920 frames for 192khz).

# Project Purpose

Gekkota was the begining of algorithmic effects within an audio-processor driven by a memory segment based animation framework. 

# Status

Experimental Idation


## Code Organization
Comments and doc-string documentation is placed in the header .h or .hpp files. 
The flagship executable of the application starts in `ui/frontend.main.cpp` with audio processing mainly in `engine` and the memory/processing code in `audio-segment`.

* audio-segment - This the memomory structure and calculation code for the segments
* engine - This code calculates and processes the segments into audio data, on the host or gpu
* ui - This is the directory with the main executable
* title - This is a global object which contains information about the currently playing title and it's sound/song contents
* midi - This is the code which interfaces with midi controllers
* audio-output - This is the code which interfaces with the external audio output (ALSA)
* plot-visiaul - This is the mapping of audio data to a visual display of it
* instrument - This is where the midi events are handled and translated into internal sound events
* frontend-service - This is the code that intefaces with the front end windowing framework (wayland)
