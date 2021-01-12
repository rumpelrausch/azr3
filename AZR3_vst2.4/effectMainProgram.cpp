#include "effectMain.h"

void effectMain::setProgram(VstInt32 program)
{
	//	this->setProgram(program,false);
	this->setProgram(program, true);
}

void effectMain::setProgram(VstInt32 program, bool force)
{
	long x;

	if (force)
		actual_curProgram = -2;

	vstProgram* ap = &programs[program];

	// prepare for delayed program change
	virtual_my_p = ap->p;
	curProgram = program;

#ifndef DELAYED_PARAMS
	setProgramDelayCount = -1;
#endif

	// has the program change fxDelay elapsed?
	if (setProgramDelayCount == -1)
	{
		curProgram = program;
		virtual_my_p = my_p;
	}

	if (actual_curProgram != program)
	{
		for (x = 0; x < kNumParams; x++)
		{
#ifdef DELAYED_PARAMS
			param_is_manual = true;
#endif
			setParameter(x, ap->p[x]);
		}
	}

	// set parameter display only after program change fxDelay has elapsed
	// this must occur after the setParameter calls
#ifdef DELAYED_PARAMS
	if (setProgramDelayCount == -1 || param_is_manual)
	{
		setProgramDelayCount = 0;
#endif
		if (editor)
			((AEffGUIEditor*)editor)->setParameter(n_display, (float)curProgram);
		actual_curProgram = program;
#ifdef DELAYED_PARAMS
	}
	// retrigger fxDelay if we are idle or during fxDelay
	else
	{
		setProgramDelayCount = 1;
	}
#endif

	param_is_manual = false;
}

void effectMain::setProgramName(char* name)
{
	strcpy(programs[curProgram].name, name);
	if (editor)
		((AEffGUIEditor*)editor)->setParameter(n_display, (float)curProgram);
}

void effectMain::getProgramName(char* name)
{
	strcpy(name, programs[curProgram].name);
}

bool effectMain::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy(text, programs[index].name);
		return true;
	}
	return false;
}

bool effectMain::copyProgram(VstInt32 destination)
{
	if (destination < kNumPrograms)
	{
		programs[destination] = programs[curProgram];
		return true;
	}
	return false;
}

VstInt32 effectMain::getProgram()
{
	return(curProgram);
}
