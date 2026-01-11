#pragma once
#include <string>
#include <memory>
#include <set>
#include <vector>
#include "AudioEngine.h"
#include "Clip.h"
#include "PluginManager.h"
#include "AudioProcessor.h"
#include "Track.h"
#include "imgui.h"

// imgui math helpers
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

struct EditorLayout {
	float transportHeight = 60.0f;
	float bottomPanelHeight = 220.0f;
	float libraryWidth = 200.0f;
	float trackListWidth = 240.0f;
	float trackRowHeight = 80.0f;
	float trackGap = 8.0f;

	void Scale(float scale) {
		transportHeight *= scale;
		bottomPanelHeight *= scale;
		libraryWidth *= scale;
		trackListWidth *= scale;
		trackRowHeight *= scale;
		trackGap *= scale;
	}
};

enum class FollowMode {
	Page,
	Continuous
};

struct EditorState {
	float mainScale = 1.0f;
	float pixelsPerBeat = 60.0f;
	double timelineGrid = 1.0;
	float scrollY = 0.0f; // master timeline scroll y

	// timeline selection (beats)
	double selectionStart = 0.0;
	double selectionEnd = 0.0;

	// follow playback
	bool followPlayback = false;
	FollowMode followMode = FollowMode::Continuous; // continuous scroll mode

	// selection
	int selectedTrackIndex = 0;		   // primary selection
	std::set<int> multiSelectedTracks; // multi-selection

	std::shared_ptr<Clip> selectedClip = nullptr;

	// clipboard
	std::shared_ptr<AudioProcessor> processorClipboard = nullptr;

	// automation clipboard
	std::vector<AutomationPoint> automationClipboard;

	// window visibility
	bool showSettingsWindow = false;

	// MIDI keyboard state
	bool isComputerMIDIKeyboardEnabled = true;
	int mIDIOctave = 3;		// base octave
	int mIDIVelocity = 100; // default velocity

	// active keyboard notes
	std::set<int> activeMIDINotes;

	// os drag and drop
	std::string droppedPath;
	float dropX = 0.0f;
	float dropY = 0.0f;
	bool processDrop = false;

	// os drag preview
	bool isOsDragging = false;
	float osDragX = 0.0f;
	float osDragY = 0.0f;
	double lastOsDragTime = 0.0; // drag anti-flicker timer
};

struct EditorContext {
	AudioEngine& engine;
	EditorState state;
	EditorLayout layout;
	PluginManager pluginManager;

	// native window handle
	void* nativeWindowHandle = nullptr;

	EditorContext(AudioEngine& e)
		: engine(e) {}
	Project* GetProject() { return engine.GetProject(); }
};
