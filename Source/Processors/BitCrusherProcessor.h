#pragma once
#include "AudioProcessor.h"
#include <vector>

class BitCrusherProcessor : public AudioProcessor {
public:
	BitCrusherProcessor();
	~BitCrusherProcessor() override = default;

	const char* GetName() const override { return "Bit Crusher"; }
	std::string GetProcessorId() const override { return "BitCrusher"; }
	bool IsInstrument() const override { return false; }

	void PrepareToPlay(double sampleRate) override;

	void Process(float* buffer, int numFrames, int numChannels,
				 std::vector<MIDIMessage>& mIDIMessages,
				 const ProcessContext& context) override;
private:
	Parameter* pBitDepth = nullptr;
	Parameter* pDownsample = nullptr;
	Parameter* pDrive = nullptr;

	// per-channel state for downsampling
	struct ChannelState {
		float phasor = 0.0f;
		float lastSample = 0.0f;
	};
	std::vector<ChannelState> mChannelStates;
};
