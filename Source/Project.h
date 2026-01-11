#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include "Track.h"
#include "Transport.h"

class Project {
public:
	Project();
	~Project();

	void Initialize();

	Transport& GetTransport() { return mTransport; }
	std::vector<std::shared_ptr<Track>>& GetTracks() { return mTracks; }
	std::shared_ptr<Track> GetMasterTrack() { return mMasterTrack; }

	// track management
	void CreateTrack();
	void RemoveTrack(int index);
	void MoveTrack(int srcIndex, int dstIndex, bool asChild);

	// grouping
	void GroupSelectedTracks(const std::set<int>& indices);
	void UngroupTrack(int trackIndex);
	void CheckEmptyGroups();

	// selection
	void SetSelectedTrack(int index);

	void PrepareToPlay(double sampleRate);

	// set bpm
	void SetBpm(double bpm);

	// audio callback
	void ProcessBlock(float* outputBuffer, int numFrames, int numChannels, std::vector<MIDIMessage>& liveMIDIEvents);

	// wav export
	bool RenderAudio(const std::string& path, double startBeat, double endBeat, double sampleRate = 48000.0);

	std::mutex& GetMutex() { return mMutex; }

	// serialization
	void Save(const std::string& path);
	void Load(const std::string& path);
private:
	Transport mTransport;
	std::vector<std::shared_ptr<Track>> mTracks;
	std::shared_ptr<Track> mMasterTrack;

	std::vector<float> mMixBuffer;
	bool mWasPlaying = false;
	int mSelectedTrackIndex = 0;

	// core dsp processing
	void ProcessAudioGraph(float* destinationBuffer, int numFrames, int numChannels, const ProcessContext& context, const std::vector<MIDIMessage>& liveMIDIEvents, bool anySolo);

	// recursive track helper
	void ProcessTrackRecursively(std::shared_ptr<Track> track, float* accumulationBuffer, int numFrames, int numChannels, const ProcessContext& context, const std::vector<MIDIMessage>& liveMIDIEvents, bool anySolo);

	// internal helper
	void PrepareToPlayInternal(double sampleRate);

	std::mutex mMutex;
};
