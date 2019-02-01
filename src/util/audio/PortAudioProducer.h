/*
 * Xournal++
 *
 * Class to record audio using libportaudio
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <XournalType.h>

#include "DeviceInfo.h"
#include "AudioQueue.h"

#include <control/settings/Settings.h>

#include <list>

#include <portaudiocpp/PortAudioCpp.hxx>

class PortAudioProducer
{
public:
    explicit PortAudioProducer(Settings *settings, AudioQueue *audioQueue);
    ~PortAudioProducer();

public:
    std::list<DeviceInfo> getInputDevices();
    const DeviceInfo getSelectedInputDevice();
    void setInputDevice(DeviceInfo deviceInfo);
    bool isRecording();
    void startRecording();
    int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);
    void stopRecording();

private:
    XOJ_TYPE_ATTRIB;

protected:
    double sampleRate = 44100.0;
    const unsigned long framesPerBuffer = 64;

    portaudio::AutoSystem autoSys;
    portaudio::System& sys;
    Settings* settings;
    AudioQueue* audioQueue;

    PaDeviceIndex selectedInputDevice = 0;
    unsigned int inputChannels = 0;

    portaudio::MemFunCallbackStream<PortAudioProducer>* inputStream = nullptr;
};

