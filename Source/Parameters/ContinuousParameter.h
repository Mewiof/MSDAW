#pragma once

#include "Parameter.h"

class ContinuousParameter : public Parameter {
public:
	ContinuousParameter(const std::string& name, float value, float minValue, float maxValue)
		: Parameter(name, value, minValue, maxValue) {}

	virtual ~ContinuousParameter() = default;
protected:
	// typing interception
	void CheckTypingStart(ImGuiID currentID);
	bool IsTyping(ImGuiID currentID) const;
	bool DrawTypingInput(ImGuiID currentID, float width, float yOffset = 0.0f);

	// dragging and screen-wrap
	float GetSafeMouseDeltaY() const;
	void HandleInfiniteDrag();
	void RestoreMousePosition();
private:
	static ImGuiID s_TypingID;
	static char s_TextBuffer[64];
	static bool s_FocusNextFrame;
	static bool s_MoveCursorToEnd;
};
