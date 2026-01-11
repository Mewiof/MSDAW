#include "PrecompHeader.h"
#include "ClipView.h"
#include "Clips/AudioClip.h"
#include "Clips/MIDIClip.h"
#include <string>
#include <algorithm>

void ClipView::Render(const ImVec2& pos, float width, float height) {
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Clip View", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	auto clip = mContext.state.selectedClip;

	if (!clip) {
		float txtW = ImGui::CalcTextSize("No Clip Selected").x;
		ImGui::SetCursorPos(ImVec2(width * 0.5f - txtW * 0.5f, height * 0.5f - 10.0f));
		ImGui::TextDisabled("No Clip Selected");
		ImGui::End();
		return;
	}

	// basic info header
	ImGui::Text("Clip: %s", clip->GetName().c_str());
	ImGui::SameLine();
	ImGui::TextDisabled("(%.2f beats)", clip->GetDuration());
	ImGui::Separator();

	if (auto ac = std::dynamic_pointer_cast<AudioClip>(clip)) {
		// audio clip controls
		ImGui::Columns(3, "AudioClipCols", false);

		// column 1: warping
		{
			ImGui::Text("Warping");
			ImGui::Dummy(ImVec2(0, 5));

			bool warping = ac->IsWarpingEnabled();
			if (ImGui::Checkbox("Warp", &warping)) {
				ac->SetWarpingEnabled(warping);
			}

			if (warping) {
				const char* warpModes[] = {"Beats", "Tones", "Texture", "Re-Pitch", "Complex", "Complex Pro"};
				int currentMode = (int)ac->GetWarpMode();
				ImGui::SetNextItemWidth(100);
				if (ImGui::Combo("Mode", &currentMode, warpModes, IM_ARRAYSIZE(warpModes))) {
					if (currentMode >= 0 && currentMode <= 5)
						ac->SetWarpMode((WarpMode)currentMode);
				}

				double bpm = ac->GetSegmentBpm();
				float fBpm = (float)bpm;
				ImGui::SetNextItemWidth(100);
				if (ImGui::DragFloat("Seg. BPM", &fBpm, 0.1f, 20.0f, 999.0f, "%.2f")) {
					ac->SetSegmentBpm((double)fBpm);
				}
			} else {
				ImGui::TextDisabled("Warping Disabled");
				ImGui::TextDisabled("Audio plays at native speed");
			}
		}
		ImGui::NextColumn();

		// column 2: pitch
		{
			ImGui::Text("Pitch / Transpose");
			ImGui::Dummy(ImVec2(0, 5));

			double semi = ac->GetTransposeSemitones();
			float fSemi = (float)semi;
			ImGui::SetNextItemWidth(100);
			if (ImGui::DragFloat("Semitones", &fSemi, 0.1f, -48.0f, 48.0f, "%.0f st")) {
				ac->SetTransposeSemitones((double)fSemi);
			}

			double cents = ac->GetTransposeCents();
			float fCents = (float)cents;
			ImGui::SetNextItemWidth(100);
			if (ImGui::DragFloat("Detune", &fCents, 0.1f, -100.0f, 100.0f, "%.0f ct")) {
				ac->SetTransposeCents((double)fCents);
			}

			// add a helper reset button
			if (ImGui::Button("Reset Pitch")) {
				ac->SetTransposeSemitones(0.0);
				ac->SetTransposeCents(0.0);
			}
		}
		ImGui::NextColumn();

		// column 3: file info
		{
			ImGui::TextDisabled("File Info");
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::Text("Sample Rate: %.0f Hz", ac->GetSampleRate());
			ImGui::Text("Channels: %d", ac->GetNumChannels());
			double durSec = 0.0;
			if (ac->GetSampleRate() > 0)
				durSec = (double)ac->GetTotalFileFrames() / ac->GetSampleRate();
			ImGui::Text("Length: %.2f sec", durSec);
			ImGui::Text("Frames: %llu", ac->GetTotalFileFrames());
		}

		ImGui::Columns(1);
	} else if (auto mc = std::dynamic_pointer_cast<MIDIClip>(clip)) {
		// MIDI clip controls
		ImGui::Text("MIDI Properties");
		ImGui::Separator();
		ImGui::Text("Notes: %zu", mc->GetNotes().size());
		// placeholder for MIDI specific tools
		ImGui::TextDisabled("Use Piano Roll to edit notes.");
	}

	ImGui::End();
}
