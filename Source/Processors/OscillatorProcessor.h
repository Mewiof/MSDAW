#pragma once
#include "AudioProcessor.h"
#include <cmath>

class OscillatorProcessor : public AudioProcessor {
public:
	void PrepareToPlay(double sampleRate) override {
		mSampleRate = sampleRate;
	}

	void Process(float* buffer, int numFrames, int numChannels,
				 std::vector<MIDIMessage>& mIDIMessages,
				 const ProcessContext& context) override {
		(void)mIDIMessages;
		(void)context;

		for (int i = 0; i < numFrames; ++i) {
			float sample = 0.2f * std::sin(static_cast<float>(mPhase));
			for (int c = 0; c < numChannels; ++c) {
				buffer[i * numChannels + c] += sample;
			}
			mPhase += 2.0 * 3.14159265359 * mFrequency / mSampleRate;
			if (mPhase >= 2.0 * 3.14159265359) {
				mPhase -= 2.0 * 3.14159265359;
			}
		}
	}

	void SetFrequency(float freq) { mFrequency = freq; }
private:
	double mSampleRate = 48000.0;
	double mPhase = 0.0;
	double mFrequency = 440.0; // a4
};
