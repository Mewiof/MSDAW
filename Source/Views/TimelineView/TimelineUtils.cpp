#include "PrecompHeader.h"
#include "TimelineUtils.h"
#include <cmath>
#include <algorithm>

namespace TimelineUtils {

	void RenderWaveform(ImDrawList* drawList, const std::vector<float>& samples, int channels,
						double sourceFramesPerPixel, double offsetFrames,
						const ImVec2& rectMin, const ImVec2& rectMax,
						ImU32 color, bool forceMono) {
		if (samples.empty())
			return;

		size_t totalFileFrames = samples.size() / channels;

		// viewport culling
		ImVec2 clipMin = drawList->GetClipRectMin();
		ImVec2 clipMax = drawList->GetClipRectMax();

		float startPx = std::max(rectMin.x, clipMin.x);
		float endPx = std::min(rectMax.x, clipMax.x);

		if (startPx >= endPx)
			return;

		bool drawStereo = (channels == 2 && !forceMono);

		auto DrawSegment = [&](float midY, float halfH, int channelIdx) {
			if (sourceFramesPerPixel > 1.0) {
				int scanStep = (int)std::max(1.0, sourceFramesPerPixel / 10.0);

				for (float px = startPx; px < endPx; px += 1.0f) {
					float relX = px - rectMin.x;

					// apply offsetFrames here to shift view into file
					double preciseIdxStart = (relX * sourceFramesPerPixel) + offsetFrames;
					double preciseIdxEnd = ((relX + 1.0f) * sourceFramesPerPixel) + offsetFrames;

					size_t idxStart = (size_t)preciseIdxStart;
					size_t idxEnd = (size_t)preciseIdxEnd;

					if (idxStart >= totalFileFrames)
						break;
					if (idxEnd > totalFileFrames)
						idxEnd = totalFileFrames;

					if (idxEnd <= idxStart)
						continue;

					float minVal = 0.0f, maxVal = 0.0f;
					bool firstSample = true;

					for (size_t k = idxStart; k < idxEnd; k += scanStep) {
						float v = samples[k * channels + channelIdx];
						if (firstSample) {
							minVal = maxVal = v;
							firstSample = false;
						} else {
							if (v < minVal)
								minVal = v;
							if (v > maxVal)
								maxVal = v;
						}
					}

					if (!firstSample) {
						float y1 = midY + minVal * halfH;
						float y2 = midY + maxVal * halfH;

						if (std::abs(y2 - y1) < 1.0f) {
							y1 -= 0.5f;
							y2 += 0.5f;
						}

						drawList->AddRectFilled(ImVec2(px, y1), ImVec2(px + 1.0f, y2), color);
					}
				}
			} else {
				float traceStartPx = startPx - 2.0f;
				if (traceStartPx < rectMin.x)
					traceStartPx = rectMin.x;

				ImVec2 prevPos;
				bool first = true;

				for (float px = traceStartPx; px < endPx; px += 1.0f) {
					float relX = px - rectMin.x;
					double preciseIdx = (relX * sourceFramesPerPixel) + offsetFrames;
					size_t idx = (size_t)preciseIdx;

					if (idx >= totalFileFrames)
						break;

					float val = samples[idx * channels + channelIdx];
					ImVec2 currentPos(px, midY + val * halfH);

					if (!first) {
						drawList->AddLine(prevPos, currentPos, color, 1.5f);
					} else {
						first = false;
					}
					prevPos = currentPos;
				}
			}
		};

		if (drawStereo) {
			float totalH = rectMax.y - rectMin.y;
			float channelH = totalH * 0.5f;
			float halfH = channelH * 0.45f;

			drawList->AddLine(ImVec2(rectMin.x, rectMin.y + channelH), ImVec2(rectMax.x, rectMin.y + channelH), IM_COL32(0, 0, 0, 40));

			DrawSegment(rectMin.y + channelH * 0.5f, halfH, 0); // left
			DrawSegment(rectMin.y + channelH * 1.5f, halfH, 1); // right
		} else {
			float midY = (rectMin.y + rectMax.y) * 0.5f;
			float halfH = (rectMax.y - rectMin.y) * 0.45f;
			DrawSegment(midY, halfH, 0);
		}
	}

} // namespace TimelineUtils
