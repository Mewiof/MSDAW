#include "PrecompHeader.h"
#include "TransportView.h"
#include "Project.h"
#include "Transport.h"

void TransportView::Render(const ImVec2& pos, float width, float height) {
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Transport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	Project* project = mContext.GetProject();
	Transport* transport = project ? &project->GetTransport() : nullptr;

	if (transport) {
		bool isPlaying = transport->IsPlaying();
		if (ImGui::Button(isPlaying ? " || " : " > ", ImVec2(40 * mContext.state.mainScale, 0))) {
			if (isPlaying) {
				// pause logic (ableton style): stop and return to insert marker
				transport->Pause();
				double startBeat = mContext.state.selectionStart;
				int64_t startSample = (int64_t)(startBeat * (60.0 / transport->GetBpm()) * transport->GetSampleRate());
				transport->SetPosition(startSample);
			} else {
				// play logic (ableton style): play from insert marker
				double startBeat = mContext.state.selectionStart;
				int64_t startSample = (int64_t)(startBeat * (60.0 / transport->GetBpm()) * transport->GetSampleRate());
				transport->SetPosition(startSample);
				transport->Play();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", ImVec2(40 * mContext.state.mainScale, 0)))
			transport->Stop(); // resets to 0
		ImGui::SameLine();

		// loop toggle
		bool isLooping = transport->IsLoopEnabled();
		if (isLooping) {
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 100, 0, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(220, 120, 0, 255));
		}
		if (ImGui::Button("Loop", ImVec2(40 * mContext.state.mainScale, 0))) {
			transport->SetLoopEnabled(!isLooping);
		}
		if (isLooping)
			ImGui::PopStyleColor(2);

		ImGui::SameLine();

		// follow toggle
		bool follow = mContext.state.followPlayback;
		if (follow) {
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 100, 0, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(220, 120, 0, 255));
		}
		if (ImGui::Button("Follow", ImVec2(50 * mContext.state.mainScale, 0))) {
			mContext.state.followPlayback = !follow;
		}
		if (follow)
			ImGui::PopStyleColor(2);

		ImGui::SameLine();
		ImGui::Text("| Time: %.2f", (double)transport->GetPosition() / transport->GetSampleRate());
		ImGui::SameLine();
		float bpm = (float)transport->GetBpm();
		ImGui::SetNextItemWidth(80 * mContext.state.mainScale);
		if (ImGui::DragFloat("BPM", &bpm, 1.0f, 40.0f, 300.0f, "%.1f"))
			if (mContext.GetProject()) {
				mContext.GetProject()->SetBpm(bpm);
			}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80 * mContext.state.mainScale);
		float gridSize = (float)mContext.state.timelineGrid;
		if (ImGui::DragFloat("Grid", &gridSize, 0.25f, 0.125f, 16.0f, "%.3f")) {
			mContext.state.timelineGrid = gridSize;
		}

		// computer MIDI keyboard toggle
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20, 0)); // spacer
		ImGui::SameLine();

		bool mIDIKey = mContext.state.isComputerMIDIKeyboardEnabled;
		if (mIDIKey) {
			// yellow active color like ableton
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 200, 0, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 220, 50, 255));
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
		}

		if (ImGui::Button("KEY", ImVec2(40 * mContext.state.mainScale, 0))) {
			mContext.state.isComputerMIDIKeyboardEnabled = !mIDIKey;
		}

		if (mIDIKey) {
			ImGui::PopStyleColor(3);
		}

		// optional: show current octave/vel on hover or beside it
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Computer MIDI Keyboard (M)\nOctave: %d (Z/X)\nVelocity: %d (C/V)",
							  mContext.state.mIDIOctave, mContext.state.mIDIVelocity);
		}
	}
	ImGui::SameLine(ImGui::GetWindowWidth() - 150 * mContext.state.mainScale);
	ImGui::TextColored(mContext.engine.IsStreamRunning() ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
					   mContext.engine.IsStreamRunning() ? "Audio: ON" : "Audio: OFF");
	ImGui::End();
}
