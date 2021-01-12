#include "CTabButton.h"

CTabButton::CTabButton(CRect& size, CControlListener* listener, int tag, CBitmap* bm) : CControl(size, listener, tag, bm)
{
}

CTabButton::~CTabButton()
{
}

void CTabButton::draw(CDrawContext* pContext)
{
}

void CTabButton::mouse(CDrawContext* pContext, CPoint& where, long button)
{

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

	if (button)
		((AEffGUIEditor*)this->getEditor())->setParameter(tag, 1);
}
