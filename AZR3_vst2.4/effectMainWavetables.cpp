#include "effectMain.h"

bool effectMain::buildTonewheelSet(int shape)
{
	long	i;
	float	amp = .5f;
	float	tw = 0, twfuzz;
	float	twmix = .5f;
	float	twdist = .7f;
	float	tws = (float)WAVETABLESIZE;


	if (shape == last_shape)
		return(false);
	last_shape = shape;
	/*
	We don't just produce flat sine curves but slightly distorted
	and even a triangle wave can be choosen.

	  Though this makes things sound more interesting it's _not_ a
	  tonewheel emulation. If we wanted that we would have to calculate
	  or otherwise define different waves for every playable note. If
	  anyone wants to implement real tonewheels you will have to make
	  drastic changes:
	  - implement many wavetables and a choosing algorithm
	  - construct wavetable data either through calculations
	  or from real wave files. Tha latter is what they do at
	  n@tive 1nstrument5.
	*/
	for (i = 0; i < WAVETABLESIZE; i++)
	{
		float	ii = (float)i;

		if (shape == TONEWHEEL_SHAPE_SINE1)
		{
			tw = amp *
				(
					sinf(ii * 2 * Pi / tws)
					+ .01f * sinf(ii * 8 * Pi / tws)
					+ .003f * sinf(ii * 12 * Pi / tws)
					);
			tonewheel[i] = tw;
		}
		else if (shape == TONEWHEEL_SHAPE_SINE2 || shape == TONEWHEEL_SHAPE_SINE3)
		{
			tw = amp *
				(
					sinf(ii * 2 * Pi / tws)
					+ .03f * sinf(ii * 8 * Pi / tws)
					+ .01f * sinf(ii * 12 * Pi / tws)
					);

			if (shape == TONEWHEEL_SHAPE_SINE2)
				twdist = 1;
			else if (shape == TONEWHEEL_SHAPE_SINE3)
				twdist = 2;

			tw *= twdist;
			twfuzz = 2 * tw - tw * tw * tw;
			if (twfuzz > 1)
				twfuzz = 1;
			else if (twfuzz < -1)
				twfuzz = -1;
			tonewheel[i] = .5f * twfuzz / twdist;
		}
		else if (shape == TONEWHEEL_SHAPE_TRI)
		{
			if (i<int(tws / 4) || i>int(tws * .75f))
				tw += 2 / tws;
			else
				tw -= 2 / tws;
			tonewheel[i] = tw;
		}
		else if (shape == TONEWHEEL_SHAPE_SAW)
		{
			tw = sinf(ii * Pi / tws);
			if (i > int(tws / 2))
			{
				tw = sinf((ii - tws / 2) * Pi / tws);
				tw = 1 - tw;
			}

			tonewheel[i] = tw - .5f;
		}
		else
		{
			// pure sine
			tw = amp *
				(
					sinf(ii * 2 * Pi / tws)
					//+.01f*sinf( ii*8*Pi/tws )
					//+.003f*sinf( ii*12*Pi/tws )
					);
			tonewheel[i] = tw;
		}
	}

	for (i = 0; i < WAVETABLESIZE; i++)
	{
		//		int	f=TONEWHEELSIZE/WAVETABLESIZE;
		int f = 1;
		int	icount;
		int i2[9];

		i2[0] = (int)(i * 1 * f);
		i2[1] = (int)(i * 2 * f);
		i2[2] = (int)(i * 3 * f);
		i2[3] = (int)(i * 4 * f);
		i2[4] = (int)(i * 6 * f);
		i2[5] = (int)(i * 8 * f);
		i2[6] = (int)(i * 10 * f);
		i2[7] = (int)(i * 12 * f);
		i2[8] = (int)(i * 16 * f);

		for (icount = 0; icount < 9; icount++)
		{
			while (i2[icount] >= WAVETABLESIZE)
				i2[icount] -= WAVETABLESIZE;
		}

		sin_16[i] = tonewheel[i2[0]];
		sin_8[i] = tonewheel[i2[1]];
		sin_513[i] = tonewheel[i2[2]];
		sin_4[i] = tonewheel[i2[3]];
		sin_223[i] = tonewheel[i2[4]];
		sin_2[i] = tonewheel[i2[5]];
		sin_135[i] = tonewheel[i2[6]];
		sin_113[i] = tonewheel[i2[7]];
		sin_1[i] = tonewheel[i2[8]];
	}

	return(true);
}

