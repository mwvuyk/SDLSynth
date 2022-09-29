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
constexpr double ONEOVER = 1.0 / (double)SAMPLE_RATE;
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
    double val;
    EnvState state;
};

std::vector<Note> notes;
Envelope adsr = Envelope(ONEOVER);

double ToTime(uint32_t samplenumber)
{
    return samplenumber*ONEOVER;
}

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{

    Sint16 *buffer = (Sint16 *)raw_buffer;
    int length = (bytes / 2); // 2 bytes per sample for AUDIO_S16SYS
    uint32_t &samplenumber(*(uint32_t *)user_data);

    memset(buffer, 0, bytes);
    for (int i = 0; i < length; i++)
    {
        double dTime = ToTime(samplenumber);
        for (Note& n : notes)
        {
            buffer[i] += (Sint16)(adsr.env(n.val, n.state) * sin(2.0 * M_PI * (261.626 * pow(TWELFTHROOTOFTWO, n.key - 1)) * (dTime - n.timeOn))); 
            }
        samplenumber++;
    }
    
}

int GetKey(const int sdlkey)
{
    const auto key = KEY2KEY.find(sdlkey);
    return (key != KEY2KEY.end() ? key->second : 0);
}

int KBInput(std::vector<Note> &notes, double dTime)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN)
        {
            return -1; // Exit condition
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            int key = GetKey(event.key.keysym.sym);

            if (event.type == SDL_KEYDOWN && key)
            {
                auto find = std::find_if(notes.begin(), notes.end(), [key](Note &n)
                                         { return n.key == key && n.state != EnvState::Release; });
                if (find == notes.end())
                {
                    Note n;
                    n.key = key;
                    n.val = 0.0;
                    n.state = EnvState::Attack;
                    n.timeOn = dTime;
                    notes.push_back(n);
                    SDL_Log("NoteOn: %i, at %f", key, dTime);
                }
            }

            if (event.type == SDL_KEYUP && key)
            {
                auto find = std::find_if(notes.begin(), notes.end(), [key](Note &n)
                                         { return n.key == key && n.state != EnvState::Release; });
                if (find != notes.end())
                {
                    find->state = EnvState::Release;
                    find->timeOff = dTime;
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

    uint32_t samplenumber = 0;


    AudioDevice audio = AudioDevice(audio_callback, &samplenumber, SAMPLE_RATE, NULL, 0, 0, AUDIO_S16SYS, 1, pow(2,8));

    audio.Pause(0);
    
    while (true)
    {
        auto find = std::find_if(notes.begin(), notes.end(), [](Note &n)
                                 { return n.state == EnvState::Rest; });
        if(find != notes.end())
        {
        notes.erase(find);
        }

        if (KBInput(notes, ToTime(samplenumber)) == -1) // Keyboard input. Returns -1 if Escape key is pressed.
        {
            break;
        };
    }

    return 0;
}