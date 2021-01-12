#include "effectMain.h"

void effectMain::getParameterLabel(VstInt32 index, char* label)
{
	// forget it, we don't fully support non-GUI interface...
	// sprintf(label, "",index);
	if (index < kNumParams)
		strcpy(label, labels[index]);
	else
		strcpy(label, "---");
}

void effectMain::getParameterDisplay(VstInt32 index, char* text)
{
	text[0] = 0;
	switch (index)
	{
	case n_bender:
		parameterConversions::convert_bendrange(p[index], text);
		break;
	case n_tone:
		parameterConversions::convert_tone(p[index], text);
		break;
	case n_l_slow:
	case n_u_slow:
	case n_l_fast:
	case n_u_fast:
		parameterConversions::convert_speed(p[index], text);
		break;
	case n_perc:
		parameterConversions::convert_perc(p[index], text);
		break;
	case n_shape:
		parameterConversions::convert_shape(p[index], text);
		break;
	default:
		sprintf(text, "%3.1f %%", 100 * p[index]);
		break;
	}
}

void effectMain::getParameterName(VstInt32 index, char* label)
{
	if (index < kNumParams)
		strcpy(label, labels[index]);
	else
		strcpy(label, "---");
}

void effectMain::setInternalParameter(VstInt32 index, float value)
{
	if (index<0 || index > kNumParams || (!param_is_manual && setProgramDelayCount >= 0))
		return;

	if (compare)
	{
		vstProgram* ap = &programs[curProgram];
		value = ap->p[index];			// fetch value from program memory
		my_p = ap->p;					// let machine use program memory
	}
	else
	{
		p[index] = value;				// put value into edit buffer
		my_p = p;						// let machine use edit buffer
	}

	//if(value!=last_value[index])
	{
		last_value[index] = value;
		switch (index)
		{
		case n_1_db1:
		case n_1_db2:
		case n_1_db3:
		case n_1_db4:
		case n_1_db5:
		case n_1_db6:
		case n_1_db7:
		case n_1_db8:
		case n_1_db9:
			rebuildWavetableSet(1);
			rebuildClickWavetable();
			break;
		case n_2_db1:
		case n_2_db2:
		case n_2_db3:
		case n_2_db4:
		case n_2_db5:
		case n_2_db6:
		case n_2_db7:
		case n_2_db8:
		case n_2_db9:
			rebuildWavetableSet(2);
			rebuildClickWavetable();
			break;
		case n_3_db1:
		case n_3_db2:
		case n_3_db3:
		case n_3_db4:
		case n_3_db5:
			rebuildWavetableSet(3);
			rebuildClickWavetable();
			break;
		case n_shape:
			if (buildTonewheelSet(int(value * (NUM_OF_TONEWHEEL_SHAPES - 1) + 1) - 1))
			{
				rebuildWavetableSet(1);
				rebuildWavetableSet(2);
				rebuildWavetableSet(3);
			}
			break;
		case n_1_perc:
		case n_2_perc:
		case n_3_perc:
		case n_perc:
		case n_percvol:
		case n_percfade:
		{
			int v = (int)(my_p[n_perc] * 10);
			float pmult;
			if (v < 1)
				pmult = 0;
			else if (v < 2)
				pmult = 1;
			else if (v < 3)
				pmult = 2;
			else if (v < 4)
				pmult = 3;
			else if (v < 5)
				pmult = 4;
			else if (v < 6)
				pmult = 6;
			else if (v < 7)
				pmult = 8;
			else if (v < 8)
				pmult = 10;
			else if (v < 9)
				pmult = 12;
			else
				pmult = 16;

			n1->set_percussion(1.5f * my_p[n_percvol], pmult, my_p[n_percfade]);
		}
		break;
		case n_click:
			rebuildClickWavetable();
			break;
		case n_vol1:
			n1->set_volume(value * .3f, 0);
			break;
		case n_vol2:
			n1->set_volume(value * .4f, 1);
			break;
		case n_vol3:
			n1->set_volume(value * .6f, 2);
			break;
		case n_mono:
			if (value != mono_before)
			{
				if (value >= .5f)
					n1->set_numofvoices(1);
				else
					n1->set_numofvoices(NUMOFVOICES);

				n1->set_volume(my_p[n_vol1] * .3f, 0);
				n1->set_volume(my_p[n_vol2] * .3f, 1);
				n1->set_volume(my_p[n_vol3] * .6f, 2);
			}
			mono_before = value;
			break;
		case n_1_vibrato:
			vibchanged1 = true;
			break;
		case n_1_vmix:
			if (my_p[n_1_vibrato] == 1)
			{
				vmix1 = value;
				vibchanged1 = true;
			}
			break;
		case n_2_vibrato:
			vibchanged2 = true;
			break;
		case n_2_vmix:
			if (my_p[n_2_vibrato] == 1)
			{
				vmix2 = value;
				vibchanged2 = true;
			}
			break;
		case n_drive:
			if (value > 0)
				do_dist = true;
			else
				do_dist = false;
			dist = 2 * (0.1f + value);
			sin_dist = sinf(dist);
			i_dist = 1 / dist;
			dist4 = 4 * dist;
			dist8 = 8 * dist;
			break;
		case n_mrvalve:
			odchanged = true;
			break;
		case n_mix:
			odmix = value;
			if (my_p[n_mrvalve] == 1)
				odchanged = true;
			break;
		case n_tone:
			fuzz_filt.setparam(800 + value * 3000, .7f, samplerate);
			break;
		case n_speed:
			if (value > .5f)
				fastmode = true;
			else
				fastmode = false;
			break;
		case n_l_slow:
			lslow = value * 10;
			break;
		case n_l_fast:
			lfast = value * 10;
			break;
		case n_u_slow:
			uslow = value * 10;
			break;
		case n_u_fast:
			ufast = value * 10;
			break;
		case n_belt:
			ubelt_up = (value * 3 + 1) * .012f;
			ubelt_down = (value * 3 + 1) * .008f;
			lbelt_up = (value * 3 + 1) * .0045f;
			lbelt_down = (value * 3 + 1) * .0035f;
			break;
		case n_spread:
			lfos_ok = false;
			break;
		case n_splitpoint:
			splitpoint = (long)(value * 128);
			break;
		}

		if (editor)
			((AEffGUIEditor*)editor)->setParameter(index, value);
	}
}

