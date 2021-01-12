#pragma once
#include "../parameterConversions.h"

class customControl
{
public:
	customControl();
	virtual void setStringConvert(void (*convert) (float value, char* string));
	void (*stringConvert)(float value, char* string);
	char caption[16];
};
