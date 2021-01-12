#pragma once
#include "Globals.h"

class vstProgram
{
	friend class effectMain;
	private:
		float p[kNumParams];
		char name[24];
};
