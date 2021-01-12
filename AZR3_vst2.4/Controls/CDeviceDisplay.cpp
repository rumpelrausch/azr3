#include "CDeviceDisplay.h"

CDeviceDisplay::CDeviceDisplay(const CRect& size, CControlListener* listener, long tag, CBitmap* background) : CControl(size, listener, tag, background)
{
	my_tag = tag;
	top = size.top;
	bottom = size.bottom;
	left = size.left;
	right = size.right;
	first = 1;
	line1[0] = 0;
	line2[0] = 0;
	line3[0] = 0;
	line4[0] = 0;
	last_value = -2;
	linevalue = -1;
	my_drawcontext = NULL;
	oc = NULL;
}

void CDeviceDisplay::draw(CDrawContext* pContext)
{
	last_value = value;
	my_drawcontext = pContext;
	if (first == 1)
	{
		oc = new COffscreenContext(this->getFrame(), right - left, bottom - top, kBlackCColor);
		first = 0;
	}
	CRect rect(0, 0, right - left, bottom - top);
	pBackground->draw(oc, rect, CPoint(left, top));
	oc->setFont(kNormalFontVerySmall);
	{
		CColor mc = { 255,236,200,0 };
		oc->setFontColor(mc);
		oc->setLineWidth(1);
		oc->setFrameColor(mc);
		mc.red = 126;
		mc.green = 210;
		mc.blue = 204;
		oc->setFillColor(mc);
	}
	oc->drawString(line1, CRect(2, 0, right - left, 10), false, kLeftText);
	oc->drawString(line2, CRect(2, 10, right - left, 10 + 10), false, kLeftText);
	oc->drawString(line3, CRect(2, 20, right - left, 20 + 10), false, kLeftText);
	oc->drawString(line4, CRect(2, 30, right - left, 30 + 10), false, kLeftText);
	if (linevalue >= 0)
	{
		oc->drawRect(CRect(2, 42, right - left - 2, 48));
		oc->fillRect(CRect(3, 43, 4 + (int)((right - left - 7) * linevalue), 47));
	}
	oc->copyFrom(pContext, CRect(left, top, right, bottom), CPoint(0, 0));
}

void CDeviceDisplay::setline(float value)
{
	linevalue = value;
	this->setDirty();
}

void CDeviceDisplay::setstring(int line, char* string)
{
	if (line == 1)
		strncpy(line1, string, sizeof(line1));
	else if (line == 2)
		strncpy(line2, string, sizeof(line2));
	else if (line == 3)
		strncpy(line3, string, sizeof(line3));
	else if (line == 4)
		strncpy(line4, string, sizeof(line4));
	this->setDirty();
}

void CDeviceDisplay::mouse(CDrawContext* pContext, CPoint& where, long button)
{
	int	x;
	x = where.v;

	beginEdit();
	do
	{
		button = pContext->getMouseButtons();
		getMouseLocation(pContext, where);

		if (abs(where.v - x) > 10)
		{
			if (where.v < x)
			{
				if (value > 0)
				{
					setDirty(true);
					value--;
				}
			}
			else
			{
				setDirty(true);
				value++;
			}
			x = where.v;
		}

		if (isDirty())
			listener->valueChanged(pContext, this);

		doIdleStuff();
	} while (button & kLButton);
	endEdit();
}

CDeviceDisplay::~CDeviceDisplay()
{
	if (oc != NULL)
		delete oc;
}
