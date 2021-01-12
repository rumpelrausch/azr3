#pragma once
#include "CAbstractVertical.h"
class CDrawbar :
    public CAbstractVertical
{
public:
	CDrawbar(const CRect& size, CControlListener* listener, long tag, CBitmap* bitmap, CBitmap* background, int handleHeight);
	void	drawControl(CDrawContext* pContext);
protected:
	int		handleHeight;
	CBitmap* faderBitmap;
};
