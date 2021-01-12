#include "CDrawbar.h"

CDrawbar::CDrawbar(const CRect& size, CControlListener* listener, long tag, CBitmap* bitmap, CBitmap* background, int handleHeight)
	: CAbstractVertical(size, listener, tag, bitmap, background),
	faderBitmap(bitmap),
	handleHeight(handleHeight)
{
	setControlDimensions(controlSize.right - controlSize.left, controlSize.bottom - controlSize.top);
	setReverseMouseDirection();
}

void CDrawbar::drawControl(CDrawContext* pContext)
{
	int	pos = int((1 - value) * (height - handleHeight));
	
	CRect rect(0, 0, width, height);
	faderBitmap->draw(offscreenContext, rect, CPoint(0, pos));
	rect(0, height - pos, width, height);
	pBackground->draw(offscreenContext, rect, CPoint(controlSize.left, controlSize.bottom - pos));
	rect(controlSize.left, controlSize.top, controlSize.right, controlSize.bottom);
	offscreenContext->copyFrom(pContext, rect, CPoint(0, 0));
}