void effectMain::setParameter(VstInt32 index, float value)
{
	/*
		If you implement a plugin as suggested in the SDK you
		will have the following side effect:
		An automated or manually changed parameter will always
		be written into the actual program. The original program
		is lost. To prevent this we use several program buffers
		and let the plugin decide wether the data will be
		saved or not. This is the desired behaviour, similar
		to all those hardware expanders.

		A new feature is delayed program change. When the host runs
		several setProgram calls shortly after another we don't want
		to perform real program and parameter changes. We store the
		new program name in next_curProgram and let virtual_my_p point
		to the according program memory. From setProgram we set
		setProgramStep=sps_requested. The process routine sees this,
		sets setProgramStep=sps_waiting, counts for a while, sets
		setProgramStep=sps_acknowledged and	runs a setProgram(next_curProgram).
		Now setProgram and setParameter can decide whether to ignore
		the change (if next_curProgram==curProgram) or to perform
		a real, full program change with all parameter changes.
		setProgramStep will be reset to sps_idle.

		ap		program memory
		p		edit buffer
		i_p		buffer to save edit buffer (for compare function)
		my_p	points to any buffer. The "machine" always uses my_p.
	*/

	/*
		UPDATE:
		Due to incompatibilities with several hosts we make the whole feature
		compile-time switch dependent. We use delayed program change only if
		DELAYED_PARAMS is defined.
	*/

	vstProgram* ap = &programs[curProgram];
	my_p = p;					// let machine use program memory

// ...no "save" button without editor, so no delayed parameter changes...
	if (!editor)
		param_is_manual = true;

	if (index == n_mute)
	{
		if (value > 0)
			mute = true;
		else
		{
			// This is one of the two methods to finally activate everything.
			// We need some time in advance in order to fill all buffers with correct data.
			// The other method is located at processReplacing.
			mute = false;
			fullyloaded = true;
		}
	}

	if (index == n_param_is_manual)
	{
		if (value > .5f)				// we have a manual parameter change
		{
			param_is_manual = true;
		}
		else
			param_is_manual = false;
	}
	else if (index == n_1_midi || index == n_2_midi || index == n_3_midi)
	{
		if (editor)
			((AEffGUIEditor*)editor)->setParameter(index, value);
	}
	else if (index == n_split)
	{
		if (value == 1)
			waitforsplit = true;
		else
		{
			waitforsplit = false;
			splitpoint = 0;
		}
	}
	else if (index >= 0 && index < kNumParams)
	{
#ifdef DELAYED_PARAMS
		if (!param_is_manual)			// not manual: bank or patch load
#endif
			ap->p[index] = value;			// copy value into program memory

		setInternalParameter(index, value);
	}
#ifdef DELAYED_PARAMS
	else if (index == n_compare)
	{
		if (value == 0)					// compare button released
		{
			for (int i = 0; i < kNumParams; i++)
				p[i] = i_p[i];			// recall edit buffer
			compare = false;
			my_p = p;						// let machine use edit buffer
		}
		else							// compare button pressed
		{
			for (int i = 0; i < kNumParams; i++)
				i_p[i] = p[i];		// save edit buffer
			compare = true;
			my_p = ap->p;					// let machine use program memory
		}

		for (int i = 0; i < kNumParams; i++)
			my_setParameter(i, my_p[i]);	// let everyone know the new values
	}
	else if (index == n_save)				// save button pressed
	{
		for (int i = 0; i < kNumParams; i++)
		{
			if (compare)
			{
				ap->p[i] = i_p[i];		// copy compare buffer params to program memory
			}
			else
			{
				ap->p[i] = my_p[i];		// copy current machine params to program memory
				i_p[i] = my_p[i];			// copy current machine params to compare buffer
										// We need this since there will be a setParameter
										// to n_compare with value 0 which will write the
										// compare buffer back to the edit buffer.
			}
		}
	}
	else if (index == n_display)
#else
	if (index == n_display)
#endif
	{
		if (value < 0)
			value = 0;
		if (value > kNumPrograms - 1)
			value = kNumPrograms - 1;
		if ((long)value != getProgram())
			setProgram((long)value);
	}
}

float effectMain::getParameter(VstInt32 index)
{
	gp_value = 0;
	if (index >= 0 && index < kNumParams)
		gp_value = virtual_my_p[index];

	if (index == n_output)
	{
		gp_value = fabsf(last_out1) * 7;
		if (gp_value < 0 || mono == 0)
			gp_value = 0;
		if (gp_value > 1)
			gp_value = 1;
	}

	return gp_value;
}
