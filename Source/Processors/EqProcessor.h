#pragma once
#include "AudioProcessor.h"
#include <vector>
#include <complex>

enum class FilterType {
	LowCut = 0,
	LowShelf,
	Bell,
	HighShelf,
	HighCut
};

enum class EqMode {
	Stereo = 0,
	Left,
	Right,
	Mid,
	Side
};

struct BiquadCoeffs {
	double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
};

// band state per channel
struct BiquadState {
	double z1 = 0;
	double z2 = 0;
};

class EqProcessor : public AudioProcessor {
public:
	EqProcessor();
	~EqProcessor() override = default;

	const char* GetName() const override { return "EQ Eight"; }
	std::string GetProcessorId() const override { return "EqEight"; }

	void PrepareToPlay(double sampleRate) override;
	void Reset() override;
	void Process(float* buffer, int numFrames, int numChannels,
				 std::vector<MIDIMessage>& mIDIMessages,
				 const ProcessContext& context) override;

	// custom graph ui
	bool RenderCustomUI(const ImVec2& size) override;
private:
	static const int kNumBands = 8;

	struct BandParams {
		Parameter* pFreq = nullptr;
		Parameter* pGain = nullptr;
		Parameter* pQ = nullptr;
		Parameter* pType = nullptr;
		Parameter* pActive = nullptr;

		BiquadCoeffs coeffs;
	};

	std::vector<BandParams> mBands;

	// global parameters
	Parameter* pGlobalGain = nullptr;
	Parameter* pScale = nullptr;
	Parameter* pAdaptQ = nullptr;
	Parameter* pMode = nullptr;

	// [channel][band]
	std::vector<std::vector<BiquadState>> mStates;
	double mSampleRate = 48000.0;

	void RecalculateCoeffs(int bandIdx);

	// ui helpers
	float GetMagnitudeForFreq(double freq);
	std::complex<double> GetBiquadResponse(const BiquadCoeffs& coeffs, double freq);

	// ui state
	int mSelectedBandIndex = 0;
	bool mDraggingNode = false;
};
