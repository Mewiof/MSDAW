#pragma once
#include "EditorContext.h"
#include "imgui.h"

class TransportView {
public:
	TransportView(EditorContext& context)
		: mContext(context) {}
	void Render(const ImVec2& pos, float width, float height);
private:
	EditorContext& mContext;
};
