#include "fxDelay.h"

fxDelay::fxDelay(int buflen, bool interpolate)
	: alpha(0), alpha2(0), alpha3(0), offset(0), outPointer(0),
	writep(p_buflen / 2), samplerate(44100), p_buflen(buflen), interp(interpolate),
	readp(0)
{
	float x = 0;
	int	y;
	buffer = new float[p_buflen];
	for (y = 0; y < p_buflen; y++)
		buffer[y] = 0;
};

void fxDelay::set_delay(float dtime)
{
	offset = dtime * samplerate * .001f;

	if (offset < 0.1f)
		offset = 0.1f;
	else if (offset >= p_buflen)
		offset = (float)p_buflen - 1;

	outPointer = writep - offset;
	if (outPointer < 0)
		outPointer += p_buflen;

	readp = (int)outPointer;
	alpha = outPointer - readp;
	alpha2 = alpha * alpha;
	alpha3 = alpha2 * alpha;
};

void fxDelay::set_samplerate(float sr)
{
	set_delay(1000 * offset / samplerate);
	samplerate = sr;
}

void fxDelay::flood(float value)
{
	int x;
	for (x = 0; x < p_buflen; x++)
		buffer[x] = value;
}

fxDelay::~fxDelay()
{
	delete[] buffer;
}

float fxDelay::clock(float input)
{
	float	output = 0, y0, y1, y2, ym1;
	int		ym1p, y1p, y2p;
	if (p_buflen > 4410)
	{
		return(0);
	}

	buffer[writep] = input;

	if (interp)
	{
		ym1p = readp - 1;
		if (ym1p < 0)
			ym1p += p_buflen;
		y1p = readp + 1;
		if (y1p >= p_buflen)
			y1p -= p_buflen;
		y2p = readp + 2;
		if (y2p >= p_buflen)
			y2p -= p_buflen;


		ym1 = buffer[ym1p];
		y0 = buffer[readp];
		y1 = buffer[y1p];
		y2 = buffer[y2p];

		output = (alpha3 * (y0 - y1 + y2 - ym1) +
			alpha2 * (-2 * y0 + y1 - y2 + 2 * ym1) +
			alpha * (y1 - ym1) + y0);
	}
	else
		output = buffer[readp];

	writep++;
	if (writep >= p_buflen)
		writep -= p_buflen;
	readp++;
	if (readp >= p_buflen)
		readp -= p_buflen;

	return(output);
};
