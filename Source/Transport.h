#pragma once
#include <atomic>
#include <cstdint>

class Transport {
public:
	Transport();
	~Transport() = default;

	// non-copyable due to atomics
	Transport(const Transport&) = delete;
	Transport& operator=(const Transport&) = delete;

	void Play();
	void Stop();  // stops and rewinds to 0
	void Pause(); // stops but keeps position

	void SetPlaying(bool playing);
	bool IsPlaying() const;

	void SetPosition(int64_t sample);
	int64_t GetPosition() const;

	void SetBpm(double bpm);
	double GetBpm() const;

	void SetSampleRate(double sampleRate);
	double GetSampleRate() const;

	// loop control
	void SetLoopEnabled(bool enabled);
	bool IsLoopEnabled() const;
	void SetLoopRange(int64_t startSample, int64_t endSample);
	int64_t GetLoopStart() const;
	int64_t GetLoopEnd() const;

	// called by audio engine to advance time
	void Advance(int numFrames);
private:
	std::atomic<bool> mIsPlaying;
	std::atomic<int64_t> mCurrentSample;
	std::atomic<double> mBpm;
	std::atomic<double> mSampleRate;

	std::atomic<bool> mLoopEnabled;
	std::atomic<int64_t> mLoopStart;
	std::atomic<int64_t> mLoopEnd;
};
