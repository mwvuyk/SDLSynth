#include "cadev.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>


AudioDevice::AudioDevice(void (*callback_function)(void*, Uint8*, int), void* userdata, int freq, const char *device, 
                 int iscapture, int allowed_changes, int format, int channels, int samples)
                 {
        
        desired.freq = freq; // number of samples per second
        desired.format = format; // sample type (here: signed short i.e. 16 bit)
        desired.channels = channels; // only one channel
        desired.samples = samples; // buffer-size
        desired.callback = callback_function; // function SDL calls periodically to refill the buffer
        desired.userdata = userdata; // counter, keeping track of current sample number

        nDev = SDL_OpenAudioDevice(device, iscapture, &desired, &acquired, allowed_changes);
        if(nDev == 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
        if(desired.format != acquired.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");
        SDL_Log("Audio Device opened with ID %i", nDev);
                 }


AudioDevice::~AudioDevice()
{
        SDL_PauseAudioDevice(nDev, 1); // stop playing sound
        SDL_CloseAudioDevice(nDev);
        SDL_Log("Audio Device closed with ID %i", nDev);
}

void AudioDevice::Pause(int state)
{
    SDL_PauseAudioDevice(nDev, state);
}

