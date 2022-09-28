#include <math.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "cadev.h"

const int AMPLITUDE = 1000;
const int SAMPLE_RATE = 44100;
constexpr double ONEOVER = 1/(double)SAMPLE_RATE;
const double TWELFTHROOTOFTWO = 1.0594630943592952646;

const std::unordered_map<int, int> KEY2KEY = {{SDLK_z, 1}, {SDLK_s, 2}, {SDLK_x, 3}, {SDLK_d, 4}, {SDLK_c, 5}, {SDLK_v, 6}, {SDLK_g, 7}, {SDLK_b, 8}, {SDLK_h, 9}, {SDLK_n, 10}, {SDLK_j, 11}, {SDLK_m, 12}, {SDLK_COMMA, 13}, {SDLK_l, 14}, {SDLK_PERIOD, 15}, {SDLK_SEMICOLON, 16}, {SDLK_SLASH, 17}, {SDLK_q, 13}, {SDLK_2, 14}, {SDLK_w, 15}, {SDLK_3, 16}, {SDLK_e, 17}, {SDLK_r, 18}, {SDLK_5, 19}, {SDLK_t, 20}, {SDLK_6, 21}, {SDLK_y, 22}, {SDLK_7, 23}, {SDLK_u, 24}, {SDLK_i, 25}, {SDLK_9, 26}, {SDLK_o, 27}, {SDLK_0, 28}, {SDLK_p, 29}};

// const int keys[] {SDLK_z , SDLK_s,  SDLK_x,  SDLK_d, SDLK_c, SDLK_v, SDLK_g, SDLK_b, SDLK_h, SDLK_n, SDLK_j, SDLK_m, SDLK_COMMA, SDLK_l, SDLK_PERIOD, SDLK_SEMICOLON,
// SDLK_SLASH, SDLK_q, SDLK_2, SDLK_w, SDLK_3, SDLK_e, SDLK_r, SDLK_5, SDLK_t, SDLK_6, SDLK_y, SDLK_7, SDLK_u, SDLK_i, SDLK_9, SDLK_o, SDLK_0, SDLK_p};




struct Note
{
    int key;
    double timeOn;
    double timeOff;
    double vel;
    bool active;
};

std::vector<Note> notes;


void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16 *)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    double &time(*(double *)user_data);

    for (int i = 0; i < length; i++)
    {
        buffer[i] = 0;
    }

    for (int i = 0; i < length; i++, time = time + ONEOVER )
    {
        for (Note n : notes)
        {
            buffer[i] += (Sint16)(AMPLITUDE * sin(2.0f * M_PI * (261.626f * pow(TWELFTHROOTOFTWO, n.key - 1)) * (time - n.timeOn))); // render 441 HZ sine wave
        }
    }
}

int GetKey(int sdlkey)
{
    auto key = KEY2KEY.find(sdlkey);
    return (key != KEY2KEY.end() ? key->second : 0);
}

int KBInput(std::vector<Note>& notes)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN)
        {
            return -1; // Exit condition
        }

        int key = 0;
        if (event.type == SDL_KEYDOWN || SDL_KEYUP)
        {
            key = GetKey(event.key.keysym.sym);

            if (event.type == SDL_KEYDOWN && key)
            {
                auto find = std::find_if(notes.begin(), notes.end(), [key](Note &n)
                                         { return n.key == key; });
                if (find == notes.end())
                {
                    Note n;
                    n.key = key;
                    n.active = true;
                    notes.push_back(n);
                }
            }

            if (event.type == SDL_KEYUP && key)
            {
                auto find = std::find_if(notes.begin(), notes.end(), [key](Note &n)
                                         { return n.key == key; });
                if (find != notes.end())
                {
                    notes.erase(find);
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());

    SDL_Window *window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 680, 480, 0);
    //   SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    SDL_UpdateWindowSurface(window);

    double time = 0;
    AudioDevice audio = AudioDevice(audio_callback, &time, SAMPLE_RATE, NULL, 0, 0, AUDIO_S16SYS, 1, 2048);

    audio.Pause(0);

    while (true)
    {
        if (KBInput(notes) == -1) //Keyboard input. Returns -1 if Escape key is pressed.
        {
            break;
        };
    }

    return 0;
}