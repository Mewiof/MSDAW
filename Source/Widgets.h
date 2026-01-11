#pragma once

#include "imgui.h"

enum ImGuiKnobVariant {
	ImGuiKnobVariant_Linear,  // 0 to 100, -10 to +10, etc.
	ImGuiKnobVariant_Percent, // 0% to 100%
	ImGuiKnobVariant_Hertz,	  // logarithmic (20Hz ... 20kHz)
	ImGuiKnobVariant_Decibel  // linear, but formatted as dB
};

float LinearToLog(float t, float min, float max);
float LogToLinear(float value, float min, float max);
void FormatKnobValue(char* buffer, size_t bufferSize, float value, ImGuiKnobVariant variant);
bool DrawKnob(const char* label, float* value, float min, float max, ImGuiKnobVariant variant = ImGuiKnobVariant_Linear);
