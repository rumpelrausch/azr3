#include "effectMain.h"
#include "effectAudioProcessing.h"

// TODO: put into class
//extern void convert_bendrange(float value, char* string);
//extern void convert_tone(float value, char* string);
//extern void convert_speed(float value, char* string);
//extern void convert_perc(float value, char* string);
//extern void convert_shape(float value, char* string);

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new effectMain(audioMaster);
}

effectMain::effectMain(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, kNumPrograms, kNumParams)
{
	fullyloaded = false;
	mute = true;

	if (audioMaster)
	{
		setNumInputs(2);
		setNumOutputs(kNumOutputs);
		canDoubleReplacing(false);
		canProcessReplacing(true);
		isSynth(true);
		this->
			noTail(false);
		setUniqueID(MY_ID);
	}

	initall();

	n1 = new noteController(NUMOFVOICES);
	vdelay1 = new fxDelay(441, true);
	vdelay2 = new fxDelay(441, true);
	wand_r = new fxDelay(4410, false);
	wand_l = new fxDelay(4410, false);
	delay1 = new fxDelay(4410, true);
	delay2 = new fxDelay(4410, true);
	delay3 = new fxDelay(4410, true);
	delay4 = new fxDelay(4410, true);

	my_p = virtual_my_p = programs[0].p;
	setProgramDelayCount = 0;
	setSampleRate((float)updateSampleRate());
	setFactorySounds();
	actual_curProgram = -1;
	setProgram(0, true);

	buildTonewheelSet(TONEWHEEL_SHAPE_SINE);

	editor = NULL;
	editor = new Editor(this);

	suspend();
	param_is_manual = true;
}

effectMain::~effectMain()
{
	if (n1 != NULL)
		n1->~noteController();
	n1 = NULL;
	if (wand_r != NULL)
		wand_r->~fxDelay();
	wand_r = NULL;
	if (wand_l != NULL)
		wand_l->~fxDelay();
	wand_l = NULL;
	if (vdelay1 != NULL)
		vdelay1->~fxDelay();
	vdelay1 = NULL;
	if (vdelay2 != NULL)
		vdelay2->~fxDelay();
	vdelay2 = NULL;
	if (delay1 != NULL)
		delay1->~fxDelay();
	delay1 = NULL;
	if (delay2 != NULL)
		delay2->~fxDelay();
	delay2 = NULL;
	if (delay3 != NULL)
		delay3->~fxDelay();
	delay3 = NULL;
	if (delay4 != NULL)
		delay4->~fxDelay();
	delay4 = NULL;
	if (editor != NULL)
		editor->~AEffEditor();
	editor = NULL;
}

/*
//	Don't set variable parameters here:
//	The function might get called after program select
//	so the program values would get overriden.
*/
void effectMain::setSampleRate(float sampleRate)
{
	AudioEffectX::setSampleRate(sampleRate);
	samplerate = sampleRate;

	warmth.setparam(2700, 1.2f, sampleRate);

	if (n1 != NULL)
		n1->set_samplerate(samplerate);
	if (vdelay1 != NULL)
		vdelay1->set_samplerate(samplerate);
	if (vdelay2 != NULL)
		vdelay2->set_samplerate(samplerate);
	vlfo.set_samplerate(samplerate);
	vlfo.set_rate(35, 0);
	split.setparam(400, 1.3f, samplerate);
	horn_filt.setparam(2500, .5f, samplerate);
	//horn_filt.setparam(4500, 0.02f, samplerate);
	damp.setparam(200, .9f, samplerate);
	//damp.setparam(3500, 1.5f, samplerate);
	if (wand_r != NULL)
	{
		wand_r->set_samplerate(samplerate);
		wand_r->set_delay(35);
	}
	if (wand_r != NULL)
	{
		wand_l->set_samplerate(samplerate);
		wand_l->set_delay(20);
	}
	if (delay1 != NULL)
		delay1->set_samplerate(samplerate);
	if (delay2 != NULL)
		delay2->set_samplerate(samplerate);
	if (delay3 != NULL)
		delay3->set_samplerate(samplerate);
	if (delay4 != NULL)
		delay4->set_samplerate(samplerate);
	lfo1.set_samplerate(samplerate);
	lfo2.set_samplerate(samplerate);
	lfo3.set_samplerate(samplerate);
	lfo4.set_samplerate(samplerate);

	body_filt.setparam(190, 1.5f, samplerate);
	postbody_filt.setparam(1100, 1.5f, samplerate);
}

bool effectMain::getOutputProperties(VstInt32 index, VstPinProperties* properties)
{
	if (index < kNumOutputs)
	{
		/*
			We could add "L" and "R" for the channels, but this
			is not recognized by all hosts. This is the most compatible
			solution.
		*/
		sprintf(properties->label, "AZR3");
		sprintf(properties->shortLabel, "AZR3");
		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;
		return true;
	}
	return false;
}

//bool effectMain::getInputProperties(VstInt32 index, VstPinProperties* properties)
//{
//	if (index < 2)
//	{
//		properties->flags = kVstPinIsActive | kVstPinIsStereo;
//		sprintf(properties->label, "AZR3 Input");
//		sprintf(properties->shortLabel, "AZR3 IN");
//	}
//	return true;
//}

bool effectMain::getEffectName(char* name)
{
	strcpy(name, EFFECT_NAME);
	return true;
}

bool effectMain::getVendorString(char* text)
{
	strcpy(text, VENDOR_STRING);
	return true;
}

bool effectMain::getProductString(char* text)
{
	strcpy(text, PRODUCT_STRING);
	return true;
}

VstInt32 effectMain::canDo(char* text)
{
	if (!strcmp(text, "receiveVstEvents"))
		return 1;
	else if (!strcmp(text, "receiveVstMidiEvent"))
		return 1;
	else if (!strcmp(text, "sendVstMidiEvent"))
		return 1;
	else
		return -1;
}