/**
* Build one of the three waveform sets with four complete waves
* per set. "number" is 1..3 and references the waveform set
*/
void effectMain::rebuildWavetableSet(int number)
{
	int i, c;
	float* t;
	float	this_p[kNumParams];
	for (c = 0; c < kNumParams; c++)
		this_p[c] = my_p[c];
	if (number == 2)
	{
		c = n_2_db1;
		t = &wavetable[WAVETABLESIZE * TABLES_PER_CHANNEL];
	}
	else if (number == 3)
	{
		t = &wavetable[WAVETABLESIZE * TABLES_PER_CHANNEL * 2];
		c = n_3_db1;
	}
	else
	{
		t = &wavetable[0];
		c = n_1_db1;
	}

	//	weight to each drawbar
	this_p[c] *= 1.5f;
	this_p[c + 1] *= 1.0f;
	this_p[c + 2] *= .8f;
	this_p[c + 3] *= .8f;
	this_p[c + 4] *= .8f;
	this_p[c + 5] *= .8f;
	this_p[c + 6] *= .8f;
	this_p[c + 7] *= .6f;
	this_p[c + 8] *= .6f;

	for (i = 0; i < WAVETABLESIZE; i++)
	{
		t[i] = t[i + WAVETABLESIZE] = t[i + WAVETABLESIZE * 2] = t[i + WAVETABLESIZE * 3] =
			t[i + WAVETABLESIZE * 4] =
			t[i + WAVETABLESIZE * 5] =
			t[i + WAVETABLESIZE * 6] =
			t[i + WAVETABLESIZE * 7] =
			sin_16[i] * this_p[c] + sin_8[i] * this_p[c + 1]
			+ sin_513[i] * this_p[c + 2];

		/*
			This is very important for a warm sound:
			The "tone wheels" are a limited resource and they
			supply limited pitch heights. If a drawbar register
			is forced to play a tune above the highest possible
			note it will simply be transposed one octave down.
			In addition it will appear less loud; that's what
			d2, d4 and d8 are for.
		*/
#define d2 .5f
#define d4 .25f
#define d8 .125f
		if (number == 3)
		{
			t[i] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4];
			t[i + WAVETABLESIZE] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4];
			t[i + WAVETABLESIZE * 2] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4];
			t[i + WAVETABLESIZE * 3] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4];
			t[i + WAVETABLESIZE * 4] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4];
			t[i + WAVETABLESIZE * 5] += sin_4[i] * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4];
			t[i + WAVETABLESIZE * 6] += sin_4[i] * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4];
			t[i + WAVETABLESIZE * 7] += sin_4[int(i / 2)] * d2 * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4];
		}
		else
		{
			t[i] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4]
				+ sin_2[i] * this_p[c + 5]
				+ sin_135[i] * this_p[c + 6] + sin_113[i] * this_p[c + 7]
				+ sin_1[i] * this_p[c + 8];
			t[i + WAVETABLESIZE] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4]
				+ sin_2[i] * this_p[c + 5]
				+ sin_135[i] * this_p[c + 6] + sin_113[i] * this_p[c + 7]
				+ sin_1[int(i / 2)] * d2 * this_p[c + 8];
			t[i + WAVETABLESIZE * 2] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4]
				+ sin_2[i] * this_p[c + 5]
				+ sin_135[i] * this_p[c + 6] + sin_113[int(i / 2)] * d2 * this_p[c + 7]
				+ sin_1[int(i / 2)] * d2 * this_p[c + 8];
			t[i + WAVETABLESIZE * 3] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4]
				+ sin_2[i] * this_p[c + 5]
				+ sin_135[int(i / 2)] * d2 * this_p[c + 6] + sin_113[int(i / 2)] * d2 * this_p[c + 7]
				+ sin_1[int(i / 2)] * d2 * this_p[c + 8];
			t[i + WAVETABLESIZE * 4] += sin_4[i] * this_p[c + 3] + sin_223[i] * this_p[c + 4]
				+ sin_2[int(i / 2)] * d2 * this_p[c + 5]
				+ sin_135[int(i / 2)] * d2 * this_p[c + 6] + sin_113[int(i / 2)] * d2 * this_p[c + 7]
				+ sin_1[int(i / 4)] * d4 * this_p[c + 8];
			t[i + WAVETABLESIZE * 5] += sin_4[i] * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4]
				+ sin_2[int(i / 2)] * d2 * this_p[c + 5]
				+ sin_135[int(i / 2)] * d2 * this_p[c + 6] + sin_113[int(i / 4)] * d4 * this_p[c + 7]
				+ sin_1[int(i / 4)] * d4 * this_p[c + 8];
			t[i + WAVETABLESIZE * 6] += sin_4[i] * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4]
				+ sin_2[int(i / 2)] * d2 * this_p[c + 5]
				+ sin_135[int(i / 4)] * 0 * this_p[c + 6] + sin_113[int(i / 4)] * d4 * this_p[c + 7]
				+ sin_1[int(i / 4)] * d4 * this_p[c + 8];
			t[i + WAVETABLESIZE * 7] += sin_4[int(i / 2)] * d2 * this_p[c + 3] + sin_223[int(i / 2)] * d2 * this_p[c + 4]
				+ sin_2[int(i / 4)] * d4 * this_p[c + 5]
				+ sin_135[int(i / 4)] * 0 * this_p[c + 6] + sin_113[int(i / 4)] * d4 * this_p[c + 7]
				+ sin_1[int(i / 8)] * d8 * this_p[c + 8];
		}
	}
	/*
		The grown up source code viewer will find that sin_135 is only
		folded once (/2). Well, I had terrible aliasing problems when
		folding it twice (/4), and the easiest solution was to set it to
		zero instead. You can't claim you actually heard it, can you?
	*/
	wavetable[WAVETABLESIZE * 12] = 0;
}

void effectMain::rebuildClickWavetable()
{
	/*
		Click is not just click - it has to follow the underlying
		note pitch. However, the click emulation is just "try and
		error". Improve it if you can, but PLEAZE tell me how you
		did it...
	*/
	click[0] = my_p[n_click] *
		(my_p[n_1_db1] + my_p[n_1_db2]
			+ my_p[n_1_db3] + my_p[n_1_db4]
			+ my_p[n_1_db5] + my_p[n_1_db6]
			+ my_p[n_1_db7] + my_p[n_1_db8]
			+ my_p[n_1_db9]) / 9;

	click[1] = my_p[n_click] *
		(my_p[n_2_db1] + my_p[n_2_db2]
			+ my_p[n_2_db3] + my_p[n_2_db4]
			+ my_p[n_2_db5] + my_p[n_2_db6]
			+ my_p[n_2_db7] + my_p[n_2_db8]
			+ my_p[n_2_db9]) / 9;

	click[2] = my_p[n_click] *
		(my_p[n_3_db1] + my_p[n_3_db2]
			+ my_p[n_3_db3] + my_p[n_3_db4]
			+ my_p[n_1_db5]) / 22;
}
