#pragma once
#include "EditorContext.h"
#include "TimelineDefs.h"
#include "imgui.h"

class TimelineTrackView {
public:
	static void RenderTracks(EditorContext& context, TimelineInteractionState& interaction,
							 PendingClipMove& pendingMove, PendingClipDelete& pendingDelete,
							 const ImVec2& winPos, float contentWidth, float viewWidth, float scrollX, float startY);
};
