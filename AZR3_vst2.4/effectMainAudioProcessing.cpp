#include "effectMain.h"

//void effectMain::process(float** inputs, float** outputs, long sampleFrames)
//{
//	// Do this hack only for VST instruments!!!
//	processReplacing(inputs, outputs, sampleFrames);
//}

void effectMain::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	/*
	OK, here we go. This is the order of actions in here:
	- process event queue
	- return zeroes if in "mute" state
	- clock the "noteController" and get the combined sound output
	from the voices.
	We actually get three values, one for each keyboard.
	They are added according to the assigned channel volume
	control values.
	- calculate switch smoothing to prevent clicks
	- vibrato
	- additional low pass "warmth"
	- distortion
	- speakers
		*/
	out1 = outputs[0];
	out2 = outputs[1];
	in1 = inputs[0];
	in2 = inputs[1];
	int	x;

	while (--sampleFrames >= 0)
	{
#ifdef DELAYED_PARAMS
		if (setProgramDelayCount > 0)
		{
			setProgramDelayCount++;
			// here we confirm a requested program change
			if (setProgramDelayCount > 4096)
			{
				setProgramDelayCount = -1;
				setProgram(curProgram);
			}
		}
#endif
		// we need this variable further down
		samplecount++;
		if (fullyloaded)
		{
			if (samplecount > 10000)
				samplecount = 0;
		}
		else
		{
			// This is one of the two methods to finally activate everything.
			// We need some time in advance in order to fill all buffers with correct data.
			// The other method is located at setParameter and responds to the editor
			// event "n_mute".
			if (samplecount > ((long)(samplerate * 1.5f)))
			{
				samplecount = 0;
				if (fullyloaded == false)
				{
					fullyloaded = true;
					resume();
				}
			}
		}

		// read events from our own event queue
		while ((evt = this->event_clock()) != NULL)
		{
			int channel = (int)evt[3];
			float* tbl;

			// flash channel LEDs
			if (channel <= 0)
				setParameter(n_1_midi, 1);
			if (channel == 1 || channel < 0)
				setParameter(n_2_midi, 1);
			if (channel == 2 || channel < 0)
				setParameter(n_3_midi, 1);

			if (channel < 0 || channel>2)
				channel = 0;
			switch ((unsigned char)evt[0])
			{
			case evt_noteon:
			{
				int		note = (int)evt[1];
				bool	percenable = false;
				float	sustain = my_p[n_sustain] + .0001f;

				// here we choose the correct wavetable according to the played note
#define foldstart 80
				if (note > foldstart + 12 + 12)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 7];
				else if (note > foldstart + 12 + 8)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 6];
				else if (note > foldstart + 12 + 5)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 5];
				else if (note > foldstart + 12)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 4];
				else if (note > foldstart + 8)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 3];
				else if (note > foldstart + 5)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE * 2];
				else if (note > foldstart)
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL + WAVETABLESIZE];
				else
					tbl = &wavetable[channel * WAVETABLESIZE * TABLES_PER_CHANNEL];

				if (channel == 0)
				{
					if (my_p[n_1_perc] > 0)
						percenable = true;
					if (my_p[n_1_sustain] < .5f)
						sustain = 0;
				}
				else if (channel == 1)
				{
					if (my_p[n_2_perc] > 0)
						percenable = true;
					if (my_p[n_2_sustain] < .5f)
						sustain = 0;
				}
				else if (channel == 2)
				{
					if (my_p[n_3_perc] > 0)
						percenable = true;
					if (my_p[n_3_sustain] < .5f)
						sustain = 0;
				}

				n1->note_on(note,
					(long)this->evt[2],
					tbl, WAVETABLESIZE, channel, percenable, click[channel], sustain);
			}
			break;
			case evt_noteoff:
				n1->note_off((long)evt[1], channel);
				break;
			case evt_alloff:
				n1->all_notes_off();
				break;
			case evt_pedal:
				n1->set_pedal((int)evt[1], channel);
				break;
			case evt_progchange:
				if (evt[1] < kNumPrograms && evt[1] != curProgram)
				{
					param_is_manual = true;
					setProgram((long)evt[1]);
				}
				break;
			case evt_pitch:
				n1->set_pitch(evt[1], channel);
				break;
			case evt_modulation:
				param_is_manual = true;
				setParameterAutomated(n_speed, evt[1]);
				param_is_manual = false;
				break;
			case evt_volume:
				my_p[n_master] = evt[1];
				break;
			case evt_channel_volume:
				param_is_manual = true;
				setParameterAutomated(n_vol1 + channel, evt[1] / 127.0f);
				param_is_manual = false;
				break;
			case evt_drawbar:
				if (channel == 2 && evt[1] > 4)
					break;
				if (evt[1] < 0 || evt[1]>8)
					break;
				if (evt[1] == 1)
					evt[1] = 2;
				else if (evt[1] == 2)
					evt[1] = 1;
				int offset = n_1_db1;
				if (channel == 1)
					offset = n_2_db1;
				else if (channel == 2)
					offset = n_3_db1;
				param_is_manual = true;
				setParameterAutomated(offset + (int)evt[1], evt[2] / 127.0f);
				param_is_manual = false;
				break;
			}
		}

		p_mono = n1->clock();
		mono1 = p_mono[0];
		mono1 += (*in1 + *in2) / 2; // mix in inputs
		mono2 = p_mono[1];
		mono = p_mono[2];

		// smoothing of vibrato switch 1
		if (vibchanged1 && samplecount % 10 == 0)
		{
			if (my_p[n_1_vibrato] == 1)
			{
				vmix1 += .01f;
				if (vmix1 >= my_p[n_1_vmix])
					vibchanged1 = false;
			}
			else
			{
				vmix1 -= .01f;
				if (vmix1 <= 0)
					vibchanged1 = false;
			}
		}

		// smoothing of vibrato switch 2
		if (vibchanged2 && samplecount % 10 == 0)
		{
			if (my_p[n_2_vibrato] == 1)
			{
				vmix2 += .01f;
				if (vmix2 >= my_p[n_2_vmix])
					vibchanged2 = false;
			}
			else
			{
				vmix2 -= .01f;
				if (vmix2 <= 0)
					vibchanged2 = false;
			}
		}

		// smoothing of OD switch
		if (odchanged && samplecount % 10 == 0)
		{
			if (my_p[n_mrvalve] == 1)
			{
				odmix += .05f;
				if (odmix >= my_p[n_mix])
					odchanged = false;
			}
			else
			{
				odmix -= .05f;
				if (odmix <= 0)
					odchanged = false;
			}
			n_odmix = 1 - odmix;
			n2_odmix = 2 - odmix;
			odmix75 = .75f * odmix;
			n25_odmix = n_odmix * .25f;
		}

		// Vibrato LFO
		lfo_calced = false;

		// Vibrato 1
		if (my_p[n_1_vibrato] == 1 || vibchanged1)
		{
			if (samplecount % 5 == 0)
			{
				viblfo = vlfo.clock();
				lfo_calced = true;
				vdelay1->set_delay(viblfo * 2 * my_p[n_1_vstrength]);
			}
			mono1 = (1 - vmix1) * mono1 + vmix1 * vdelay1->clock(mono1);
		}

		// Vibrato 2
		if (my_p[n_2_vibrato] == 1 || vibchanged2)
		{
			if (samplecount % 5 == 0)
			{
				if (!lfo_calced)
					viblfo = vlfo.clock();
				vdelay2->set_delay(viblfo * 2 * my_p[n_2_vstrength]);
			}
			mono2 = (1 - vmix2) * mono2 + vmix2 * vdelay2->clock(mono2);
		}


		// ...add a minimal offset to prevent denormalization.
		// Don't know wether it's just superstition...
		mono += mono1 + mono2 + .0001f;

		// Mr. Valve
		/*
		Completely rebuilt.
		Multiband distortion:
		The first atan() waveshaper is applied to a lower band. The second
		one is applied to the whole spectrum as a clipping function (combined
		with an fabs() branch).
		The "warmth" filter is now applied _after_ distortion to flatten
		down distortion overtones. It's only applied with activated distortion
		effect, so we can switch warmth off and on without adding another parameter.
		*/
		if (my_p[n_mrvalve] == 1 || odchanged)
		{
			if (do_dist)
			{
				body_filt.clock(mono);
				postbody_filt.clock(atanf(body_filt.lp() * dist8) * 6);
				fuzz = atanf(mono * dist4) * .25f + postbody_filt.bp() + postbody_filt.hp();

				if (_fabsf(mono) > my_p[n_set])
				{
					fuzz = atanf(fuzz * 10);
				}
				fuzz_filt.clock(fuzz);
				mono = ((fuzz_filt.lp() * odmix * sin_dist + mono * (n2_odmix)) * sin_dist) * i_dist;
			}
			else
			{
				fuzz_filt.clock(mono);
				mono = fuzz_filt.lp() * odmix75 + mono * n25_odmix * i_dist;
			}
			warmth.clock(mono);
			mono = DENORMALIZE(warmth.lp());
		}

		// Speakers
		/*
		I started the rotating speaker sim from scratch with just
		a few sketches about how reality looks like:
		Two horn speakers, rotating in a circle. Combined panning
		between low and mid filtered sound and the volume. Add the
		doppler effect. Let the sound of one speaker get reflected
		by a wall and mixed with the other speakers' output. That's
		all not too hard to calculate and to implement in C++, and
		the results were already quite realistic. However, to get
		more density and the explicit "muddy" touch I added some
		phase shifting gags and some unexpected additions with
		the other channels' data. The result did take many nights
		of twiggling wih parameters. There are still some commented
		alternatives; feel free to experiment with the emulation.
		Never forget to mono check since there are so many phase
		effects in here you might end up in the void.
		I'm looking forward to the results...
		*/

		/*
		Update:
		I added some phase shifting using allpass filters.
		This should make it sound more realistic.
		*/

		if (my_p[n_speakers] == 1)
		{
			if (samplecount % 100 == 0)
			{
				if (fastmode)
				{
					if (lspeed < lfast)
						lspeed += lbelt_up;
					if (lspeed > lfast)
						lspeed = lfast;

					if (uspeed < ufast)
						uspeed += ubelt_up;
					if (uspeed > ufast)
						uspeed = ufast;
				}
				else
				{
					if (lspeed > lslow)
						lspeed -= lbelt_down;
					if (lspeed < lslow)
						lspeed = lslow;
					if (uspeed > uslow)
						uspeed -= ubelt_down;
					if (uspeed < uslow)
						uspeed = uslow;
				}

				//recalculate mic positions when "spread" has changed
				if (!lfos_ok)
				{
					float s = (my_p[n_spread] + .5f) * .8f;
					spread = (s) * 2 + 1;
					spread2 = (1 - spread) / 2;
					lfo1.set_phase(0);
					lfo2.set_phase(s / 2);
					lfo3.set_phase(0);
					lfo4.set_phase(s / 2);
					cross1 = 1.5f - 1.2f * s;
					// early reflections depend upon mic position.
					// we want less e/r if mics are positioned on
					// opposite side of speakers.
					// when positioned right in front of them e/r
					// brings back some livelyness.
					//
					// so "spread" does the following to the mic positions:
					// minimum: mics are almost at same position (mono) but
					// further away from cabinet.
					// maximum: mics are on opposite sides of cabinet and very
					// close to speakers.
					// medium: mics form a 90° angle, heading towards cabinet at
					// medium distance.
					er_feedback = .1f * cross1;
					lfos_ok = true;
				}

				if (lspeed != lfo3.get_rate())
				{
					lfo3.set_rate(lspeed * 5, 1);
					lfo4.set_rate(lspeed * 5, 1);
				}

				if (uspeed != lfo1.get_rate())
				{
					lfo1.set_rate(uspeed * 5, 1);
					lfo2.set_rate(uspeed * 5, 1);
				}
			}

			// split signal into upper and lower cabinet speakers
			split.clock(mono);
			lower = split.lp() * 5;
			upper = split.hp();


			// upper speaker is kind of a nasty horn
			horn_filt.clock(upper);
			damp.clock(upper);
			upper = upper + horn_filt.lp() * 0.3;
			upper_damp = damp.lp();

			// do fxLFO stuff
			if (samplecount % 5 == 0)
			{
				lfo_d_out = lfo1.clock();
				lfo_d_nout = 1 - lfo_d_out;

				delay1->set_delay(10 + lfo_d_out * .8f);
				delay2->set_delay(17 + lfo_d_nout * .8f);

				lfo_d_nout = lfo2.clock();

				lfo_out = lfo_d_out * spread + spread2;
				lfo_nout = lfo_d_nout * spread + spread2;

				// phase shifting lines
				// A light bulb and some LDRs...
				lfo_phaser1 = (1 - cosf(lfo_d_out * 1.8f) + 1) * .054f;
				lfo_phaser2 = (1 - cosf(lfo_d_nout * 1.8f) + 1) * .054f;
				for (x = 0; x < 4; x++)
				{
					allpass_r[x].set_delay(lfo_phaser1);
					allpass_l[x].set_delay(lfo_phaser2);
				}

				if (lslow > 0)
				{
					llfo_d_out = lfo3.clock();
					llfo_d_nout = 1 - llfo_d_out;
				}

				// additional fxDelay lines in complex mode
				if (my_p[n_complex] > .5f)
				{
					delay4->set_delay(llfo_d_out + 15);
					delay3->set_delay(llfo_d_nout + 25);
				}

				llfo_d_nout = lfo4.clock();
				llfo_out = llfo_d_out * spread + spread2;
				llfo_nout = llfo_d_nout * spread + spread2;
			}

			if (lslow > 0)
			{
				lright = (1 + .6f * llfo_out) * lower;
				lleft = (1 + .6f * llfo_nout) * lower;
			}
			else
			{
				lright = lleft = lower;
			}

			// emulate horn characteristics
			// (sound is dampened when listened from aside)
			//right = DENORMALIZE((3 + lfo_nout * 2.5f) * upper + 1.5f * upper_damp);
			//left = DENORMALIZE((3 + lfo_out * 2.5f) * upper + 1.5f * upper_damp);
			//right = DENORMALIZE((3 + lfo_nout * 1.5f) * upper + 1.5f * upper_damp);
			//left = DENORMALIZE((3 + lfo_out * 1.5f) * upper + 1.5f * upper_damp);
			right = DENORMALIZE((3 + lfo_nout) * upper * 3.3f - upper_damp * 16.5f * (3 + lfo_out));
			left = -DENORMALIZE((3 + lfo_out) * upper * 3.3f - upper_damp * 16.5f * (3 + lfo_nout));

			//phaser...
			last_r = allpass_r[0].clock(
				allpass_r[1].clock(
					allpass_r[2].clock(
						allpass_r[3].clock(upper + last_r * .33f))));
			last_l = allpass_l[0].clock(
				allpass_l[1].clock(
					allpass_l[2].clock(
						allpass_l[3].clock(upper + last_l * .33f))));

			right += last_r;
			left += last_l;

			// rotating speakers can only develop in a live room -
			// wouldn't work without some early reflections.
			er_r = wand_r->clock(right + lright - (left * .3f) - er_l * er_feedback);
			er_r = DENORMALIZE(er_r);
			er_l = wand_l->clock(left + lleft - (right * .3f) - er_r_before * er_feedback);
			er_l = DENORMALIZE(er_l);
			er_r_before = er_r;


			// We use two additional fxDelay lines in "complex" mode
			if (my_p[n_complex] > .5f)
			{
				right = right * .3f + 1.5f * er_r + delay1->clock(right) + delay3->clock(er_r) + (1.5 * upper) + (1.5 * lower);
				left = left * .3f + 1.5f * er_l + delay2->clock(left) + delay4->clock(er_l) + (1.5 * upper) + (1.5 * lower);
			}
			else
			{
				right = right * .3f + 1.5f * er_r + delay1->clock(right) + lright + (1.5 * upper) + (1.5 * lower);
				left = left * .3f + 1.5f * er_l + delay2->clock(left) + lleft + (1.5 * upper) + (1.5 * lower);
			}

			right *= .033f;
			left *= .033f;

			// spread crossover (emulates mic positions)
			last_out1 = (left + cross1 * right) * my_p[n_master];
			last_out2 = (right + cross1 * left) * my_p[n_master];
		}
		else
		{
			last_out1 = last_out2 = mono * my_p[n_master];
		}
		if (mute)
		{
			last_out1 = 0;
			last_out2 = 0;
		}

		(*out1++) = last_out1;
		(*out2++) = last_out2;

		in1++;
		in2++;
	}

}

void effectMain::resume()
{
	//	wantEvents ();
	if (fullyloaded)
	{
		mute = false;
	}
}

void effectMain::suspend()
{
	int x;
	n1->suspend();
	mute = true;

	for (x = 0; x < 4; x++)
	{
		allpass_r[x].reset();
		allpass_l[x].reset();
	}

	delay1->flood(0);
	delay2->flood(0);
	delay3->flood(0);
	delay4->flood(0);
	vdelay1->flood(0);
	vdelay2->flood(0);
	wand_r->flood(0);
	wand_l->flood(0);

	param_is_manual = false;
}
