#include "PrecompHeader.h"
#include "Transport.h"

Transport::Transport()
	: mIsPlaying(false), mCurrentSample(0), mBpm(120.0), mSampleRate(48000.0),
	  mLoopEnabled(false), mLoopStart(0), mLoopEnd(0) {
}

void Transport::Play() {
	mIsPlaying = true;
}

void Transport::Stop() {
	mIsPlaying = false;
	mCurrentSample = 0;
}

void Transport::Pause() {
	mIsPlaying = false;
}

void Transport::SetPlaying(bool playing) {
	mIsPlaying = playing;
}

bool Transport::IsPlaying() const {
	return mIsPlaying;
}

void Transport::SetPosition(int64_t sample) {
	mCurrentSample = sample;
}

int64_t Transport::GetPosition() const {
	return mCurrentSample;
}

void Transport::SetBpm(double bpm) {
	mBpm = bpm;
}

double Transport::GetBpm() const {
	return mBpm;
}

void Transport::SetSampleRate(double sampleRate) {
	mSampleRate = sampleRate;
}

double Transport::GetSampleRate() const {
	return mSampleRate;
}

void Transport::SetLoopEnabled(bool enabled) {
	mLoopEnabled = enabled;
}

bool Transport::IsLoopEnabled() const {
	return mLoopEnabled;
}

void Transport::SetLoopRange(int64_t startSample, int64_t endSample) {
	mLoopStart = startSample;
	mLoopEnd = endSample;
}

int64_t Transport::GetLoopStart() const {
	return mLoopStart;
}

int64_t Transport::GetLoopEnd() const {
	return mLoopEnd;
}

void Transport::Advance(int numFrames) {
	if (mIsPlaying) {
		mCurrentSample += numFrames;
	}
}
