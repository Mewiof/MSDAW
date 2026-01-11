#include "PrecompHeader.h"
#include "AudioEngine.h"
#include <iostream>
#include <algorithm>

static int AudioCallbackWrapper(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
								double streamTime, RtAudioStreamStatus status, void* userData) {
	AudioEngine* engine = static_cast<AudioEngine*>(userData);
	return engine->OnAudioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status, userData);
}

AudioEngine::AudioEngine()
	: dac(RtAudio::UNSPECIFIED) {
}

AudioEngine::~AudioEngine() {
	Stop();
	if (isStreamOpen) {
		dac.closeStream();
	}
}

bool AudioEngine::Init() {
	if (dac.getDeviceCount() < 1) {
		std::cout << "\nNo audio devices found!\n";
		return false;
	}

	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;

	sampleRate = 48000.0;
	unsigned int bufferFrames = 512;

	// initialize project
	mProject = std::make_unique<Project>();
	mProject->Initialize();
	mProject->PrepareToPlay(sampleRate);

	try {
		dac.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, (unsigned int)sampleRate, &bufferFrames, &AudioCallbackWrapper, (void*)this);
		isStreamOpen = true;
	} catch (std::exception& e) {
		std::cout << '\n'
				  << e.what() << '\n';
		return false;
	}

	return true;
}

void AudioEngine::Start() {
	if (isStreamOpen) {
		try {
			dac.startStream();
		} catch (std::exception& e) {
			std::cout << '\n'
					  << e.what() << '\n';
		}
	}
}

void AudioEngine::Stop() {
	if (isStreamOpen && dac.isStreamRunning()) {
		try {
			dac.stopStream();
		} catch (std::exception& e) {
			std::cout << '\n'
					  << e.what() << '\n';
		}
	}
}

bool AudioEngine::IsStreamRunning() const {
	return isStreamOpen && dac.isStreamRunning();
}

void AudioEngine::SendMIDIEvent(int status, int note, int velocity) {
	std::lock_guard<std::mutex> lock(mMIDIMutex);
	MIDIMessage msg;
	msg.status = (uint8_t)status;
	msg.data1 = (uint8_t)note;
	msg.data2 = (uint8_t)velocity;
	msg.frameIndex = 0;
	mPendingMIDIMessages.push_back(msg);
}

int AudioEngine::OnAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
								 double streamTime, RtAudioStreamStatus status, void* userData) {
	float* out = (float*)outputBuffer;

	(void)inputBuffer;
	(void)streamTime;
	(void)status;
	(void)userData;

	// 1. retrieve pending live MIDI events
	std::vector<MIDIMessage> blockMIDIEvents;
	{
		std::lock_guard<std::mutex> lock(mMIDIMutex);
		if (!mPendingMIDIMessages.empty()) {
			blockMIDIEvents = mPendingMIDIMessages;
			mPendingMIDIMessages.clear();
		}
	}

	// 2. clear output buffer
	std::fill(out, out + (nBufferFrames * 2), 0.0f);

	// 3. process project
	if (mProject) {
		mProject->ProcessBlock(out, nBufferFrames, 2, blockMIDIEvents);
	}

	return 0;
}
