#pragma once
#include "imgui.h"
#include <vector>

namespace TimelineUtils {
	// renders audio waveform into the given drawList within rectMin/rectMax
	// sourceFramesPerPixel: defines density
	// offsetFrames: how many source frames into the file to start drawing (supports clip offset)
	void RenderWaveform(ImDrawList* drawList,
						const std::vector<float>& samples,
						int channels,
						double sourceFramesPerPixel,
						double offsetFrames,
						const ImVec2& rectMin,
						const ImVec2& rectMax,
						ImU32 color,
						bool forceMono = false);
} //namespace TimelineUtils
