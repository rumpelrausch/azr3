#include "fxAllpass.h"

fxAllpass::fxAllpass() : a1(0.f), zm1(0.f)
{
	a1 = zm1 = my_delay = y = 0;
}

void	fxAllpass::reset()
{
	a1 = zm1 = y = 0;
	set_delay(my_delay);
}

void	fxAllpass::set_delay(float fxDelay)
{
	my_delay = fxDelay;
	a1 = (1 - fxDelay) / (1 + fxDelay);
	a1 = DENORMALIZE(a1);
}

float	fxAllpass::clock(float input)
{
	if (input<.00000001f && input>-.00000001f)	// prevent Pentium FPU Normalizing
		return(0);

	y = -a1 * input + zm1;
	zm1 = y * a1 + input;
	return(y);
}
