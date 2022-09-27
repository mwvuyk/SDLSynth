#ifndef CADEVH
#define CADEVH

#include <SDL2/SDL_audio.h>


class AudioDevice{

SDL_AudioDeviceID nDev;

public:
    AudioDevice(const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes);
    ~AudioDevice();

    void Pause(int state);
};

#endif