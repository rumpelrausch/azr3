#include "fxLFO.h"

fxLFO::fxLFO()
{
	output = 0;
	inc = 0;
	dir = 1;
	c = 1;
	s = 0;
}

void fxLFO::set_samplerate(float sr)
{
	samplerate = sr;
}

void fxLFO::set_rate(float srate, int type)
{
	my_srate = srate;
	my_type = type;
	if (type == 0)
		inc = 2.0f * PI * srate / samplerate;
	else
		inc = 2 * srate / samplerate;
	ci = cosf(inc);
	si = sinf(inc);
}

float fxLFO::get_rate()
{
	return(my_srate);
}

void fxLFO::set_phase(float phase)
{
	if (phase >= 0 && phase <= 1)
	{
		output = phase;
		s = phase;
	}
}

float fxLFO::clock()
{
	if (my_type == 1)			// triangle wave
	{
		if (dir == 1)
			output += inc;
		else
			output -= inc;

		if (output >= 1)
		{
			dir = -1;
			output = 1;
		}
		else if (output <= 0)
		{
			dir = 1;
			output = 0;
		}
	}
	else if (my_type == 0)	// sine wave
	{
		nc = DENORMALIZE(c * ci - s * si);
		ns = DENORMALIZE(c * si + s * ci);
		c = nc;
		s = ns;
		output = DENORMALIZE((s + 1) * .5f);
	}

	return(output);
}
