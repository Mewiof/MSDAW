#include "PrecompHeader.h"
#include "Parameter.h"

Parameter* Parameter::sAutomationRequestParameter = nullptr;

bool Parameter::HandleCommonInteractions() {
	bool changed = false;

	// double-click to reset
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		ResetToDefault();
		changed = true;
	}

	// right-click for context menu
	if (ImGui::BeginPopupContextItem((name + "_context").c_str())) {
		if (ImGui::MenuItem("Reset to Default")) {
			ResetToDefault();
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
