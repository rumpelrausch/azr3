#pragma once
#include "customControl.h"
#include "CAbstractVertical.h"

class CCircDisplay :
	public customControl,
	public CAbstractVertical
{
public:
	CCircDisplay(const CRect& size, CControlListener* listener, int tag, CBitmap* bitmap, CBitmap* digits, CBitmap* background);
	//void setStringConvert(void (*convert) (float value, char* string));
	void drawControl(CDrawContext* pContext);
protected:
	//void (*stringConvert)(float value, char* string);
	//void (*parameterConversions::*stringConvert) (float value, char* string);
	int	numOfSpritePhases, digitWidth, digitHeight;
	CBitmap* digitBitmap;
};
