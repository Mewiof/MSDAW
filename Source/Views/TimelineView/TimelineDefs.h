#pragma once
#include "Clip.h"
#include "Clips/AudioClip.h"
#include "Clips/MIDIClip.h"
#include "imgui.h"
#include <memory>
#include <map>
#include <vector>

// shared payload for drag-and-drop of devices
struct DeviceMovePayload {
	int trackIndex;
	int deviceIndex;
};

struct PendingClipMove {
	std::shared_ptr<Clip> clip;
	int fromTrackIdx;
	int toTrackIdx;
	double newStartBeat;
	bool valid = false;
};

struct PendingClipDelete {
	std::shared_ptr<Clip> clip;
	int trackIdx;
	bool valid = false;
};

enum class DragState {
	None,
	Moving,
	ResizingLeft,
	ResizingRight
};

// consolidated interaction state to pass between view, ruler, and tracks
struct TimelineInteractionState {
	// clip dragging
	DragState dragState = DragState::None;
	double dragOriginalStart = 0.0;
	double dragOriginalDuration = 0.0;
	double dragOriginalOffset = 0.0;

	// live dragging state (preview)
	int dragSourceTrackIdx = -1;
	int dragTargetTrackIdx = -1;

	// dynamic values calculated during drag
	double dragCurrentBeat = 0.0;
	double dragCurrentDuration = 0.0;
	double dragCurrentOffset = 0.0;

	// automation dragging
	int autoDragTrackIndex = -1;
	int autoDragPointIndex = -1;
	bool autoDragIsTension = false;
	float dragStartY = 0.0f;
	float dragStartVal = 0.0f;

	// automation dragging multiple points: maps index -> original state (beat, value)
	std::map<int, std::pair<double, float>> autoDragInitialStates;

	// automation selection state (marquee)
	bool autoMarqueeActive = false;
	ImVec2 autoMarqueeStart = {0, 0};
	ImVec2 autoMarqueeCurrent = {0, 0};

	// ruler selection
	double selectionDragStart = 0.0;

	// clipboard
	std::shared_ptr<Clip> clipboard;

	// renaming
	std::shared_ptr<Clip> clipToRename = nullptr;
	bool triggerRenamePopup = false;
	char renameBuffer[256] = "";
};

// helper to clone clips
inline std::shared_ptr<Clip> CloneClip(std::shared_ptr<Clip> source) {
	if (!source)
		return nullptr;
	if (auto ac = std::dynamic_pointer_cast<AudioClip>(source)) {
		return std::make_shared<AudioClip>(*ac);
	}
	if (auto mc = std::dynamic_pointer_cast<MIDIClip>(source)) {
		return std::make_shared<MIDIClip>(*mc);
	}
	return nullptr;
}
