#include "effectMain.h"

VstInt32 effectMain::processEvents(VstEvents* ev)
{
	/*
	Fairly linear stuff. Differing from the SDK example:
	The SDK	does not guarantee that all events arrive in correct order.
	It's possible that a weird VST host sends a note off with a delta
	of 500 followed by a note on with a delta of 100. If we process
	the events in the order they arrive we would have a "hanging note"
	with this example. So we want to do our own event handling and make
	it sample accurate. Sample accuracy can only be provided inside the
	process/processReplacing function. Now you know why. I have spoken.
		*/
	if (mute)
		return(0);
	for (long i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;
		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
		char* midiData = event->midiData;

		long status = midiData[0] & 0xf0;		// strip channel

		float channel = (float)(midiData[0] & 0x0f);		// strip command
		if (channel > 2)
			channel = 0;

		if (status == 0x90 || status == 0x80)	// notes
		{
			long note = midiData[1] & 0x7f;
			long velocity = midiData[2] & 0x7f;
			if (status == 0x80)
				velocity = 0;
			if (!velocity || status == 0x80)
			{
				if (splitpoint > 0 && channel == 0 && note <= splitpoint)
					channel = 2;
				add_event(evt_noteoff, event->deltaFrames, (float)note, (float)velocity, channel);
			}
			else
			{
				if (waitforsplit && note > 0)
				{
					splitpoint = note;
					setParameterAutomated(n_splitpoint, float(note) / 128.0f);
					if (editor)
						((AEffGUIEditor*)editor)->setParameter(n_split, float(note) / 128.0f);
					waitforsplit = false;
				}
				else
				{
					if (splitpoint > 0 && channel == 0 && note <= splitpoint)
						channel = 2;
					add_event(evt_noteon, event->deltaFrames, (float)note, (float)velocity, channel);
				}
			}
		}
		else if (status == 0xb0 && (midiData[1] == 0x78
			|| midiData[1] == 0x79
			|| midiData[1] == 0x7a
			|| midiData[1] == 0x7b
			|| midiData[1] == 0x7c
			|| midiData[1] == 0x7d
			|| midiData[1] == 0x7e
			|| midiData[1] == 0x7f))	// all notes off
		{
			add_event(evt_alloff, event->deltaFrames, 0, 0, -1);
		}
		else if (status == 0xb0 && midiData[1] == 0x07)	// main volume
		{
			add_event(evt_volume, event->deltaFrames, (float)midiData[2] / 128, 0, -1);
		}
		else if (status == 0xb0 && midiData[1] == 0x40)	// footswitch
		{
			float	mp = 0;
			if (midiData[2] > 64)
				mp = 127;
			if (my_p[n_pedalspeed] < .5f)
			{
				add_event(evt_pedal, event->deltaFrames, mp, 0, channel);
			}
			else
			{
				add_event(evt_modulation, event->deltaFrames, mp, 0, -1);
			}
		}
#ifndef DEVELOP
		else if (status == 0xb0 && midiData[1] > 0x4a && midiData[1] < 0x54) // drawbar controllers
		{
			add_event(evt_drawbar, event->deltaFrames, (float)(midiData[1] - 0x4b), (float)midiData[2], channel);
		}
#endif
		else if (status == 0xb0 && midiData[1] == 0x03) // channel volume
		{
			add_event(evt_channel_volume, event->deltaFrames, (float)midiData[2], 0, channel);
		}
		else if (status == 0xb0 && midiData[1] == 0x01)	// Modulation wheel
		{
			// We had problems with many hosts:
			// Modulation was set to 0 during song start
			// So we don't use modulation as volume control anymore
			/*
			if(my_p[n_pedalspeed]<.5f)
			{
			*/
			if (midiData[2] < 63)
				add_event(evt_modulation, event->deltaFrames, 0, 0, -1);
			else
				add_event(evt_modulation, event->deltaFrames, 1, 0, -1);
			/*
			}
			else
			add_event(evt_volume,event->deltaFrames,(float)midiData[2]/128,0,-1);
		*/
		}
		else if (status == 0xc0)	// Program Change
		{
			add_event(evt_progchange, event->deltaFrames, midiData[1], 0, -1);
		}
#ifdef DEVELOP
		else if (status == 0xb0 && midiData[1] == 0x4a) // Ozone Ctrl 1
		{
			char t[100];
			ctrl1 = ((float)midiData[2]) / 127;
			sprintf(t, "ctrl1=%f ctrl2=%f ctrl3=%f ctrl4=%f\n", ctrl1, ctrl2, ctrl3, ctrl4);
			OutputDebugString(t);
		}
		else if (status == 0xb0 && midiData[1] == 0x47) // Ozone Ctrl 2
		{
			char t[100];
			ctrl2 = ((float)midiData[2]) / 127;
			sprintf(t, "ctrl1=%f ctrl2=%f ctrl3=%f ctrl4=%f\n", ctrl1, ctrl2, ctrl3, ctrl4);
			OutputDebugString(t);
		}
		else if (status == 0xb0 && midiData[1] == 0x51) // Ozone Ctrl 3
		{
			char t[100];
			ctrl3 = ((float)midiData[2]) / 127;
			sprintf(t, "ctrl1=%f ctrl2=%f ctrl3=%f ctrl4=%f\n", ctrl1, ctrl2, ctrl3, ctrl4);
			OutputDebugString(t);
		}
		else if (status == 0xb0 && midiData[1] == 0x5b) // Ozone Ctrl 4
		{
			char t[100];
			ctrl4 = ((float)midiData[2]) / 127;
			sprintf(t, "ctrl1=%f ctrl2=%f ctrl3=%f ctrl4=%f\n", ctrl1, ctrl2, ctrl3, ctrl4);
			OutputDebugString(t);
		}
#endif
		else if (status == 0xe0)	// pitch bend
		{
			float pitch = (float)(midiData[2] * 128 + midiData[1]);

			if (pitch > 8192 + 600)
			{
				float p = pitch / 8192 - 1;
				pitch = p * (float)pow(1.059463094359, int(12 * my_p[n_bender])) + 1 - p;
			}
			else if (pitch < 8192 - 600)
			{
				float p = (8192 - pitch) / 8192;
				pitch = 1 / (p * (float)pow(1.059463094359, int(12 * my_p[n_bender])) + 1 - p);
			}
			else
				pitch = 1;

			add_event(evt_pitch, event->deltaFrames, pitch, 0, channel);
		}

		event++;
	}
	return 1;		// we want more events
}

