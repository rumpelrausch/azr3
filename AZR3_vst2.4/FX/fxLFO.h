#pragma once
#include "fxGlobals.h"

class fxLFO
{
public:
	fxLFO();
	~fxLFO() {};
	float clock();
	void	set_samplerate(float samplerate);
	void	set_rate(float srate, int type);	// Hz; type: 0=sin, 1=tri
	void	set_phase(float phase);
	float	get_rate();
private:
	int		my_type;
	float	output;
	float	samplerate;
	float	inc;
	int		dir;
	float	c, s, ci, si, nc, ns;
	float	my_srate;
};
