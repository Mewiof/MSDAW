#include "PrecompHeader.h"
#include "ToggleParameter.h"

bool ToggleParameter::Draw() {
	ImGui::PushID(this);
	bool bVal = value > 0.5f;
	bool changed = ImGui::Checkbox(name.c_str(), &bVal);
	if (changed) {
		float oldValue = value;
		value = bVal ? 1 : 0;
		CommitEditImmediate(oldValue);
	}
	changed |= HandleCommonInteractions();
	ImGui::PopID();
	return changed;
}
