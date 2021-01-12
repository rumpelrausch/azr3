#include "fxGenericFilter.h"

fxGenericFilter::fxGenericFilter()
{
	m_l = m_h = m_b = m_f = q = 0;
}

void fxGenericFilter::clock(float input)
{
	float	in;
	in = DENORMALIZE(input);
	m_l = DENORMALIZE(m_l);
	m_b = DENORMALIZE(m_b);

	m_h = in - m_l - q * m_b;
	m_b += m_f * m_h;
	m_l += m_f * m_b;
}

float fxGenericFilter::lp()
{
	return(m_l);
}

float fxGenericFilter::bp()
{
	return(m_b);
}

float fxGenericFilter::hp()
{
	return(m_h);
}

void fxGenericFilter::set_samplerate(float samplerate)
{
	fs = samplerate;
	m_l = m_h = m_b = 0;
	setparam(fc, q, fs);
}

void fxGenericFilter::setparam(float cutoff, float mq, float samplerate)
{
	fc = cutoff;
	q = mq;
	fs = samplerate;
	m_f = 2.0f * sinf(PI * fc / fs);
}
