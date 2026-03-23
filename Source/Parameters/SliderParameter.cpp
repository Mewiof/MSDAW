#include "PrecompHeader.h"
#include "SliderParameter.h"

bool SliderParameter::Draw() {
	ImGui::PushID(this);
	ImGui::Text("%s", name.c_str());
	ImGui::SetNextItemWidth(-1);
	bool changed = ImGui::SliderFloat("##Slider", &value, minValue, maxValue, "%.2f");
	changed |= HandleCommonInteractions();
	ImGui::PopID();
	return changed;
}
