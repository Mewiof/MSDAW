#include "PrecompHeader.h"
#include "Widgets.h"

float LinearToLog(float t, float min, float max) {
	if (min <= 0.0f || max <= 0.0f)
		return min + t * (max - min); // fallback if negative
	return min * powf(max / min, t);
}
float LogToLinear(float value, float min, float max) {
	if (min <= 0.0f || max <= 0.0f)
		return (value - min) / (max - min);
	return logf(value / min) / logf(max / min);
}
void FormatKnobValue(char* buffer, size_t bufferSize, float value, ImGuiKnobVariant variant) {
	switch (variant) {
	case ImGuiKnobVariant_Percent:
		snprintf(buffer, bufferSize, "%.0f%%", value); // "50%"
		break;
	case ImGuiKnobVariant_Hertz:
		if (value >= 1000.0f)
			snprintf(buffer, bufferSize, "%.2f kHz", value / 1000.0f); // "2.55 kHz"
		else
			snprintf(buffer, bufferSize, "%.1f Hz", value); // "400.5 Hz"
		break;
	case ImGuiKnobVariant_Decibel:
		if (value > -70.0f)
			snprintf(buffer, bufferSize, "%+.1f dB", value);
		else
			snprintf(buffer, bufferSize, "-inf dB");
		break;
	case ImGuiKnobVariant_Linear:
	default:
		snprintf(buffer, bufferSize, "%.2f", value);
		break;
	}
}
bool DrawKnob(const char* label, float* value, float min, float max, ImGuiKnobVariant variant) {
	ImGuiIO& iO = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// constants
	// 1.65 pi looks good
	const float ANGLE_MIN = 3.14159265359f * 0.675f;
	const float ANGLE_MAX = 3.14159265359f * 2.325f;
	const float radius = 18.0f;
	const float lineHeight = ImGui::GetTextLineHeight();

	// layout calculation
	// we want: label (param name) -> knob -> label (value)
	ImVec2 pos = ImGui::GetCursorScreenPos();

	// the center of the knob circle moves down to make room for the top label
	float knobCenterY = pos.y + lineHeight + style.ItemInnerSpacing.y + radius;
	ImVec2 center = ImVec2(pos.x + radius, knobCenterY);

	// total height = label + spacing + (radius * 2) + spacing + value
	float totalHeight = (lineHeight * 2) + (style.ItemInnerSpacing.y * 2) + (radius * 2);
	float totalWidth = radius * 2;

	// 1. calculate normalized value (t)
	float t = 0.0f;
	if (variant == ImGuiKnobVariant_Hertz) {
		t = LogToLinear(*value, min, max);
	} else {
		t = (*value - min) / (max - min);
	}

	// 2. interaction area
	// make the button cover the whole height (label + knob + value) for easier grabbing
	ImGui::PushID(label);
	ImGui::InvisibleButton(label, ImVec2(totalWidth, totalHeight));

	bool valueChanged = false;
	bool isActive = ImGui::IsItemActive();
	bool isHovered = ImGui::IsItemHovered();

	// interaction logic
	if (isActive && iO.MouseDelta.y != 0.0f) {
		float mouseSensitivity = 0.005f;
		if (iO.KeyShift)
			mouseSensitivity /= 10.0f;

		t -= iO.MouseDelta.y * mouseSensitivity;
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;

		if (variant == ImGuiKnobVariant_Hertz) {
			*value = LinearToLog(t, min, max);
		} else {
			*value = min + t * (max - min);
		}
		valueChanged = true;
	}

	// double click reset
	if (isHovered && ImGui::IsMouseDoubleClicked(0)) {
		if (variant == ImGuiKnobVariant_Hertz)
			*value = LinearToLog(0.5f, min, max);
		else
			*value = (min + max) * 0.5f;
		valueChanged = true;
	}

	// 3. visualization
	float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;

	// colors
	ImU32 colBackgroud = ImGui::GetColorU32(ImGuiCol_FrameBg);
	ImU32 colText = ImGui::GetColorU32(ImGuiCol_Text);

	// determine arc color based on interaction
	ImVec4 arcColorVec = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
	if (isActive || isHovered) {
		// brighten the color significantly when hovered/active
		arcColorVec.x = std::min(arcColorVec.x * 1.3f, 1.0f);
		arcColorVec.y = std::min(arcColorVec.y * 1.3f, 1.0f);
		arcColorVec.z = std::min(arcColorVec.z * 1.3f, 1.0f);
		arcColorVec.w = 1.0f; // ensure full opacity
	}
	ImU32 colArc = ImGui::ColorConvertFloat4ToU32(arcColorVec);

	// draw background arc
	drawList->PathArcTo(center, radius * 0.85f, ANGLE_MIN, ANGLE_MAX, 32);
	drawList->PathStroke(colBackgroud, 0, 3.0f);

	// draw active arc
	if (t > 0.001f) {
		drawList->PathArcTo(center, radius * 0.85f, ANGLE_MIN, angle, 32);
		drawList->PathStroke(colArc, 0, 3.0f);
	}

	// draw tick (gap/indicator)
	ImVec2 tickVector = ImVec2(cosf(angle), sinf(angle));
	float arcRadius = radius * 0.85f;
	float tickLen = arcRadius + 1.5f;
	drawList->AddLine(
		ImVec2(center.x + tickVector.x, center.y + tickVector.y),
		ImVec2(center.x + tickVector.x * tickLen, center.y + tickVector.y * tickLen),
		colBackgroud, 3.0f);

	// 4. labels

	// top label: parameter name
	// calculate size to center it horizontally
	ImVec2 labelSize = ImGui::CalcTextSize(label);
	ImVec2 labelPos = ImVec2(
		pos.x + (totalWidth - labelSize.x) * 0.5f,
		pos.y);
	drawList->AddText(labelPos, colText, label);

	// bottom label: value
	// format
	char buffer[64];
	FormatKnobValue(buffer, sizeof(buffer), *value, variant);

	ImVec2 valSize = ImGui::CalcTextSize(buffer);
	ImVec2 valPos = ImVec2(
		pos.x + (totalWidth - valSize.x) * 0.5f,
		pos.y + lineHeight + style.ItemInnerSpacing.y + (radius * 2) + style.ItemInnerSpacing.y);

	// use the bright color for the text if interacting, otherwise standard text color
	ImU32 valTextCol = (isActive || isHovered) ? colArc : colText;
	drawList->AddText(valPos, valTextCol, buffer);

	ImGui::PopID();
	return valueChanged;
}
