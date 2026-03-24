#pragma once

#include "ContinuousParameter.h"

class SliderParameter : public ContinuousParameter {
public:
	SliderParameter(const std::string& name, float value, float minValue, float maxValue)
		: ContinuousParameter(name, value, minValue, maxValue) {}

	bool Draw() override;
};
