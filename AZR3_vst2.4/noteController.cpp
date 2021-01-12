/*
	The voiceController producing machine, diverted into two blocks:

	VOICES actually calculate a single voiceController sound. They
	do basic voiceController handling like ADSR, pitch and the unevitable
	click and percussion effects.

	A NOTEMASTER instantiates as many voiceController class objects as needed
	for the current instrument. The numbers are tunable in "Globals.h",
	definition "NUMOFVOICES". It handles all incoming events like note
	on and off, all notes off, pedal, pitch bend and so on. The events
	will then be assigned to the corresponding voices. The noteController
	assigns a specific wavetable to each voiceController.	Only active voices
	are handled by the noteController - that's the main reason why the CPU
	meter goes up when you hold more notes. The positive effect is: The
	meter goes down if you use less voices...
*/
#include "noteController.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// -------- helper function
char* noteController::note2str(long note)
{
	static char	notestr[4];
	int			octave = int(note / 12);
	switch (note % 12)
	{
	case 0:
		sprintf(notestr, "C%1d", octave);
		break;
	case 1:
		sprintf(notestr, "C#%1d", octave);
		break;
	case 2:
		sprintf(notestr, "D%1d", octave);
		break;
	case 3:
		sprintf(notestr, "D#%1d", octave);
		break;
	case 4:
		sprintf(notestr, "E%1d", octave);
		break;
	case 5:
		sprintf(notestr, "F%1d", octave);
		break;
	case 6:
		sprintf(notestr, "F#%1d", octave);
		break;
	case 7:
		sprintf(notestr, "G%1d", octave);
		break;
	case 8:
		sprintf(notestr, "G#%1d", octave);
		break;
	case 9:
		sprintf(notestr, "A%1d", octave);
		break;
	case 10:
		sprintf(notestr, "A#%1d", octave);
		break;
	case 11:
		sprintf(notestr, "B%1d", octave);
		break;
	}
	return(notestr);
}

noteController::noteController(int number)
{
	my_samplerate = 44100;
	pitch = next_pitch = 1;
	my_percussion = -1;
	if (number < 1)
		number = 1;
	if (number > MAXVOICES)
		number = MAXVOICES;

	for (x = 0; x <= MAXVOICES; x++)
		voices[x] = NULL;

	for (x = 0; x <= number; x++)
	{
		voices[x] = new voiceController();
		if (voices[x] != NULL)
		{
			age[x] = 0;
			chan[x] = 15;
			voices[x]->reset();
			voices[x]->set_samplerate(my_samplerate);
			volume[x] = 1;
		}
	}

	numofvoices = number;
}

noteController::~noteController()
{
	for (x = 0; x < MAXVOICES; x++)
		if (voices[x] != NULL)
			voices[x]->~voiceController();
}

void noteController::set_numofvoices(int number)
{
	// we create one additional voiceController for channel 2 (bass pedal)
	if (number < 1)
		number = 1;
	if (number > MAXVOICES)
		number = MAXVOICES;
	for (x = 0; x <= MAXVOICES; x++)
	{
		if (voices[x] != NULL)
		{
			voices[x]->~voiceController();
		}
		voices[x] = NULL;
		age[x] = 0;
	}
	for (x = 0; x <= number; x++)
	{
		voices[x] = new voiceController();
		if (voices[x] != NULL)
		{
			age[x] = 0;
			chan[x] = 15;
			voices[x]->reset();
			voices[x]->set_samplerate(my_samplerate);
			voices[x]->set_percussion(my_percussion, my_perc_multiplier, my_percfade);
			volume[x] = 1;
		}
	}
	numofvoices = number;
}

