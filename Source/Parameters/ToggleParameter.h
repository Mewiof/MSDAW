#pragma once

#include "Parameter.h"

class ToggleParameter : public Parameter {
public:
	ToggleParameter(const std::string& name, float value, float minValue, float maxValue)
		: Parameter(name, value, minValue, maxValue) {}

	bool Draw() override;
};
