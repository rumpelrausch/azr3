#pragma once
#include "../Globals.h"
#include <vstgui.h>

class CTabButton : public CControl
{
public:
	CTabButton(CRect& size, CControlListener* listener, int tag, CBitmap* bm);
	~CTabButton();
	void draw(CDrawContext* pContext);
	void mouse(CDrawContext* pContext, CPoint& where, long button);
};
