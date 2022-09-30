#ifndef CADEVH
#define CADEVH

#include <SDL2/SDL_audio.h>
#include <fstream>

class AudioDevice
{

    SDL_AudioDeviceID nDev;
    SDL_AudioSpec desired;
    SDL_AudioSpec acquired;

public:
    AudioDevice(void (*callback_function)(void *, Uint8 *, int), void *userdata, int freq = 44100, const char *device = NULL,
                int iscapture = 0, int allowed_changes = 0, int format = AUDIO_S16SYS, int channels = 1, int samples = 2048);
    ~AudioDevice();

    void Pause(int state);
};

enum class EnvState
{
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
    Envelope(double sampletime, double a = 0.1, double d = 0.1, double s = 0.8, double r = 0.2);

    double env(double &val, EnvState &state);
};

class OutFile
{

    int preAudio;
    int postAudio;
    std::ofstream audioFile;

    void writeToFile(int value, int size);

public:
    OutFile(const char *filename, int sampleRate, int bitDepth);

    ~OutFile();

    void writeBuf(void* data, int bytes);
};

#endif