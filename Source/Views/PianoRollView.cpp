#include "PrecompHeader.h"
#include "PianoRollView.h"
#include "Clips/MIDIClip.h"
#include "Clips/AudioClip.h"
#include "Project.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// helper to check if a point is inside a rect
static bool IsPointInRect(const ImVec2& p, const ImVec2& min, const ImVec2& max) {
	return (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y);
}

// helper to check rect overlap
static bool RectOverlap(const ImVec2& minA, const ImVec2& maxA, const ImVec2& minB, const ImVec2& maxB) {
	return (minA.x < maxB.x && maxA.x > minB.x && minA.y < maxB.y && maxA.y > minB.y);
}

bool PianoRollView::IsNoteSelected(int index) {
	for (int i : mSelectedIndices) {
		if (i == index)
			return true;
	}
	return false;
}

void PianoRollView::SelectNote(int index, bool addToSelection) {
	if (!addToSelection) {
		mSelectedIndices.clear();
		mSelectedIndices.push_back(index);
	} else {
		// toggle
		auto it = std::find(mSelectedIndices.begin(), mSelectedIndices.end(), index);
		if (it != mSelectedIndices.end())
			mSelectedIndices.erase(it);
		else
			mSelectedIndices.push_back(index);
	}
}

void PianoRollView::Render() {
	auto selectedClip = mContext.state.selectedClip;
	if (!selectedClip)
		return;

	if (std::dynamic_pointer_cast<MIDIClip>(selectedClip)) {
		ImGui::SetNextWindowSize(ImVec2(600 * mContext.state.mainScale, 400 * mContext.state.mainScale), ImGuiCond_FirstUseEver);
		ImGui::Begin("Piano Roll", nullptr);

		auto mIDIClip = std::dynamic_pointer_cast<MIDIClip>(selectedClip);
		Project* project = mContext.GetProject();
		Transport* transport = project ? &project->GetTransport() : nullptr;

		ImGui::Text("Editing Clip: %s", mIDIClip->GetName().c_str());
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100 * mContext.state.mainScale);
		ImGui::InputFloat("Snap", &mPrSnapGrid, 0.0f, 0.0f, "%.2f");
		if (mPrSnapGrid < 0.01f)
			mPrSnapGrid = 0.01f;

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(Ctrl+A: Select All | Del: Remove)");

		ImGui::Separator();

		const float NOTE_HEIGHT = 16.0f * mContext.state.mainScale;
		const float KEY_WIDTH = 40.0f * mContext.state.mainScale;
		const float PIXELS_PER_BEAT_PR = 100.0f * mContext.state.mainScale;

		ImGui::BeginChild("PianoRollGrid", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		float scrollX = ImGui::GetScrollX();

		// determine content size
		double maxDuration = std::max(mIDIClip->GetDuration(), 4.0);
		for (const auto& n : mIDIClip->GetNotesEx()) {
			if (n.startBeat + n.durationBeats > maxDuration)
				maxDuration = n.startBeat + n.durationBeats;
		}

		float contentWidth = (float)(maxDuration + 1.0) * PIXELS_PER_BEAT_PR;
		float contentHeight = 128.0f * NOTE_HEIGHT;

		// input handling setup
		ImGui::InvisibleButton("##pr_canvas", ImVec2(contentWidth + KEY_WIDTH, contentHeight));

		bool isHovered = ImGui::IsItemHovered();
		bool isActive = ImGui::IsItemActive();
		bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
		bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		bool isMouseDoubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
		ImVec2 mousePos = ImGui::GetMousePos();

		// calculate coordinates relative to canvas
		float mouseRelX = mousePos.x - canvasPos.x;
		float mouseRelY = mousePos.y - canvasPos.y;

		// calculate piano roll logic coordinates
		bool inKeyArea = (mouseRelX < KEY_WIDTH + scrollX);
		inKeyArea = (mousePos.x < canvasPos.x + KEY_WIDTH); // screen space check is safer

		double mouseBeat = (double)(mouseRelX - KEY_WIDTH) / PIXELS_PER_BEAT_PR;
		if (mouseBeat < 0)
			mouseBeat = 0;
		int mouseNoteRow = (int)(mouseRelY / NOTE_HEIGHT);
		int mouseNoteNum = 127 - mouseNoteRow;

		auto& notes = mIDIClip->GetNotesEx();

		// 1. keyboard shortcuts
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
			if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A)) {
				mSelectedIndices.clear();
				for (int i = 0; i < (int)notes.size(); ++i)
					mSelectedIndices.push_back(i);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
				// delete back to front to preserve indices
				std::sort(mSelectedIndices.rbegin(), mSelectedIndices.rend());
				for (int idx : mSelectedIndices) {
					if (idx >= 0 && idx < (int)notes.size()) {
						notes.erase(notes.begin() + idx);
					}
				}
				mSelectedIndices.clear();
			}
		}

		// 2. interaction logic

		// a. piano keys preview (left panel)
		if (isMouseDown && inKeyArea && isHovered) {
			mInteraction = InteractionMode::PianoKeyInput;
			if (mouseNoteNum != mLastPreviewNote) {
				// stop old
				if (mLastPreviewNote != -1)
					mContext.engine.SendMIDIEvent(0x80, mLastPreviewNote, 0);
				// start new
				if (mouseNoteNum >= 0 && mouseNoteNum <= 127) {
					mContext.engine.SendMIDIEvent(0x90, mouseNoteNum, 100);
					mLastPreviewNote = mouseNoteNum;
				}
			}
		}
		// note off logic (global release or moved off keys)
		else {
			if (mLastPreviewNote != -1 && (!isMouseDown || !inKeyArea)) {
				mContext.engine.SendMIDIEvent(0x80, mLastPreviewNote, 0);
				mLastPreviewNote = -1;
			}
		}

		// b. grid interaction
		if (isHovered && !inKeyArea) {

			// detect hit on notes
			int hitIndex = -1;
			bool hittingResizeRight = false;

			// check intersection (iterate backwards for z-order)
			for (int i = (int)notes.size() - 1; i >= 0; --i) {
				const auto& note = notes[i];
				float nx = canvasPos.x + KEY_WIDTH + (float)(note.startBeat * PIXELS_PER_BEAT_PR);
				float ny = canvasPos.y + ((127 - note.noteNumber) * NOTE_HEIGHT);
				float nw = (float)(note.durationBeats * PIXELS_PER_BEAT_PR);

				if (mousePos.x >= nx && mousePos.x <= nx + nw && mousePos.y >= ny && mousePos.y <= ny + NOTE_HEIGHT) {
					hitIndex = i;
					if (mousePos.x >= nx + nw - 8.0f)
						hittingResizeRight = true;
					break;
				}
			}

			// cursor
			if (hitIndex != -1) {
				ImGui::SetMouseCursor(hittingResizeRight ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_Hand);
			}

			// mouse down logic
			if (isMouseClicked) {
				if (hitIndex != -1) {
					// clicked a note
					if (ImGui::GetIO().KeyCtrl) {
						SelectNote(hitIndex, true); // toggle
					} else if (ImGui::GetIO().KeyShift) {
						if (!IsNoteSelected(hitIndex))
							SelectNote(hitIndex, true); // add
					} else {
						if (!IsNoteSelected(hitIndex))
							SelectNote(hitIndex, false); // select only this
					}

					// prepare drag
					mInteraction = hittingResizeRight ? InteractionMode::ResizingNotes : InteractionMode::MovingNotes;
					mDragInitialStates.clear();
					for (int idx : mSelectedIndices) {
						if (idx >= 0 && idx < (int)notes.size()) {
							NoteDragState s;
							s.originalStart = notes[idx].startBeat;
							s.originalDuration = notes[idx].durationBeats;
							s.originalNoteNum = notes[idx].noteNumber;
							mDragInitialStates[idx] = s;
						}
					}
				} else {
					// clicked empty space
					if (isMouseDoubleClicked) {
						// create note
						mSelectedIndices.clear();
						MIDINote newNote;
						newNote.noteNumber = std::clamp(mouseNoteNum, 0, 127);
						// snap calculation
						newNote.startBeat = std::round(mouseBeat / mPrSnapGrid) * mPrSnapGrid;
						if (newNote.startBeat < 0)
							newNote.startBeat = 0;
						newNote.durationBeats = mPrSnapGrid;
						newNote.velocity = 100;
						notes.push_back(newNote);
						mSelectedIndices.push_back((int)notes.size() - 1);
					} else {
						// start marquee or deselect
						if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
							mSelectedIndices.clear();
						}
						mInteraction = InteractionMode::Selecting;
						mMarqueeStart = mousePos;
						mMarqueeEnd = mousePos;
					}
				}
			}
		}

		// c. handling active drags
		if (isMouseDown && mInteraction != InteractionMode::None) {

			if (mInteraction == InteractionMode::Selecting) {
				mMarqueeEnd = mousePos;
				// update selection based on rect
				ImVec2 minR(std::min(mMarqueeStart.x, mMarqueeEnd.x), std::min(mMarqueeStart.y, mMarqueeEnd.y));
				ImVec2 maxR(std::max(mMarqueeStart.x, mMarqueeEnd.x), std::max(mMarqueeStart.y, mMarqueeEnd.y));

				if (!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
					mSelectedIndices.clear();
				}

				for (int i = 0; i < (int)notes.size(); ++i) {
					const auto& note = notes[i];
					float nx = canvasPos.x + KEY_WIDTH + (float)(note.startBeat * PIXELS_PER_BEAT_PR);
					float ny = canvasPos.y + ((127 - note.noteNumber) * NOTE_HEIGHT);
					float nw = (float)(note.durationBeats * PIXELS_PER_BEAT_PR);
					float nh = NOTE_HEIGHT;

					if (RectOverlap(minR, maxR, ImVec2(nx, ny), ImVec2(nx + nw, ny + nh))) {
						if (!IsNoteSelected(i))
							mSelectedIndices.push_back(i);
					}
				}
			} else if (mInteraction == InteractionMode::MovingNotes) {
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
				double deltaBeats = delta.x / PIXELS_PER_BEAT_PR;
				int deltaSemitones = -(int)(delta.y / NOTE_HEIGHT);

				for (auto& pair : mDragInitialStates) {
					int idx = pair.first;
					if (idx < 0 || idx >= (int)notes.size())
						continue;

					auto& note = notes[idx];
					auto& state = pair.second;

					double newStart = state.originalStart + deltaBeats;
					if (mPrSnapGrid > 0)
						newStart = std::round(newStart / mPrSnapGrid) * mPrSnapGrid;
					if (newStart < 0)
						newStart = 0;

					note.startBeat = newStart;
					note.noteNumber = std::clamp(state.originalNoteNum + deltaSemitones, 0, 127);
				}
			} else if (mInteraction == InteractionMode::ResizingNotes) {
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
				double deltaBeats = delta.x / PIXELS_PER_BEAT_PR;

				for (auto& pair : mDragInitialStates) {
					int idx = pair.first;
					if (idx < 0 || idx >= (int)notes.size())
						continue;

					auto& note = notes[idx];
					auto& state = pair.second;

					double newDur = state.originalDuration + deltaBeats;
					if (mPrSnapGrid > 0)
						newDur = std::round(newDur / mPrSnapGrid) * mPrSnapGrid;
					if (newDur < mPrSnapGrid)
						newDur = mPrSnapGrid;

					note.durationBeats = newDur;
				}
			}
		} else if (!isMouseDown) {
			mInteraction = InteractionMode::None;
		}

		// 3. rendering

		// a. background keys
		float keyDrawX = canvasPos.x + scrollX;

		for (int i = 0; i < 128; ++i) {
			int noteNum = 127 - i;
			float y = canvasPos.y + (i * NOTE_HEIGHT);
			int n = noteNum % 12;
			bool isBlack = (n == 1 || n == 3 || n == 6 || n == 8 || n == 10);

			bool isPreviewPlaying = (mLastPreviewNote == noteNum);
			bool isComputerKeyPressed = (mContext.state.activeMIDINotes.find(noteNum) != mContext.state.activeMIDINotes.end());

			ImU32 keyColor;
			if (isPreviewPlaying || isComputerKeyPressed) {
				keyColor = IM_COL32(255, 100, 100, 255);
			} else {
				keyColor = isBlack ? IM_COL32(50, 50, 50, 255) : IM_COL32(230, 230, 230, 255);
			}

			drawList->AddRectFilled(ImVec2(keyDrawX, y), ImVec2(keyDrawX + KEY_WIDTH, y + NOTE_HEIGHT), keyColor);
			drawList->AddRect(ImVec2(keyDrawX, y), ImVec2(keyDrawX + KEY_WIDTH, y + NOTE_HEIGHT), IM_COL32(0, 0, 0, 100));

			if (n == 0) {
				char buf[8];
				snprintf(buf, 8, "C%d", (noteNum / 12) - 1);
				drawList->AddText(ImVec2(keyDrawX + 3, y + 1), IM_COL32(0, 0, 0, 255), buf);
			}

			// horizontal grid lines
			float lineStart = canvasPos.x + KEY_WIDTH;
			float lineEnd = lineStart + contentWidth;
			ImU32 gridColor = isBlack ? IM_COL32(45, 45, 48, 255) : IM_COL32(60, 60, 65, 255);

			drawList->AddRectFilled(ImVec2(lineStart, y), ImVec2(lineEnd, y + NOTE_HEIGHT), gridColor);
			drawList->AddLine(ImVec2(lineStart, y + NOTE_HEIGHT), ImVec2(lineEnd, y + NOTE_HEIGHT), IM_COL32(30, 30, 30, 255));
		}

		// b. vertical grid lines (beats)
		int totalBeats = (int)(contentWidth / PIXELS_PER_BEAT_PR);
		for (int b = 0; b <= totalBeats; ++b) {
			float x = canvasPos.x + KEY_WIDTH + (b * PIXELS_PER_BEAT_PR);
			drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasPos.y + contentHeight), IM_COL32(100, 100, 100, 80));
		}

		// c. notes
		for (size_t i = 0; i < notes.size(); ++i) {
			const auto& note = notes[i];
			float x = canvasPos.x + KEY_WIDTH + (float)(note.startBeat * PIXELS_PER_BEAT_PR);
			float w = (float)(note.durationBeats * PIXELS_PER_BEAT_PR);
			float y = canvasPos.y + ((127 - note.noteNumber) * NOTE_HEIGHT);

			bool isSelected = IsNoteSelected((int)i);

			ImU32 fillColor = isSelected ? IM_COL32(255, 160, 160, 255) : IM_COL32(255, 100, 100, 200);
			ImU32 borderColor = isSelected ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 150);

			drawList->AddRectFilled(ImVec2(x, y + 1), ImVec2(x + w, y + NOTE_HEIGHT - 1), fillColor, 4.0f);
			drawList->AddRect(ImVec2(x, y + 1), ImVec2(x + w, y + NOTE_HEIGHT - 1), borderColor, 4.0f);
		}

		// d. marquee selection rect
		if (mInteraction == InteractionMode::Selecting) {
			ImVec2 minR(std::min(mMarqueeStart.x, mMarqueeEnd.x), std::min(mMarqueeStart.y, mMarqueeEnd.y));
			ImVec2 maxR(std::max(mMarqueeStart.x, mMarqueeEnd.x), std::max(mMarqueeStart.y, mMarqueeEnd.y));

			drawList->AddRectFilled(minR, maxR, IM_COL32(200, 200, 255, 50));
			drawList->AddRect(minR, maxR, IM_COL32(200, 200, 255, 200));
		}

		// e. playhead
		if (transport) {
			double currentBeat = (double)transport->GetPosition() / transport->GetSampleRate() * (transport->GetBpm() / 60.0);
			double relBeat = currentBeat - mIDIClip->GetStartBeat();
			if (relBeat >= 0) {
				float phX = canvasPos.x + KEY_WIDTH + (float)(relBeat * PIXELS_PER_BEAT_PR);
				drawList->AddLine(ImVec2(phX, canvasPos.y), ImVec2(phX, canvasPos.y + contentHeight), IM_COL32(255, 50, 50, 200), 2.0f);
			}
		}

		ImGui::EndChild();
		ImGui::End();
	}
}
