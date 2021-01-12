#include "fxGenericLowpass.h"

fxGenericLowpass::fxGenericLowpass() : fxGenericFilter()
{
}

float fxGenericLowpass::clock(float input)
{
	float	in;
	in = DENORMALIZE(input);
	m_l = DENORMALIZE(m_l);
	m_b = DENORMALIZE(m_b);

	m_h = in - m_l - q * m_b;
	m_b += m_f * m_h;
	m_l += m_f * m_b;

	return(m_l);
}
