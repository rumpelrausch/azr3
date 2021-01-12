#include "customControl.h"

customControl::customControl()
	: stringConvert(NULL), caption("               ")
{
}

void customControl::setStringConvert(void (*convert) (float value, char* string))
{
	stringConvert = convert;
}
