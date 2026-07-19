#include "PrecompHeader.h"
#include "Parameter.h"

Parameter* Parameter::sAutomationRequestParameter = nullptr;
Parameter* Parameter::sSelectedParameter = nullptr;

Parameter* Parameter::sEditingParam = nullptr;
float Parameter::sEditOldValue = 0.0f;
Parameter* Parameter::sLastTouchedParameter = nullptr;

std::function<void(Parameter*, float, float)> Parameter::sOnEditCommitted;

void Parameter::BeginEditGesture() {
	// capture the pre-gesture value once, before any delta is applied this frame
	sEditingParam = this;
	sEditOldValue = value;
}

void Parameter::EndEditGesture() {
	if (sEditingParam != this) {
		// no matching begin (e.g. click without drag): nothing committed
		return;
	}
	float oldValue = sEditOldValue;
	sEditingParam = nullptr;
	if (value != oldValue) {
		sLastTouchedParameter = this;
		if (sOnEditCommitted)
			sOnEditCommitted(this, oldValue, value);
	}
}

void Parameter::CommitEditImmediate(float oldValue) {
	// an immediate commit supersedes any in-progress gesture on this parameter
	// (e.g. double-click reset while the knob is held), so a later EndEditGesture
	// does not record a second entry
	if (sEditingParam == this)
		sEditingParam = nullptr;
	if (value == oldValue)
		return;
	sLastTouchedParameter = this;
	if (sOnEditCommitted)
		sOnEditCommitted(this, oldValue, value);
}

bool Parameter::HandleCommonInteractions() {
	bool changed = false;

	// double-click to reset
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		float oldValue = value;
		ResetToDefault();
		CommitEditImmediate(oldValue);
		changed = true;
	}

	// right-click for context menu
	if (ImGui::BeginPopupContextItem((name + "_context").c_str())) {
		if (ImGui::MenuItem("Reset to Default")) {
			float oldValue = value;
			ResetToDefault();
			CommitEditImmediate(oldValue);
			changed = true;
		}
		if (ImGui::MenuItem("Show Automation"))
			sAutomationRequestParameter = this;

		ImGui::EndPopup();
	}

	return changed;
}

Parameter* Parameter::GetAndClearAutomationRequestParameter() {
	Parameter* parameter = sAutomationRequestParameter;
	sAutomationRequestParameter = nullptr;
	return parameter;
}
