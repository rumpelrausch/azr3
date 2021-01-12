#include "parameterConversions.h"

void parameterConversions::convert_bendrange(float value, char* string)
{
	sprintf(string, " %2d ", int(value * 12));
}

void parameterConversions::convert_tone(float value, char* string)
{
	sprintf(string, "%4.0f", 300 + 3500 * value);
}

void parameterConversions::convert_speed(float value, char* string)
{
	sprintf(string, "  %2.1f", 10 * value);
	if (!strcmp(string, "  10.0"))
		sprintf(string, " 10.0");
}

void parameterConversions::convert_perc(float value, char* string)
{

	int	v;
	v = (int)(value * 10);
	if (v < 1)
		sprintf(string, "  0");
	else if (v < 2)
		sprintf(string, " 16");
	else if (v < 3)
		sprintf(string, "  8");
	else if (v < 4)
		sprintf(string, " 5.33");
	else if (v < 5)
		sprintf(string, "  4");
	else if (v < 6)
		sprintf(string, " 2.66");
	else if (v < 7)
		sprintf(string, "  2");
	else if (v < 8)
		sprintf(string, " 1.6");
	else if (v < 9)
		sprintf(string, " 1.36");
	else
		sprintf(string, "  1");
}

void parameterConversions::convert_shape(float value, char* string)
{
	sprintf(string, "  %d ", int(value * (NUM_OF_TONEWHEEL_SHAPES - 1)) + 1);
}
