#include "voiceController.h"

void voiceController::reset()
{
	actual_note = -1;
	next_note = -1;
	perc_next_note = -1;
	perc_ok = false;
	vca_phase = VCA_PHASE_IDLE;
	output = 0;
	VCA = 0;
	perc = 0;
	perc_vca = 0;
	percmultiplier = 0;
	adsr_decay = 0;
	click = 0;
	status = VS_IDLE;
	pedal = false;
	pitch = 1;
	phase = phaseinc = 0;
	sustain = 0;
}

void voiceController::suspend()
{
	actual_note = -1;
	next_note = -1;
	perc_next_note = -1;
	vca_phase = VCA_PHASE_IDLE;
	output = 0;
	VCA = 0;
	perc_vca = 0;
	status = VS_IDLE;
	pitch = 1;
	phase = phaseinc = 0;
}

void voiceController::resume()
{
}

voiceController::voiceController()
{
	midi_scaler = (1. / 127.);

	long i;

	double k = 1.059463094359;	// 12th root of 2
	double a = 6.875;	// a
	a *= k;	// b
	a *= k;	// bb
	a *= k;	// c, frequency of midi note 0
	for (i = 0; i < 128; i++)	// 128 midi notes
	{
		freqtab[i] = (float)a;
		a *= k;
	}
	samplecount1 = samplecount2 = 0;
	sustain = 0;
	perc_ok = false;
};

void voiceController::set_samplerate(float samplerate)
{
	this->samplerate = samplerate;
	clicklp.set_samplerate(samplerate);
}

void voiceController::voicecalc()
{
	/*
		This "scaler" stuff is important to keep timing independant from
		the actual samplerate.
	*/
	float	scaler;
	scaler = samplerate / 44100;

	clickattack = .004f / scaler;
	adsr_attack = .10f / scaler;

	adsr_decay = 0 / scaler;
	if (sustain > 0)
		adsr_release = (.0001f + .0005f * (1 - sustain)) / scaler;
	else
		adsr_release = .95f / scaler;

	adsr_fast_release = .03f / scaler;

	perc_decay = perc_fade * .00035f / scaler;

	perc_decay *= 3;

	samplerate_scaler = (float)((double)my_size / (double)samplerate);
	phaseinc = (float)freqtab[actual_note] * samplerate_scaler * pitch;
	if (perc_phase == 0)
	{
		// fast sine calculation, precise enough for our percussion effect
		float	fact = percmultiplier * freqtab[actual_note];
		while (fact > 3800)
			fact *= .5f;
		a = 2 * sinf(3.14159265358979f * pitch * fact / samplerate);
		s0 = .5f;
		s1 = 0;
	}
}

