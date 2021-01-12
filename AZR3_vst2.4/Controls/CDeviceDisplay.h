#pragma once
#include "../Globals.h"
#include <vstgui.h>

class CDeviceDisplay : public CControl
{
public:
	CDeviceDisplay(const CRect& size, CControlListener* listener, long tag, CBitmap* background);
	~CDeviceDisplay();
	void	draw(CDrawContext* pContext);
	void	mouse(CDrawContext* pContext, CPoint& where, long button);
	void	setstring(int line, char* string);
	void	setline(float value);
protected:
	float	last_value;
	int		top, left, bottom, right, first, my_tag;
	COffscreenContext* oc;
	CDrawContext* my_drawcontext;
	char	line1[256], line2[256], line3[256], line4[256];
	float	linevalue;
};
