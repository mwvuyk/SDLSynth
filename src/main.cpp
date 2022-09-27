#include <math.h>
#include <iostream>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "cadev.h"

const int AMPLITUDE = 1000;
const int SAMPLE_RATE = 44100;
const double TWELFTHROOTOFTWO = 1.0594630943592952646;

const std::unordered_map<int, int> KEY2KEY = {
    {SDLK_z , 1}, {SDLK_s, 2}, {SDLK_x, 3}, {SDLK_d, 4},
    {SDLK_c, 5}, {SDLK_v, 6}, {SDLK_g, 7}, {SDLK_b, 8}, {SDLK_h, 9},
    {SDLK_n, 10}, {SDLK_j, 11}, {SDLK_m, 12}, {SDLK_COMMA, 13},
    {SDLK_l, 14}, {SDLK_PERIOD, 15}, {SDLK_SEMICOLON, 16}, {SDLK_SLASH, 17},
    {SDLK_q , 13}, {SDLK_2, 14}, {SDLK_w, 15}, {SDLK_3, 16}, {SDLK_e, 17},
    {SDLK_r, 18}, {SDLK_5, 19}, {SDLK_t, 20}, {SDLK_6, 21},
    {SDLK_y, 22}, {SDLK_7, 23}, {SDLK_u, 24}, {SDLK_i, 25},
    {SDLK_9, 26}, {SDLK_o, 27}, {SDLK_0, 28}, {SDLK_p, 29}
};

//const int keys[] {SDLK_z , SDLK_s,  SDLK_x,  SDLK_d, SDLK_c, SDLK_v, SDLK_g, SDLK_b, SDLK_h, SDLK_n, SDLK_j, SDLK_m, SDLK_COMMA, SDLK_l, SDLK_PERIOD, SDLK_SEMICOLON, 
//SDLK_SLASH, SDLK_q, SDLK_2, SDLK_w, SDLK_3, SDLK_e, SDLK_r, SDLK_5, SDLK_t, SDLK_6, SDLK_y, SDLK_7, SDLK_u, SDLK_i, SDLK_9, SDLK_o, SDLK_0, SDLK_p};

int note = 0;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int*)user_data);
    if(note)
    {
    for(int i = 0; i < length; i++, sample_nr++)
        {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * (261.626f * pow(TWELFTHROOTOFTWO,note-1)) * time)); // render 441 HZ sine wave
        }
    }
    else

    {
        for(int i = 0; i < length; i++, sample_nr++)
        {
        buffer[i] = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,680, 480,0);
 //   SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    SDL_UpdateWindowSurface(window);

    uint32_t sample_nr = 0;
    AudioDevice audio = AudioDevice(audio_callback, &sample_nr, SAMPLE_RATE, NULL, 0, 0, AUDIO_S16SYS, 1, 2048);


    audio.Pause(0);
    

    SDL_Event event;
    while(true)
    {
    
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_ESCAPE) return 0; //Exit program
            auto key = KEY2KEY.find(event.key.keysym.sym);
            if(key != KEY2KEY.end() ){
            SDL_Log("Key press: %i", key->second);
            note = key->second;}
        }
        
        if(event.type == SDL_KEYUP){
            auto key = KEY2KEY.find(event.key.keysym.sym);
            if(key != KEY2KEY.end() ){
            SDL_Log("Key up: %i", key->second);
            note = 0;}
        }

    }
    }

    return 0;
}