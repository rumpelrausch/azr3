#pragma once
#include "fxGlobals.h"
#include "fxGenericFilter.h"

class fxGenericLowpass : public fxGenericFilter
{
public:
	fxGenericLowpass();
	float	clock(float input);
};
