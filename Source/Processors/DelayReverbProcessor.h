#pragma once
#include "AudioProcessor.h"
#include <vector>
#include <array>

// dsp helper structures

struct DelayLine {
	std::vector<float> buffer;
	int writePos = 0;
	int mask = 0;

	void Resize(int sizeSamples);
	void Write(float sample);
	// reads with linear interpolation
	float Read(float delayInSamples) const;
};

// one-pole filter for damping
struct OnePole {
	float z1 = 0.0f;
	float Process(float in, float coeff);
};

// comb filter with feedback and damping
struct CombFilter {
	DelayLine delayLine;
	OnePole damper;
	float feedback = 0.0f;

	void Resize(int size);
	float Process(float input, float dampingAmount);
};

// all-pass diffuser
struct AllPassFilter {
	DelayLine delayLine;
	float feedback = 0.5f;

	void Resize(int size);
	float Process(float input);
};

class DelayReverbProcessor : public AudioProcessor {
public:
	DelayReverbProcessor();
	~DelayReverbProcessor() override = default;

	const char* GetName() const override { return "Delay & Reverb"; }
	std::string GetProcessorId() const override { return "DelayReverb"; }
	bool IsInstrument() const override { return false; }

	void PrepareToPlay(double sampleRate) override;
	void Reset() override;

	void Process(float* buffer, int numFrames, int numChannels,
				 std::vector<MIDIMessage>& mIDIMessages,
				 const ProcessContext& context) override;

	bool RenderCustomUI(const ImVec2& size) override;
private:
	// parameters
	Parameter* pDelayTime = nullptr;
	Parameter* pDelayFeedback = nullptr;
	Parameter* pDelayMix = nullptr;
	Parameter* pDelayPingPong = nullptr;
	Parameter* pDelayLowCut = nullptr;
	Parameter* pDelayHighCut = nullptr;

	Parameter* pRevSize = nullptr;
	Parameter* pRevDecay = nullptr;
	Parameter* pRevDamp = nullptr;
	Parameter* pRevMix = nullptr;

	double mSampleRate = 48000.0;

	// delay state
	DelayLine mDelayL, mDelayR;
	OnePole mLpL, mLpR;
	OnePole mHpL, mHpR;
	float mHpStateL = 0.0f;
	float mHpStateR = 0.0f;

	// reverb state
	// freeverb / schroeder topology
	static const int kNumCombs = 4;
	static const int kNumAllPass = 2;

	struct ReverbChannel {
		std::array<CombFilter, kNumCombs> combs;
		std::array<AllPassFilter, kNumAllPass> allpasses;
	};

	ReverbChannel mRevL;
	ReverbChannel mRevR;

	// fixed tunings
	const int kCombTuningsL[4] = {1116, 1188, 1277, 1356};
	const int kCombTuningsR[4] = {1139, 1211, 1305, 1381};
	const int kAllPassTuningsL[2] = {556, 441};
	const int kAllPassTuningsR[2] = {579, 464};

	// ui calculation helper
	std::vector<float> mVisCurve;
	void RecalcVisCurve();
};
