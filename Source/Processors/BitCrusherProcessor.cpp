#include "PrecompHeader.h"
#include "BitCrusherProcessor.h"
#include <cmath>
#include <algorithm>

BitCrusherProcessor::BitCrusherProcessor() {
	pBitDepth = AddParameter("Bits", 24.0f, 1.0f, 24.0f);
	pDownsample = AddParameter("Downsample", 1.0f, 1.0f, 40.0f);
	pDrive = AddParameter("Drive dB", 0.0f, 0.0f, 30.0f);
}

void BitCrusherProcessor::PrepareToPlay(double sampleRate) {
	// clear states
	mChannelStates.clear();
	(void)sampleRate;
}

void BitCrusherProcessor::Process(float* buffer, int numFrames, int numChannels,
								  std::vector<MIDIMessage>& mIDIMessages,
								  const ProcessContext& context) {
	(void)mIDIMessages;
	(void)context;

	// ensure channel states exist
	if (mChannelStates.size() < (size_t)numChannels) {
		mChannelStates.resize(numChannels, {0.0f, 0.0f});
	}

	float bits = pBitDepth->value;
	float downsampleIdx = pDownsample->value;
	float drive = std::pow(10.0f, pDrive->value / 20.0f);

	float maxVal = std::pow(2.0f, bits);

	for (int i = 0; i < numFrames; ++i) {
		for (int c = 0; c < numChannels; ++c) {
			ChannelState& st = mChannelStates[c];

			// 1. advance phasor
			st.phasor += 1.0f;

			// 2. resample
			if (st.phasor >= downsampleIdx) {
				st.phasor -= downsampleIdx;

				// drive input
				float in = buffer[i * numChannels + c] * drive;

				// soft clip
				if (in > 1.0f)
					in = 1.0f;
				if (in < -1.0f)
					in = -1.0f;

				// bit reduction logic
				float norm = (in + 1.0f) * 0.5f;
				// quantize
				float step = std::floor(norm * maxVal);
				float quant = step / maxVal;
				// expand back to -1..1
				float out = (quant * 2.0f) - 1.0f;

				st.lastSample = out;
			}

			// 3. write output
			buffer[i * numChannels + c] = st.lastSample;
		}
	}
}