void effectMain::add_event(unsigned char type, long deltaframes, float value1, float value2, float value3)
{
	// a valid buffer entry is marked by a delta value >=0
	// to add an event we enter the values and set the pointer to the
	// next position which has to be empty

	has_events = true;

	// get last event
	event_last_pos = event_pos - 1;
	if (event_last_pos < 0)
		event_last_pos += EVTBUFSIZE;

	event_next_pos = event_pos + 1;
	if (event_next_pos >= EVTBUFSIZE)
		event_next_pos -= EVTBUFSIZE;

	if (delta[event_next_pos] >= 0)	// event buffer is full
		return;


	if (delta[event_last_pos] >= 0)	// there is an event before this one
	{
		if (deltaframes < delta[event_last_pos]) // error: event not in sequence
			delta[event_pos] = delta[event_last_pos] - deltaframes;	// delta difference
		else
			delta[event_pos] = deltaframes - delta[event_last_pos];	// delta difference
	}
	else
		delta[event_pos] = deltaframes;


	events[event_pos] = type;
	evalue1[event_pos] = value1;
	evalue2[event_pos] = value2;
	evalue3[event_pos] = value3;

	event_pos++;
	if (event_pos >= EVTBUFSIZE)
		event_pos -= EVTBUFSIZE;
}

float* effectMain::event_clock()
{
	if (!has_events)
		return(0);

	if (delta[event_read_pos] < 0)	// there are no events in the buffer
	{
		has_events = false;
		return(NULL);
	}

	delta[event_read_pos]--;	// count delta frames (reset event if delta==0)

	if (delta[event_read_pos] < 0)	// event is now
	{
		values[0] = (float)events[event_read_pos];
		values[1] = evalue1[event_read_pos];
		values[2] = evalue2[event_read_pos];
		values[3] = evalue3[event_read_pos];
		ret = values;
		event_read_pos++;
		if (event_read_pos >= EVTBUFSIZE)
			event_read_pos -= EVTBUFSIZE;
	}
	else
		ret = NULL;				// no event yet

	return(ret);
}
