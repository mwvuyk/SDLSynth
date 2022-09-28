#ifndef CADEVH
#define CADEVH

#include <SDL2/SDL_audio.h>



class AudioDevice{

SDL_AudioDeviceID nDev;
SDL_AudioSpec desired;
SDL_AudioSpec acquired;

public:
    AudioDevice(void (*callback_function)(void*, Uint8*, int), void* userdata, int freq = 44100, const char *device = NULL, 
                int iscapture = 0, int allowed_changes = 0, int format = AUDIO_S16SYS, int channels = 1, int samples = 2048);
    ~AudioDevice();

    void Pause(int state);
};





#endif