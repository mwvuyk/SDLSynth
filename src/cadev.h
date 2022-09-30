#ifndef CADEVH
#define CADEVH

#include <SDL2/SDL_audio.h>



class AudioDevice
{

SDL_AudioDeviceID nDev;
SDL_AudioSpec desired;
SDL_AudioSpec acquired;

public:
    AudioDevice(void (*callback_function)(void*, Uint8*, int), void* userdata, int freq = 44100, const char *device = NULL, 
                int iscapture = 0, int allowed_changes = 0, int format = AUDIO_S16SYS, int channels = 1, int samples = 2048);
    ~AudioDevice();

    void Pause(int state);
};


enum class EnvState{
    Attack,
    Decay,
    Sustain,
    Release,
    Rest
};

class Envelope
{
double dAttack;
double dDecay;
double dSustain;
double dRelease;
double dSampletime;
double dA;
double dD;
double dR;

const double dRoot = 20000.0;
const double dMaxtime = 10.0;
const double dLimit = 1e-4;

public:
Envelope(double sampletime, double a = 0.1, double d = 0.1, double s = 1.0, double r = 0.2);

double env(double& val, EnvState& state);
};



#endif