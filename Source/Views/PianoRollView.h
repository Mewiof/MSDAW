#pragma once
#include "EditorContext.h"
#include "imgui.h"
#include <vector>
#include <map>

// struct to store initial state of a note before a drag operation begins
struct NoteDragState {
	double originalStart;
	double originalDuration;
	int originalNoteNum;
};

class PianoRollView {
public:
	PianoRollView(EditorContext& context)
		: mContext(context) {}
	void Render();
private:
	EditorContext& mContext;

	// selection state
	std::vector<int> mSelectedIndices;

	// interaction state
	enum class InteractionMode {
		None,
		Selecting,	   // marquee box
		MovingNotes,   // dragging notes
		ResizingNotes, // resizing notes
		PianoKeyInput  // clicking the left piano keys
	};

	InteractionMode mInteraction = InteractionMode::None;

	// for dragging notes (move/resize)
	// maps note index -> initial state
	std::map<int, NoteDragState> mDragInitialStates;

	// for marquee selection
	ImVec2 mMarqueeStart = {0, 0};
	ImVec2 mMarqueeEnd = {0, 0};

	// audio preview state
	int mLastPreviewNote = -1; // -1 means no note playing

	// settings
	float mPrSnapGrid = 0.25f;

	// helpers
	void ClearSelection() { mSelectedIndices.clear(); }
	bool IsNoteSelected(int index);
	void SelectNote(int index, bool addToSelection);
};
