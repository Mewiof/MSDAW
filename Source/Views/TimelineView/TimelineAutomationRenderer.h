#pragma once
#include "EditorContext.h"
#include "TimelineDefs.h"
#include "Track.h"
#include "imgui.h"

class TimelineAutomationRenderer {
public:
	static void Render(EditorContext& context, TimelineInteractionState& interaction,
					   Track* track, int trackIndex,
					   const ImVec2& winPos, float contentWidth, float viewWidth, float scrollX, float yPos);
};