void noteController::note_on(long note, long velocity, float* table, int size1, int channel, bool percenable, float click, float sustain)
{
	/*
		The most interesting part here is the note priority and "stealing"
		algorithm. We do it this way:
		If a new note on event is received we look for an idle voiceController. If
		there's none available we choose the oldest active voiceController and let it
		perform a fast note off followed by a note on. This "fast retrigger"
		is entirely calculated by the voiceController itself.

		"Mono" mode is defined by numofvoices=1.
	*/
	int maxpos = 0, newpos;
	unsigned long	maxage;

	note -= 12;

	if (note < 0)
		return;
	if (note > 128)
		return;

	if (channel == 2)
	{
		newpos = numofvoices;	// reserved voiceController for bass pedal (channel 2)
	}
	else
	{
		maxage = 0;
		newpos = -1;
		maxpos = 0;

		// calculate newpos - the voiceController number to produce the note
		for (x = 0; x < numofvoices; x++)
		{
			if (voices[x] == NULL)
				continue;

			// do we have an existing note?
			// -> retrigger
			if (voices[x]->get_active() && voices[x]->check_note(note) && chan[x] == (unsigned char)channel)
			{
				newpos = x;
				age[x] = 0;
			}

			if (voices[x]->get_active())
			{
				// age all active voices
				age[x]++;

				// let maxpos hold number of oldest voiceController
				if (age[x] > maxage)
				{
					maxpos = x;
					maxage = age[x];
				}
			}
			else if (newpos == -1)	// voiceController is not active. If we don't have
									// to retrigger - this is our voiceController!
				newpos = x;
		}

		if (newpos == -1)		// no free voiceController and nothing to retrigger
			newpos = maxpos;	// -> choose oldest voiceController
	}

	// let the voiceController play the note. Fast retrigger is handled by the voiceController.
	voices[newpos]->note_on(note, velocity, table, size1, pitch, percenable, click, sustain);
	age[newpos] = 0;
	if (channel > 0 && channel < 3)
		chan[newpos] = (unsigned char)channel;
	else
		chan[newpos] = 0;
}

void noteController::note_off(long note, int channel)
{
	note -= 12;

	if (note < 0)
		return;

	if (channel == 2)
	{
		if (voices[numofvoices]->check_note(note))
			voices[numofvoices]->note_off(note);
	}
	else
	{
		for (x = 0; x < numofvoices; x++)
		{
			if (chan[x] == (unsigned char)channel && voices[x]->check_note(note))
			{
				voices[x]->note_off(note);
				age[x] = 0;
			}
		}
	}
}

float* noteController::clock()
{
	output[0] = output[1] = output[2] = 0;
	for (x = 0; x <= numofvoices; x++)
		if (chan[x] < 3)
			output[chan[x]] += volume[chan[x]] * voices[x]->clock();
	return(output);
}

void noteController::all_notes_off()
{
	pitch = next_pitch = 1;
	for (x = 0; x <= numofvoices; x++)
	{
		voices[x]->force_off();
		age[x] = 0;
	}
}

void noteController::set_pedal(int pedal, int channel)
{
	bool	my_pedal;
	if (pedal < 64)
		my_pedal = false;
	else
		my_pedal = true;
	for (x = 0; x <= numofvoices; x++)
		voices[x]->set_pedal(my_pedal);
}

void noteController::set_samplerate(float samplerate)
{
	my_samplerate = samplerate;
	for (x = 0; x <= numofvoices; x++)
		voices[x]->set_samplerate(samplerate);
}

void noteController::set_percussion(float percussion, float perc_multiplier, float percfade)
{
	my_percussion = percussion;			// memorize percussion values
	my_perc_multiplier = perc_multiplier;
	my_percfade = percfade;

	for (x = 0; x < numofvoices; x++)
		voices[x]->set_percussion(percussion, perc_multiplier, percfade);

	voices[numofvoices]->set_percussion(percussion * .3f, perc_multiplier, percfade);
}

void noteController::set_pitch(float pitch, int channel)
{
	this->pitch = pitch;
	for (x = 0; x <= numofvoices; x++)
	{
		if (chan[x] == channel)
			voices[x]->set_pitch(pitch);
	}
}

void noteController::set_volume(float vol, int channel)
{
	volume[channel] = vol;
}

void noteController::reset()
{
	for (x = 0; x <= numofvoices; x++)
		voices[x]->reset();
}

void noteController::suspend()
{
	for (x = 0; x <= numofvoices; x++)
		voices[x]->suspend();
}

void noteController::resume()
{
	for (x = 0; x <= numofvoices; x++)
		voices[x]->resume();
}
