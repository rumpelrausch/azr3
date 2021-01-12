#pragma once
#include "fxGlobals.h"

class fxDelay
{
public:
	fxDelay(int buflen, bool interpolate);
	~fxDelay();
	void	set_delay(float dtime);
	void	set_samplerate(float samplerate);
	void	flood(float value);
	float	clock(float input);
protected:
	float* buffer;
	int		p_buflen;
	bool	interp;
	float	offset;
	float	samplerate;
	int		readp, writep;

	float	outPointer;
	float	alpha, alpha2, alpha3;
};
