#include <math.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "cadev.h"

const int AMPLITUDE = 1000;
const int SAMPLE_RATE = 44100;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int*)user_data);

    for(int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * (440.0f) * time)); // render 441 HZ sine wave
    }
    
}

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_AUDIO) != 0) SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    
    uint32_t sample_nr = 0;
    AudioDevice audio = AudioDevice(audio_callback, &sample_nr, SAMPLE_RATE, NULL, 0, 0, AUDIO_S16SYS, 1, 2048);


    audio.Pause(0);
    SDL_Delay(1000); // wait while sound is playing


    return 0;
}