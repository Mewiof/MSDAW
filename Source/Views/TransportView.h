#pragma once
#include "EditorContext.h"
#include "Parameters/SliderParameter.h"
#include "imgui.h"
#include <memory>

class TransportView {
public:
	TransportView(EditorContext& context)
		: mContext(context) {
		// grid numerator/denominator as parameter widgets, seeded from the current snap state
		mGridNumParam = std::make_unique<SliderParameter>("Grid Num", (float)context.state.timelineGridNumerator, 1.0f, 64.0f);
		mGridDenParam = std::make_unique<SliderParameter>("Grid Den", (float)context.state.timelineGridDenominator, 1.0f, 128.0f);
	}
	void Render(const ImVec2& pos, float width, float height);
private:
	EditorContext& mContext;

	// editable source for the timeline snap grid; EditorState is derived from these each frame
	std::unique_ptr<SliderParameter> mGridNumParam;
	std::unique_ptr<SliderParameter> mGridDenParam;
};
