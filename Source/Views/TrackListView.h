#pragma once
#include "EditorContext.h"
#include "imgui.h"

class TrackListView {
public:
	TrackListView(EditorContext& context)
		: mContext(context) {}
	void Render(const ImVec2& pos, float width, float height);
private:
	EditorContext& mContext;

	// track rename state
	int mRenamingIndex = -1;
	char mRenameBuf[256] = {0};
};
