#pragma once
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Globals.h"

class parameterConversions
{

public:
	static void parameterConversions::convert_bendrange(float value, char* string);
	static void parameterConversions::convert_tone(float value, char* string);
	static void parameterConversions::convert_speed(float value, char* string);
	static void parameterConversions::convert_perc(float value, char* string);
	static void parameterConversions::convert_shape(float value, char* string);
};