float voiceController::clock()
{
	if (status == VS_IDLE || actual_note < 0)	// nothing to do...
		return(0);

	output = 0;
	/*
		This is the part where we read a value from the assigned wavetable.
		We use a very simple interpolation to determine the actual sample
		value. Since we use almost pure sine waves we don't have to take care
		about anti-aliasing here. The few aliasing effects we receive sound just
		like those real hardware tonewheels...
	*/
	iphase = int(phase);
	fract = phase - iphase;
	y0 = my_table[iphase];
	y1 = my_table[iphase + 1];
	output = (y0 + fract * (y1 - y0)) * VCA;

	phase += phaseinc;

	/*
		No, we don't use the bit mask stuff as mentioned in the SDK.
		It's _not_ slower this way, and we can have random wavetable sizes.
	*/
	if (phase > my_size)
		phase -= my_size;

	samplecount1++;
	if (samplecount1 > 5)
	{
		samplecount1 = 0;

		if (vca_phase == VCA_PHASE_ATTACK)		// attack
		{
			if (click <= 0)
			{
				VCA += adsr_attack;
			}

			if (VCA > 1)
			{
				VCA = 1;
				vca_phase = VCA_PHASE_DECAY;
			}
		}
		else if (vca_phase == VCA_PHASE_DECAY)
		{
			vca_phase = VCA_PHASE_SUSTAIN;
		}
		else if (vca_phase == VCA_PHASE_SUSTAIN)
		{
		}
		else if (vca_phase == VCA_PHASE_RELEASE)	// release
		{
			if (perc > 0)
			{
				if (sustain == 0)
					perc_phase = 3;
			}

			if (click > 0 && sustain == 0)
			{
				static unsigned long randSeed = 22222, maxrand = (unsigned long)-1;
				float rand = 0;
				randSeed = (randSeed * 196314165) + 907633515;
				rand = (float)randSeed / 0xffffffff;
				clicklp.clock(click * rand * .3f);
				noise = clicklp.bp() * clickvol * .2f;
				output += noise;
			}

			if (sustain > 0)
				VCA -= adsr_release;
			else
				VCA *= adsr_release;

			if (VCA <= 0.001f)
			{
				VCA = 0;
				actual_note = -1;
				phase = 0;
				vca_phase = VCA_PHASE_IDLE;
				output = 0;
				status = VS_IDLE;
			}
		}
		else if (vca_phase == VCA_PHASE_FAST_RELEASE)	// fast release
		{
			VCA -= adsr_fast_release;
			if (VCA <= 0)
			{
				VCA = 0;
				actual_note = -1;
				if (next_note >= 0)
				{
					actual_note = next_note;
					next_note = -1;
					vca_phase = VCA_PHASE_ATTACK;
					phase = 0;
					perc_phase = 0;
					this->voicecalc();
					if (perc_ok && perc > 0 && percmultiplier > 0)	// retrigger percussion
					{
						perc_phase = 1;
						perc_vca = 0;
					}
				}
				else
					status = VS_IDLE;
			}
		}
	}
	if (vca_phase == VCA_PHASE_ATTACK && click > 0)
	{
		static unsigned long randSeed = 22222, maxrand = (unsigned long)-1;
		float rand = 0;
		float mattack = 0;
		if (mattack < 1)
			mattack = VCA * 8;
		if (mattack > 1)
			mattack = 1;
		randSeed = (randSeed * 196314165) + 907633515;
		rand = (float)randSeed / 0xffffffff;
		clicklp.clock(click * rand * .3f);
		noise = clicklp.bp();
		noise *= clickvol;
		output = mattack * (2 - VCA) * (output + noise);
		VCA += clickattack;
	}

	if (perc_ok && perc_phase > 0)
	{
		s0 = s0 - a * s1;		// calculate sine wave
		s1 = s1 + a * s0;		//
		output += perc * s0 * perc_vca * perc_vca;

		samplecount2++;
		if (samplecount2 > 5)
		{
			samplecount2 = 0;
			if (perc_phase == 1)		// percussion attack
			{
				perc_vca += adsr_attack;
				if (perc_vca >= 1)
					perc_phase = 2;	// switch to percussion decay
			}
			else if (perc_phase == 2)	// percussion decay
			{
				perc_vca -= perc_decay;
				if (perc_vca <= 0)
				{
					perc_vca = 0;
					perc_phase = 0;	// percussion finished
				}
			}
			else if (perc_phase == 3)	// percussion fast release	
			{
				perc_vca -= adsr_fast_release;
				if (perc_vca <= 0)
				{
					perc_vca = 0;
					perc_phase = 0;	// percussion finished
				}
			}
		}
	}

	output = DENORMALIZE(output);
	return(output);
}

long voiceController::get_note()
{
	return(actual_note);
}

bool voiceController::check_note(long note)
{
	if (note == actual_note || note == next_note)
		return(true);
	else
		return(false);
}

bool voiceController::get_active()
{
	if (status == VS_IDLE)
		return(false);
	else
		return(true);
}

void voiceController::note_on(long note, long velocity, float* table, int size, float pitch, bool percenable, float sclick, float sust)
{
	my_table = table;
	my_size = size;
	click = sclick;
	perc_ok = percenable;
	sustain = sust;

	if (note >= 0 && note < 128)
	{
		if (click > 0)
		{
			float clickfreq = (freqtab[note] + 70) * 16;
			if (clickfreq > 5000)
				clickfreq = 5000;
			//clicklp.setparam(3000+clickfreq*.3f,.1f,samplerate);
			clicklp.setparam(clickfreq, .1f, samplerate);
			clickvol = note * note * .0008f;
		}
		if (actual_note >= 0)	// fast retrigger
		{
			next_note = note;
			vca_phase = VCA_PHASE_FAST_RELEASE;
			perc_phase = 3;
		}
		else				// normal note on
		{
			VCA = 0;
			phase = 0;
			phaseinc = 0;
			vca_phase = VCA_PHASE_ATTACK;

			actual_note = note & 0x7f;
			perc_phase = 0;
			this->voicecalc();
			perc_phase = 1;
		}

		status = VS_PLAYING;
	}
	else
		note_off(-1);
}

void voiceController::note_off(long note)
{
	if (note == actual_note && next_note >= 0)
	{
		vca_phase = VCA_PHASE_FAST_RELEASE;
		return;
	}

	if (!pedal)
		vca_phase = VCA_PHASE_RELEASE;
	else
		status = VS_WAIT_PUP;

}

void voiceController::force_off()
{
	next_note = -1;
	vca_phase = VCA_PHASE_FAST_RELEASE;
}

void voiceController::set_pedal(bool pedal)
{
	if (pedal != this->pedal)
	{
		if (!pedal)
			if (status == VS_WAIT_PUP)
				vca_phase = VCA_PHASE_RELEASE;

		this->pedal = pedal;
	}
}

void voiceController::set_percussion(float percussion, float perc_multiplier, float percfade)
{
	if (percussion >= 0)
		perc = percussion * 2;
	if (perc_multiplier >= 0)
		percmultiplier = perc_multiplier;
	if (percfade >= 0)
		perc_fade = 1 - percfade + .5f;
}

void voiceController::set_pitch(float pitch)
{
	this->pitch = pitch;
	this->voicecalc();
}
