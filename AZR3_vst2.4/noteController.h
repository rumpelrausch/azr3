#pragma once
#include "voiceController.h"

class noteController
{
public:
	noteController(int number);		// Anzahl der Stimmen
	~noteController();
	static char* note2str(long note);
	void	set_numofvoices(int number);
	void	note_on(long note, long velocity, float *table, int size1, int channel, bool percenable, float click, float sustain);
	void	all_notes_off();
	float	*clock();
	void	note_off(long note, int channel);
	void	set_pedal(int pedal, int channel);
	void	set_percussion(float percussion,float perc_multiplier,float percfade);
	void	set_samplerate(float samplerate);
	void	set_pitch(float pitch, int channel);
	void	set_volume(float vol, int channel);
	void	reset();
	void	suspend();
	void	resume();
private:
	voiceController	*voices[MAXVOICES+1];
	int		numofvoices;
	unsigned long	age[MAXVOICES];
	unsigned char	chan[MAXVOICES];
	float	volume[MAXVOICES];
	float	output[16];
	int		x;
	float	pitch,next_pitch;

	float	my_click,my_percussion,my_perc_multiplier,my_percfade,my_samplerate;
};
