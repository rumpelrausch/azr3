#include "CCircDisplay.h"
#include <stdio.h>

CCircDisplay::CCircDisplay(const CRect& size, CControlListener* listener, int tag, CBitmap* bitmap, CBitmap* digits, CBitmap* background)
	: CAbstractVertical(size, listener, tag, bitmap, background),
	digitBitmap(digits)
{
	numOfSpritePhases = int(height / width);
	digitWidth = digits->getWidth();
	digitHeight = digits->getHeight() / 12;
}

void CCircDisplay::drawControl(CDrawContext* pContext)
{
	unsigned int		index;
	int					y = 0, z;

	if (stringConvert)
		stringConvert(value, caption);
	else
		sprintf(caption, "%3d%", int(100 * value + .001f));

	CRect rect(0, 0, width, width);
	pBackground->draw(offscreenContext, rect, CPoint(controlSize.left, controlSize.top));
	bitmapControl->drawTransparent(offscreenContext, rect, CPoint(0, int(value * (numOfSpritePhases - 1)) * width));

	for (index = 0; index < strlen(caption); index++)
	{
		if (caption[index] == '.')
		{
			int offsetX = 0;
			if (y >= digitWidth)
				offsetX = y - digitWidth;
			rect(12 + offsetX, 19, 12 + offsetX + digitWidth, 19 + digitHeight);
			digitBitmap->drawTransparent(offscreenContext, rect, CPoint(0, 10 * digitHeight));
			continue;
		}
		else if (caption[index] == ' ')
		{
			y += digitWidth;
			continue;
		}
		else if (caption[index] == '%')
			z = 11 * digitHeight;
		else if (!isdigit(caption[index]))
			continue;
		else
			z = digitHeight * (caption[index] - '0');

		rect(12 + y, 18, 12 + y + digitWidth, 18 + digitHeight);
		digitBitmap->drawTransparent(offscreenContext, rect, CPoint(0, z));
		y += digitWidth;
	}
	offscreenContext->copyFrom(pContext, CRect(controlSize.left, controlSize.top, controlSize.left + width, controlSize.top + width), CPoint(0, 0));
}
