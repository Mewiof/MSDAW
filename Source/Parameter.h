#pragma once

class Parameter {
public:
	std::string name;
	float value;
	float minValue;
	float maxValue;
	float defaultValue;

	Parameter(const std::string& name, float value, float minValue, float maxValue)
		: name(name), value(value), minValue(minValue), maxValue(maxValue), defaultValue(value) {}

	virtual ~Parameter() = default;

	// returns true if value changed
	virtual bool Draw() = 0;

	// e.g., double-click to reset or right-click for the context menu
	bool HandleCommonInteractions();

	void ResetToDefault() { value = defaultValue; }

	static Parameter* GetAndClearAutomationRequestParameter();

	bool IsSelected() const { return sSelectedParameter == this; }
	void Select() { sSelectedParameter = this; }
protected:
	static Parameter* sAutomationRequestParameter;
	static Parameter* sSelectedParameter;
};
