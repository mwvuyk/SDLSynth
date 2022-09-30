#include "cadev.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

// AudioDevice Class
AudioDevice::AudioDevice(void (*callback_function)(void *, Uint8 *, int), void *userdata, int freq, const char *device,
                         int iscapture, int allowed_changes, int format, int channels, int samples)
{

        desired.freq = freq;                  // number of samples per second
        desired.format = format;              // sample type (here: signed short i.e. 16 bit)
        desired.channels = channels;          // only one channel
        desired.samples = samples;            // buffer-size
        desired.callback = callback_function; // function SDL calls periodically to refill the buffer
        desired.userdata = userdata;          // counter, keeping track of current sample number

        nDev = SDL_OpenAudioDevice(device, iscapture, &desired, &acquired, allowed_changes);
        if (nDev == 0)
                SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
        SDL_Log("%i,%i,%i,%i", desired.freq, desired.format, desired.channels, desired.samples);
        SDL_Log("%i,%i,%i,%i", acquired.freq, acquired.format, acquired.channels, acquired.samples);
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

// Envelope Class

bool approximatelyEqual(float a, float b, float epsilon)
{
        return SDL_fabs(a - b) <= ((SDL_fabs(a) < SDL_fabs(b) ? SDL_fabs(b) : SDL_fabs(a)) * epsilon);
}

Envelope::Envelope(double sampletime, double a, double d, double s, double r)
{
        dA = a;
        dD = d;
        dR = r;

        dAttack = SDL_pow(dRoot, 1.0 - dA) / dMaxtime;
        dDecay = SDL_pow(dRoot, 1.0 - dD) / dMaxtime;
        dSustain = s;
        dRelease = SDL_pow(dRoot, 1.0 - dR) / dMaxtime;
        dSampletime = sampletime;
}

double Envelope::env(double &val, EnvState &state)
{

        switch (state)
        {
        case EnvState::Attack:
                if (dD < dLimit)
                {
                        val = 1.0;
                }

                else
                {
                        val += dAttack * (1.01 - val) * dSampletime;
                }

                if (val >= 1.0)
                {
                        val = 1.0;
                        state = EnvState::Decay;
                }

                break;

        case EnvState::Decay:

                if (dD < dLimit)
                {
                        val = dSustain;
                }

                else
                {
                        val += dDecay * (dSustain - val) * dSampletime;
                }

                if (approximatelyEqual(val, dSustain, dLimit))
                {
                        state = EnvState::Sustain;
                }

                break;

        case EnvState::Sustain:
                val = dSustain;
                break;

        case EnvState::Release:

                if (dR < dLimit)
                {
                        val = 0.0;
                }

                else
                {
                        val += dRelease * (0.0 - val) * dSampletime;
                }

                if (approximatelyEqual(val, 0.0, dLimit))
                {
                        state = EnvState::Rest;
                }
                break;
        default:
                val = 0.0;
                break;
        }

        return val;
}

// File writer Class

void OutFile::writeToFile(int value, int size)
{
        audioFile.write(reinterpret_cast<const char *>(&value), size);
};

OutFile::OutFile(const char *filename, int sampleRate, int bitDepth)
{

        audioFile.open(filename, std::ios::binary);
        audioFile << "RIFF0000WAVEfmt ";
        writeToFile(16, 4);                        // Format chunk length
        writeToFile(1, 2);                         // PCM Encode
        writeToFile(1, 2);                         // Number of Channels
        writeToFile(sampleRate, 4);                // Sample Rate
        writeToFile(sampleRate * bitDepth / 8, 4); // Byte rate
        writeToFile(bitDepth / 8, 2);              // Block Align
        writeToFile(bitDepth, 2);                  // Bit depth
        audioFile << "data0000";
        preAudio = audioFile.tellp();
}

void OutFile::writeBuf(void *data, int bytes)
{
        audioFile.write(reinterpret_cast<const char *>(data), bytes);
}

OutFile::~OutFile()
{
        postAudio = audioFile.tellp();
        audioFile.seekp(preAudio - 4);
        writeToFile(postAudio - preAudio, 4);

        audioFile.seekp(4, std::ios::beg);
        writeToFile(postAudio - 8, 4);

        audioFile.close();
}