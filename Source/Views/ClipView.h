#pragma once
#include "EditorContext.h"
#include "imgui.h"

class ClipView {
public:
	ClipView(EditorContext& context)
		: mContext(context) {}
	void Render(const ImVec2& pos, float width, float height);
private:
	EditorContext& mContext;
};
