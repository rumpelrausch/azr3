#pragma once
#include "fxGlobals.h"

class fxGenericFilter
{
	friend class fxGenericLowpass;
public:
	fxGenericFilter();
	~fxGenericFilter() {};
	float	lp();
	float	bp();
	float	hp();
	void	clock(float input);
	void	setparam(float cutoff, float q, float samplerate);
	void	set_samplerate(float samplerate);
private:
	float	fs;		// sampling freq
	float	fc;		// cutoff freq
	float	q;		// resonance
	float	m_f, m_q, m_qnrm;
	float	m_h;	// hp out
	float	m_b;	// bp out
	float	m_l;	// lp out
};
