#include "cadev.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

AudioDevice::AudioDevice(const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes){
        nDev = SDL_OpenAudioDevice(device, iscapture, desired, obtained, allowed_changes);
        if(nDev == 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
        if(desired->format != obtained->format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");
        SDL_Log("Audio Device opened with ID %i", nDev);
        
}

AudioDevice::~AudioDevice(){
        SDL_PauseAudioDevice(nDev, 1); // stop playing sound
        SDL_CloseAudioDevice(nDev);
        SDL_Log("Audio Device closed with ID %i", nDev);
}

void AudioDevice::Pause(int state){
    SDL_PauseAudioDevice(nDev, state);
}

