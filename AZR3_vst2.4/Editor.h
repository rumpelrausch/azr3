#pragma once
#include "audioeffectx.h"
#include "aeffeditor.h"

#include "Globals.h"
#include "parameterConversions.h"
#include "Controls.h"
#include "Controls/CCircDisplay.h"
#include "Controls/CDrawbar.h"
#include "Controls/CDeviceDisplay.h"
#include "Controls/CTabButton.h"

#include "vstgui.h"

enum
{
	cCOnOffButton=0,
	cCParamDisplay, 
	cCTextEdit,
	cCOptionMenu, 
	cCKnob,
	cCAnimKnob,
	cCVerticalSwitch,
	cCHorizontalSwitch,
	cCRockerSwitch,
	cCMovieBitmap,
	cCMovieButton,
	cCAutoAnimation,
	cCVerticalSlider,
	cCHorizontalSlider,
	cCSpecialDigit,
	cCKickButton,
	cCSplashScreen,
	cCVuMeter,
	cCFileSelector,
	cCDisplay,
	cCFader,
	cCCircDisplay,
	cCTab,
	cCTextSplash,

};


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Editor : public AEffGUIEditor, public CControlListener  
{
public:
	Editor (AudioEffect *effect);
	virtual ~Editor ();
	virtual void idle();
protected:
	virtual bool open (void *ptr);
	virtual void close ();
	virtual void setParameter (VstInt32 index, float value);
	virtual void valueChanged (CDrawContext* context, CControl* control);
	void c_visible(long tag,bool show);

private:
	int				panelmode;
	long			m1,m2,m3;
	long			oldTicks,ticks;
	int				panel_width,panel_height;
	bool			justloaded,frame_ok;
	int				mousedown;

	CBitmap*		bitmaps[kNumBitmaps];
	CControl*		controls[kNumControls];
	int				ctyp[kNumControls];
	CDeviceDisplay*		ctrl_display;
	COnOffButton*	ctrl_compare;
	COnOffButton*	ctrl_split;
	CKickButton*	ctrl_save;
	CSplashScreen*	ctrl_splash;
};
