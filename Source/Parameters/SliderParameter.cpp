#include "PrecompHeader.h"
#include "SliderParameter.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>

bool SliderParameter::Draw() {
	bool changed = false;
	ImGui::PushID(this);
	ImGui::Text("%s", name.c_str());

	ImGui::SetNextItemWidth(-1);

	// typing interception
	static ImGuiID s_TypingSliderID = 0;
	static char s_TextBuffer[64] = "";
	static bool s_FocusNextFrame = false;

	ImGuiID currentID = ImGui::GetID("##SliderBtn");

	// intercept
	if (IsSelected() && s_TypingSliderID == 0) {
		bool startTyping = false;
		char initialChar = '\0';

		if (!ImGui::GetIO().WantTextInput) {
			for (int k = ImGuiKey_0; k <= ImGuiKey_9; k++) {
				if (ImGui::IsKeyPressed((ImGuiKey)k)) {
					startTyping = true;
					initialChar = '0' + (k - ImGuiKey_0);
					break;
				}
			}
			if (!startTyping) {
				for (int k = ImGuiKey_Keypad0; k <= ImGuiKey_Keypad9; k++) {
					if (ImGui::IsKeyPressed((ImGuiKey)k)) {
						startTyping = true;
						initialChar = '0' + (k - ImGuiKey_Keypad0);
						break;
					}
				}
			}
			if (!startTyping && (ImGui::IsKeyPressed(ImGuiKey_Minus) || ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract))) {
				startTyping = true;
				initialChar = '-';
			}
			if (!startTyping && (ImGui::IsKeyPressed(ImGuiKey_Period) || ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal))) {
				startTyping = true;
				initialChar = '.';
			}
		}

		if (startTyping) {
			s_TypingSliderID = currentID;
			s_TextBuffer[0] = initialChar;
			s_TextBuffer[1] = '\0';
			s_FocusNextFrame = true;
		}
	}

	if (s_TypingSliderID == currentID) {
		ImGui::PushItemWidth(-1);

		if (s_FocusNextFrame) {
			ImGui::SetKeyboardFocusHere();
			s_FocusNextFrame = false;
		}

		bool enterPressed = ImGui::InputText("##TypeInput", s_TextBuffer, sizeof(s_TextBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		float minV = std::min(minValue, maxValue);
		float maxV = std::max(minValue, maxValue);

		if (enterPressed) {
			value = std::clamp((float)atof(s_TextBuffer), minV, maxV);
			changed = true;
			s_TypingSliderID = 0;
		} else if (ImGui::IsItemDeactivated()) {
			if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
				s_TypingSliderID = 0;
			} else {
				value = std::clamp((float)atof(s_TextBuffer), minV, maxV);
				changed = true;
				s_TypingSliderID = 0;
			}
		}

		ImGui::PopItemWidth();
		changed |= HandleCommonInteractions();
	} else {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
		if (size.x <= 0)
			size.x = ImGui::GetContentRegionAvail().x;

		ImGui::InvisibleButton("##SliderBtn", size);
		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
			Select();

		if (active) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			float deltaY = ImGui::GetIO().MouseDelta.y;
			if (deltaY != 0.0f) {
				float range = maxValue - minValue;
				float sensitivity = range / 200.0f;
				if (ImGui::GetIO().KeyShift)
					sensitivity *= 0.1f;

				float minV = std::min(minValue, maxValue);
				float maxV = std::max(minValue, maxValue);
				value = std::clamp(value - (deltaY * sensitivity), minV, maxV);
				changed = true;
			}
		}

		if (ImGui::IsItemDeactivated()) {
			ImGui::GetIO().WantSetMousePos = true;
			ImGui::GetIO().MousePos = ImGui::GetIO().MouseClickedPos[0];
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImU32 bgColor = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		ImU32 fillColor = ImGui::GetColorU32(active ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);

		drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor, ImGui::GetStyle().FrameRounding);

		float fraction = (value - minValue) / (maxValue - minValue);
		ImVec2 fillMax = ImVec2(pos.x + fraction * size.x, pos.y + size.y);
		drawList->AddRectFilled(pos, fillMax, fillColor, ImGui::GetStyle().FrameRounding);

		if (IsSelected()) {
			drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 255), ImGui::GetStyle().FrameRounding);
		}

		char valText[32];
		snprintf(valText, sizeof(valText), "%.2f", value);
		ImVec2 textSize = ImGui::CalcTextSize(valText);
		ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
		drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), valText);

		changed |= HandleCommonInteractions();
	}

	ImGui::PopID();
	return changed;
}
