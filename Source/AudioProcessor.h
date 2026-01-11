#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include "MIDITypes.h"
#include "imgui.h"

// parameter structure for ui/audio interaction
struct Parameter {
	std::string name;
	float value;
	float minVal;
	float maxVal;

	Parameter(const std::string& n, float val, float min, float max)
		: name(n), value(val), minVal(min), maxVal(max) {}
};

// process context with transport information
struct ProcessContext {
	double sampleRate = 48000.0;
	int64_t currentSample = 0; // samples relative to project start
	double bpm = 120.0;
	bool isPlaying = false;
	double timeSigNumerator = 4.0;
	double timeSigDenominator = 4.0;
};

// base class for audio processors
class AudioProcessor {
public:
	virtual ~AudioProcessor() = default;

	// ui display name
	virtual const char* GetName() const { return "Audio Processor"; }

	// serialization id
	virtual std::string GetProcessorId() const { return "Unknown"; }

	// check if processor is instrument
	virtual bool IsInstrument() const { return false; }

	// setup
	virtual void PrepareToPlay(double sampleRate) = 0;

	// reset state
	virtual void Reset() {}

	// process block
	virtual void Process(float* buffer, int numFrames, int numChannels,
						 std::vector<MIDIMessage>& mIDIMessages,
						 const ProcessContext& context) = 0;

	// get parameters
	const std::vector<std::unique_ptr<Parameter>>& GetParameters() const { return mParameters; }

	// bypass state
	bool IsBypassed() const { return mIsBypassed; }
	void SetBypassed(bool bypassed) { mIsBypassed = bypassed; }

	// VST editor support
	virtual bool HasEditor() const { return false; }

	// open editor with window handle
	virtual void OpenEditor(void* parentWindowHandle = nullptr) { (void)parentWindowHandle; }

	virtual bool RenderCustomUI(const ImVec2& size) {
		(void)size;
		return false;
	}

	// serialization
	virtual void Save(std::ostream& out) {
		out << "PARAMS_BEGIN\n";
		for (const auto& p : mParameters) {
			out << "P \"" << p->name << "\" " << p->value << "\n";
		}
		out << "PARAMS_END\n";
	}

	virtual void Load(std::istream& in) {
		std::string line;
		while (std::getline(in, line)) {
			if (line == "PARAMS_END")
				break;
			if (line.rfind("P ", 0) == 0) {
				size_t q1 = line.find('"');
				size_t q2 = line.find('"', q1 + 1);
				if (q1 != std::string::npos && q2 != std::string::npos) {
					std::string pName = line.substr(q1 + 1, q2 - q1 - 1);
					std::string valStr = line.substr(q2 + 1);
					float val = std::stof(valStr);

					for (auto& p : mParameters) {
						if (p->name == pName) {
							p->value = val;
							break;
						}
					}
				}
			}
		}
	}
protected:
	std::vector<std::unique_ptr<Parameter>> mParameters;
	bool mIsBypassed = false;

	// create parameter helper
	Parameter* AddParameter(const std::string& name, float defaultVal, float minVal, float maxVal) {
		mParameters.push_back(std::make_unique<Parameter>(name, defaultVal, minVal, maxVal));
		return mParameters.back().get();
	}
};
