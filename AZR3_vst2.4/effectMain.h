#pragma once
#include "audioeffectx.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "noteController.h"
#include "Globals.h"
#include "Editor.h"
#include "FX/fxDelay.h"
#include "FX/fxLFO.h"
#include "FX/fxGenericFilter.h"
#include "FX/fxGenericLowpass.h"
#include "FX/fxAllpass.h"
#include "vstProgram.h"

enum
{
	evt_none = 0,
	evt_noteon,
	evt_noteoff,
	evt_alloff,
	evt_pedal,
	evt_progchange,
	evt_pitch,
	evt_modulation,
	evt_volume,
	evt_channel_volume,
	evt_drawbar,
}; 

class effectMain : public AudioEffectX
{
	friend class MoogProgram;
public:
	effectMain(audioMasterCallback audioMaster);
	~effectMain();

	void process(float** inputs, float** outputs, long sampleframes);
	void processReplacing(float** inputs, float** outputs, VstInt32 sampleframes);
	VstInt32 processEvents(VstEvents* events);

	void setProgram(VstInt32 program, bool force);
	bool copyProgram(VstInt32 destination);
	void setInternalParameter(VstInt32 index, float value);

	//---from AudioEffect-----------------------
	virtual void setProgram(VstInt32 program);
	virtual void setProgramName(char* name);
	virtual void getProgramName(char* name);
	virtual VstInt32 getProgram();
	virtual void setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void getParameterLabel(VstInt32 index, char* label);
	virtual void getParameterDisplay(VstInt32 index, char* text);
	virtual void getParameterName(VstInt32 index, char* text);
	virtual void setSampleRate(float sampleRate);
	virtual void resume();
	virtual void suspend();

	virtual bool getOutputProperties(VstInt32 index, VstPinProperties* properties);
	//virtual bool getInputProperties(VstInt32 index, VstPinProperties* properties);
	virtual bool getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text);
	virtual bool getEffectName(char* name);
	virtual bool getVendorString(char* text);
	virtual bool getProductString(char* text);
	virtual VstInt32 getVendorVersion() { return 1; }
	virtual VstInt32 canDo(char* text);
	virtual VstPlugCategory getPlugCategory() { return kPlugCategSynth; }

	void setFactorySounds();
	bool buildTonewheelSet(int shape);
	void rebuildWavetableSet(int number);
	void rebuildClickWavetable();
	void initall()
	{
		int x;
		mono_before = 0;
		samplecount = 0;
		mono = mono1 = mono2 = l_before = r_before = 0;

		viblfo = 0;
		vmix1 = vmix2 = 0;

		dist = fuzz = odmix = 0;
		spread = spread2 = cross1 = 0;
		lslow = lfast = uslow = ufast = ubelt_up = ubelt_down = lbelt_up = lbelt_down = lspeed = uspeed = 0;
		er_r = er_l = 0;
		lp = right = left = lright = lleft = upper = lower = 0;
		llfo_out = llfo_nout = llfo_d_out = llfo_d_nout = 0;
		lfo_out = lfo_nout = lfo_d_out = lfo_d_nout = 0;
		last_out1 = last_out2 = 0;

		VCA = 1;
		compare = false;
		waitforsplit = false;
		splitpoint = 0;

		gp_value = 0;
		last_shape = -1;

		odmix = 0;
		vmix1 = 0;
		vmix2 = 0;

		for (x = 0; x < WAVETABLESIZE * 12 + 1; x++)
			wavetable[x] = 0;

		for (x = 0; x < kNumParams; x++)
			last_value[x] = -99;

		for (x = 0; x < EVTBUFSIZE; x++)
		{
			this->delta[x] = -10;
			this->events[x] = evt_none;
		}
		event_pos = event_read_pos = 0;
		has_events = false;

		if (samplerate < 8000)
			samplerate = 44100;

#ifdef DEVELOP
		ctrl1 = .5f;
		ctrl2 = .51f;
		ctrl3 = .44f;
		ctrl4 = 0;
#endif

	}
	void add_event(unsigned char type, long delta, float value1, float value2, float value3);
	float* event_clock();

protected:
	noteController* n1;
	bool	mute, fullyloaded;
	float	click[16];
	float	volume[16];
	float	mono_before;
	float* out1, * out2, * in1, * in2;
	bool	compare, comparebuffer;
	float	samplerate;
	long	samplecount;
	bool	param_is_manual;
	bool	waitforsplit;
	long	splitpoint;

	float	p[kNumParams], * my_p, * virtual_my_p, actual_p[kNumParams], i_p[kNumParams];
	float	last_value[kNumParams];
	float* p_mono, mono, mono1, mono2, VCA, l_before, r_before;
	long	setProgramDelayCount, actual_curProgram;

	vstProgram	programs[kNumPrograms];

	float tonewheel[WAVETABLESIZE];	// master waveform

	float sin_16[WAVETABLESIZE];
	float sin_8[WAVETABLESIZE];
	float sin_513[WAVETABLESIZE];
	float sin_4[WAVETABLESIZE];
	float sin_223[WAVETABLESIZE];
	float sin_2[WAVETABLESIZE];
	float sin_135[WAVETABLESIZE];
	float sin_113[WAVETABLESIZE];
	float sin_1[WAVETABLESIZE];

	// TABLES_PER_CHANNEL tables per channel; 3 channels; 1 spare table
#define TABLES_PER_CHANNEL	8
	float wavetable[WAVETABLESIZE * TABLES_PER_CHANNEL * 3 + 1];

	fxLFO		vlfo;
	fxDelay* vdelay1, * vdelay2;
	float	viblfo;
	bool	vibchanged1, vibchanged2, lfo_calced;
	float	vmix1, vmix2;
	fxGenericLowpass	warmth;

	fxGenericFilter	fuzz_filt, body_filt, postbody_filt;
	float	dist, sin_dist, i_dist, dist4, dist8;
	float	fuzz;
	bool	do_dist;
	bool	odchanged;
	float	odmix, n_odmix, n2_odmix, n25_odmix, odmix75;

	float	spread, spread2;
	float	cross1;
	bool	fastmode;
	float	lslow, lfast, uslow, ufast, lspeed, uspeed;
	float	ubelt_up, ubelt_down, lbelt_up, lbelt_down;
	float	er_r, er_r_before, er_l, er_feedback;
	float	lp, right, left, lright, lleft, upper, lower, upper_damp;
	float	llfo_out, llfo_nout, llfo_d_out, llfo_d_nout;
	float	lfo_out, lfo_nout, lfo_d_out, lfo_d_nout;
	float	last_out1, last_out2;
	bool	lfos_ok;
	fxGenericFilter	split;
	fxGenericFilter	horn_filt, damp;
	fxDelay* wand_r, * wand_l, * delay1, * delay2, * delay3, * delay4;
	fxLFO		lfo1, lfo2, lfo3, lfo4;

	float		gp_value;
	int			last_shape;
	float		last_r, last_l;
	float* evt;
	// stuff for event handling
	float		values[4], * ret;
	bool		has_events;
	int			event_pos, event_last_pos, event_next_pos, event_read_pos;
	long		delta[EVTBUFSIZE];
	unsigned char	events[EVTBUFSIZE];
	float		evalue1[EVTBUFSIZE];
	float		evalue2[EVTBUFSIZE];
	float		evalue3[EVTBUFSIZE];
	fxAllpass	allpass_l[4], allpass_r[4];
	float		lfo_phaser1, lfo_phaser2;

#ifdef DEVELOP
	float		ctrl1, ctrl2, ctrl3, ctrl4;
#endif

};
