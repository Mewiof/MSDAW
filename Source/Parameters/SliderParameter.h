#pragma once

#include "Parameter.h"

class SliderParameter : public Parameter {
public:
	SliderParameter(const std::string& name, float value, float minValue, float maxValue)
		: Parameter(name, value, minValue, maxValue) {}

	bool Draw() override;
};
