#include "CAbstractVertical.h"
#include "../Globals.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

CAbstractVertical::CAbstractVertical(const CRect& size, CControlListener* listener, long tag, CBitmap* bitmap, CBitmap* background)
	: CControl(size, listener, tag, background),
	bitmapControl(bitmap),
	firstRun(true),
	offscreenContext(NULL),
	lastValue(-1),
	directionUp(true)
{
	controlSize(size.left, size.top, size.right, size.bottom);
	width = bitmap->getWidth();
	height = bitmap->getHeight();

	controlDimensions = { width, height };
}

void CAbstractVertical::draw(CDrawContext* pContext)
{
	if (firstRun)
	{
		offscreenContext = new COffscreenContext(this->getFrame(), controlDimensions.width, controlDimensions.height, kBlackCColor);
		firstRun = false;
	}

	drawControl(pContext);

	lastValue = value;
}

void CAbstractVertical::mouse(CDrawContext* pContext, CPoint& where, long button)
{
	int	x, x_start;
	float	range1 = .01f;
	float	range2 = .001f;
	float	range;
	static bool	pressed = false;

	if (!bMouseEnabled)
		return;

	if (button == -1) button = pContext->getMouseButtons();

	if (listener && button & (kAlt | kShift | kControl | kApple))
	{
		if (listener->controlModifierClicked(pContext, this, button) != 0)
			return;
	}

	if (!(button & kLButton))
		return;

	x_start = x = where.v;
	beginEdit();
	do
	{
		button = pContext->getMouseButtons();
		if ((button & kLButton) && !pressed)
		{
			pressed = true;
			((AEffGUIEditor*)this->getEditor())->setParameter(n_mousedown, (float)tag);
		}
		else if (!(button & kLButton) && pressed)
		{
			pressed = false;
			((AEffGUIEditor*)this->getEditor())->setParameter(n_mousedown, -1);
		}

		if (button & kShift)
			range = range2;
		else
			range = range1;

		getMouseLocation(pContext, where);
		if (where.v != x)
		{
			x = x_start - where.v;

			if(directionUp)
				value += range * (float)x;
			else
				value -= range * (float)x;

			if (value < 0.01f)
				value = 0;
			else if (value > 1)
				value = 1;
			x = x_start = where.v;
			//l_value = value;
		}

		bounceValue();

		if (isDirty())
			listener->valueChanged(pContext, this);

		doIdleStuff();
	} while (button & kLButton);
	endEdit();
}

void CAbstractVertical::drawControl(CDrawContext* pContext)
{
}

void CAbstractVertical::setControlDimensions(int width, int height)
{
	controlDimensions = { width, height };
}

void CAbstractVertical::setReverseMouseDirection()
{
	directionUp = false;
}

CAbstractVertical::~CAbstractVertical()
{
	if (offscreenContext != NULL)
	{
		delete offscreenContext;
	}
}
