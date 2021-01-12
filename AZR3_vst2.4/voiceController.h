#pragma once
#include "FX/fxGenericFilter.h"
#include <math.h>

#define MAXVOICES	32

// voiceController stati	(status)
#define	VS_IDLE		0
#define	VS_PLAYING	1
#define	VS_WAIT_PUP	2	// has note_off and waits for pedal up

// VCA phases	(vca_phase)
#define	VCA_PHASE_IDLE		0
#define	VCA_PHASE_ATTACK	1	// attack
#define VCA_PHASE_DECAY		2	// decay
#define	VCA_PHASE_SUSTAIN	3	// sustain = main phase
#define	VCA_PHASE_RELEASE	4	// release
#define	VCA_PHASE_FAST_RELEASE	5	// fast release
#define	VCA_PHASE_FAST_ATTACK	6	// fast attack

#include <stdio.h>

char* note2str(long note);

class voiceController
{
public:
	voiceController();
	~voiceController() {}
	float	clock();
	void	reset();
	void	suspend();
	void	resume();
	void	note_on(long note, long velocity, float* table, int size, float pitch, bool percenable, float sclick, float sust);
	void	note_off(long note);
	void	force_off();
	long	get_note();
	bool	check_note(long note);
	bool	get_active();
	void	set_samplerate(float samplerate);
	void	set_attack(float attack);
	void	set_decay(float decay);
	void	set_release(float release);
	void	set_pedal(bool pedal);
	void	set_percussion(float percussion, float perc_multiplier, float percfade);
	void	set_pitch(float pitch);
	void	voicecalc();
private:
	unsigned char	samplecount1, samplecount2;
	int		status;
	int		iphase;
	float	y0, y1, fract;
	float	samplerate_scaler;	// Anpassung der Wavetable-Logik an Samplerate
	float	phase;				// Position in der Wavetable
	float	phaseinc;			// increment for phase
	float	output;				// Ausgang
	float	click;				// click strength
	float	hertz, a, s0, s1;		// percussion sine values
	float	percmultiplier;		// percussion octave multiplier
	float	perc;				// percussion volume
	bool	perc_ok;
	float	perc_decay;
	float	perc_vca;
	int		perc_phase;
	float	perc_attack;
	float	perc_fade;
	long	actual_note;	// Note-Daten
	long	next_note;			// Vorbesetzung von actual_note.
	long	perc_next_note;
	float* my_table;			// die Wavetable
	long	mask, my_size;		// Maske und Grˆﬂe der Wavetable
	float	samplerate;
	double	midi_scaler;		// Umrechung Midi->float-Faktor [0..1]
	float	freqtab[128];		// Umrechnung Midi->Frequenz
	float	noise;
	float	clickattack;
	float	clickvol;
	float	adsr_attack;
	float	adsr_decay;
	float	adsr_sustain;
	float	adsr_release;
	float	adsr_fast_release;
	float	sustain;
	float	VCA;				// VCA-Faktor. Wird durch attack und release beeinfluﬂt
	int		vca_phase;			// 1:Attack 2:Release
	bool	pedal;				// Pedalzustand
	float	pitch;
	fxGenericFilter	clicklp;
};
