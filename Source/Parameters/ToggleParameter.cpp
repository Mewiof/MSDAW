#include "PrecompHeader.h"
#include "ToggleParameter.h"

bool ToggleParameter::Draw() {
	ImGui::PushID(this);
	bool bVal = value > 0.5f;
	bool changed = ImGui::Checkbox(name.c_str(), &bVal);
	if (changed)
		value = bVal ? 1 : 0;
	changed |= HandleCommonInteractions();
	ImGui::PopID();
	return changed;
}
