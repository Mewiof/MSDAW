#include "PrecompHeader.h"
#include "ContinuousParameter.h"

ImGuiID ContinuousParameter::s_TypingID = 0;
char ContinuousParameter::s_TextBuffer[64] = "";
bool ContinuousParameter::s_FocusNextFrame = false;
bool ContinuousParameter::s_MoveCursorToEnd = false;

void ContinuousParameter::CheckTypingStart(ImGuiID currentID) {
	if (IsSelected() && s_TypingID == 0) {
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
			s_TypingID = currentID;
			s_TextBuffer[0] = initialChar;
			s_TextBuffer[1] = '\0';
			s_FocusNextFrame = true;
			s_MoveCursorToEnd = true;
		}
	}
}

bool ContinuousParameter::IsTyping(ImGuiID currentID) const {
	return s_TypingID == currentID;
}

bool ContinuousParameter::DrawTypingInput(ImGuiID currentID, float width, float yOffset) {
	bool changed = false;

	if (yOffset != 0.0f)
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);

	if (width > 0.0f)
		ImGui::PushItemWidth(width);
	else
		ImGui::PushItemWidth(-1.0f);

	if (s_FocusNextFrame) {
		ImGui::SetKeyboardFocusHere();
		s_FocusNextFrame = false;
	}

	auto moveCursorToEndCallback = [](ImGuiInputTextCallbackData* data) -> int {
		if (s_MoveCursorToEnd) {
			data->CursorPos = data->BufTextLen;
			data->SelectionStart = data->CursorPos;
			data->SelectionEnd = data->CursorPos;
			s_MoveCursorToEnd = false;
		}
		return 0;
	};

	bool enterPressed = ImGui::InputText("##TypeInput", s_TextBuffer, sizeof(s_TextBuffer),
										 ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways,
										 moveCursorToEndCallback);

	float minV = std::min(minValue, maxValue);
	float maxV = std::max(minValue, maxValue);

	if (enterPressed) {
		value = std::clamp((float)atof(s_TextBuffer), minV, maxV);
		changed = true;
		s_TypingID = 0;
	} else if (ImGui::IsItemDeactivated()) {
		s_MoveCursorToEnd = false;
		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			s_TypingID = 0;
		} else {
			value = std::clamp((float)atof(s_TextBuffer), minV, maxV);
			changed = true;
			s_TypingID = 0;
		}
	}

	ImGui::PopItemWidth();
	changed |= HandleCommonInteractions();

	if (yOffset != 0.0f)
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset); // balance

	return changed;
}

float ContinuousParameter::GetSafeMouseDeltaY() const {
	float deltaY = ImGui::GetIO().MouseDelta.y;
	if (deltaY != 0.0f) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float teleportDistance = viewport->Size.y - 40.0f;
		float maxMovement = std::max(teleportDistance * 0.5f, 50.0f);

		// filter out massive delta spikes caused by wrapping teleports
		if (std::abs(deltaY) < maxMovement) {
			return deltaY;
		}
	}
	return 0.0f;
}

void ContinuousParameter::HandleInfiniteDrag() {
	ImVec2 mousePos = ImGui::GetIO().MousePos;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	float wrapMargin = 20.0f;
	bool wrapped = false;

	if (viewport->Size.y > wrapMargin * 3.0f) {
		if (mousePos.y <= viewport->Pos.y + wrapMargin) {
			mousePos.y = viewport->Pos.y + viewport->Size.y - wrapMargin - 1.0f;
			wrapped = true;
		} else if (mousePos.y >= viewport->Pos.y + viewport->Size.y - wrapMargin) {
			mousePos.y = viewport->Pos.y + wrapMargin + 1.0f;
			wrapped = true;
		}
	}

	if (viewport->Size.x > wrapMargin * 3.0f) {
		if (mousePos.x <= viewport->Pos.x + wrapMargin) {
			mousePos.x = viewport->Pos.x + viewport->Size.x - wrapMargin - 1.0f;
			wrapped = true;
		} else if (mousePos.x >= viewport->Pos.x + viewport->Size.x - wrapMargin) {
			mousePos.x = viewport->Pos.x + wrapMargin + 1.0f;
			wrapped = true;
		}
	}

	if (wrapped) {
		ImGui::GetIO().WantSetMousePos = true;
		ImGui::GetIO().MousePos = mousePos;
	}
}

void ContinuousParameter::RestoreMousePosition() {
	ImGui::GetIO().WantSetMousePos = true;
	ImGui::GetIO().MousePos = ImGui::GetIO().MouseClickedPos[0];
}
