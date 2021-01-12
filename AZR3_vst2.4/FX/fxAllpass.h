#pragma once
#include "fxGlobals.h"

class	fxAllpass
{
public:
	fxAllpass();
	void	reset();
	void	set_delay(float fxDelay);
	float	clock(float input);
private:
	float	a1, zm1, my_delay, y;
};
