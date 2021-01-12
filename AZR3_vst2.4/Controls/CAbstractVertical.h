#pragma once
#include "../Globals.h"
#include <vstgui.h>

class CAbstractVertical : public CControl
{
public:
	CAbstractVertical(const CRect& size, CControlListener* listener, long tag, CBitmap* bitmap, CBitmap* background);
	~CAbstractVertical();
	void draw(CDrawContext* pContext);
	void mouse(CDrawContext* pContext, CPoint& where, long button);

	virtual void drawControl(CDrawContext* pContext);
protected:
	float lastValue;
	bool firstRun, directionUp;
	int width, height;
	CRect controlSize;
	COffscreenContext* offscreenContext;
	CBitmap* bitmapControl;
	struct whTuple
	{
		int width;
		int height;
	} controlDimensions;

	void setControlDimensions(int width, int height);
	void setReverseMouseDirection();
};
