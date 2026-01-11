#pragma once
#include "Clip.h"
#include <vector>
#include <string>

enum class WarpMode {
	Beats = 0,
	Tones,
	Texture,
	RePitch,
	Complex,
	ComplexPro
};

class AudioClip : public Clip {
public:
	AudioClip();
	~AudioClip() override = default;

	// simple wav loader (supports 16-bit pcm and 32-bit float)
	bool LoadFromFile(const std::string& path);

	// generate a test tone for demonstration
	void GenerateTestSignal(double sampleRate, double durationSecs);

	// access raw interleaved samples
	const std::vector<float>& GetSamples() const { return mSamples; }
	int GetNumChannels() const { return mChannels; }
	double GetSampleRate() const { return mSampleRate; }
	uint64_t GetTotalFileFrames() const { return mTotalFileFrames; }

	// warping and pitch properties
	void SetWarpingEnabled(bool enabled) { mWarpingEnabled = enabled; }
	bool IsWarpingEnabled() const { return mWarpingEnabled; }

	void SetWarpMode(WarpMode mode) { mWarpMode = mode; }
	WarpMode GetWarpMode() const { return mWarpMode; }

	void SetSegmentBpm(double bpm) { mSegmentBpm = bpm; }
	double GetSegmentBpm() const { return mSegmentBpm; }

	void SetTransposeSemitones(double semitones) { mTransposeSemitones = semitones; }
	double GetTransposeSemitones() const { return mTransposeSemitones; }

	void SetTransposeCents(double cents) { mTransposeCents = cents; }
	double GetTransposeCents() const { return mTransposeCents; }

	// helper to calculate max duration in beats
	double GetMaxDurationInBeats(double projectBpm) const;
	// clamps duration to file end based on project bpm
	void ValidateDuration(double projectBpm);

	void Save(std::ostream& out) override;
	void Load(std::istream& in) override;
private:
	std::vector<float> mSamples; // interleaved data
	int mChannels = 2;
	double mSampleRate = 48000.0;
	uint64_t mTotalFileFrames = 0;
	std::string mFilePath;

	// warping state
	bool mWarpingEnabled = false;
	WarpMode mWarpMode = WarpMode::Beats;
	double mSegmentBpm = 120.0;
	double mTransposeSemitones = 0.0;
	double mTransposeCents = 0.0;
};
