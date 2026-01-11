#pragma once
#include "AudioProcessor.h"
#include <array>
#include <vector>

class OTTProcessor : public AudioProcessor {
public:
	OTTProcessor();
	~OTTProcessor() override = default;

	const char* GetName() const override { return "OTT Multiband"; }
	std::string GetProcessorId() const override { return "OTT"; }
	bool IsInstrument() const override { return false; }

	void PrepareToPlay(double sampleRate) override;
	void Reset() override;

	void Process(float* buffer, int numFrames, int numChannels,
				 std::vector<MIDIMessage>& mIDIMessages,
				 const ProcessContext& context) override;

	bool RenderCustomUI(const ImVec2& size) override;
private:
	// parameters
	Parameter* pDepth = nullptr;
	Parameter* pTime = nullptr;
	Parameter* pInGain = nullptr;
	Parameter* pOutGain = nullptr;

	Parameter* pLowGain = nullptr;
	Parameter* pMidGain = nullptr;
	Parameter* pHighGain = nullptr;

	const float kFreqLow = 88.3f;
	const float kFreqHigh = 2500.0f;

	double mSampleRate = 48000.0;

	// dsp helpers
	struct Biquad {
		float b0 = 0, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
		float z1 = 0, z2 = 0;

		void CalcLowPass(float freq, float q, float sampleRate);
		void CalcHighPass(float freq, float q, float sampleRate);
		float Process(float in);
	};

	struct CompressorBand {
		float rmsState = 0.0f;
		float gainReduction = 1.0f;
		float visualGain = 1.0f;

		float GetGainForSample(float sample, float timeScale, float sampleRate);
	};

	struct ChannelState {
		Biquad lpLow;
		Biquad hpLow;
		Biquad lpHigh;
		Biquad hpHigh;

		std::array<CompressorBand, 3> bands;
	};

	std::vector<ChannelState> mChannels;
};
