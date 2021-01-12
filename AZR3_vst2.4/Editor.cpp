#include <vstgui.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Editor.h"
#include "noteController.h"

#if WIN32
#include "resource.h"
#else
enum
{
	IDB_cknob = 128,
	IDB_vonoff,
	IDB_dbbrown,
	IDB_dbwhite,
	IDB_inside,
	IDB_miniledred,
	IDB_minioffon,
	IDB_yellow,
	IDB_onoffgreen,
	IDB_dbblack,
	IDB_panelfx,
	IDB_panelvoice,
	IDB_vu
};
#endif


Editor::~Editor()
{
}


Editor::Editor(AudioEffect* effect) : AEffGUIEditor(effect)
{
	m1 = 0, m2 = 0, m3 = 0;
	ticks = 0;
	frame = 0;
	frame_ok = false;
	rect.left = 0;
	rect.top = 0;
	rect.right = 709;
	rect.bottom = 448;
	justloaded = true;
	//effect->setEditor (this);
}

void Editor::idle()
{
	if (ctrl_splash->getValue())
		return;
	if (!justloaded && getTicks() - oldTicks < 200)
		return;
	if (!controls[n_1_midi] || !controls[n_2_midi] || !controls[n_3_midi]
		|| !ctrl_display)
		return;

	if (justloaded && ctrl_display)
	{
		if (ticks < 100)
		{
			controls[n_1_midi]->setValue(1);
			controls[n_2_midi]->setValue(1);
			controls[n_3_midi]->setValue(1);
			ctrl_display->setstring(1, "=== Rumpelrausch Täips ===");
			ctrl_display->setstring(2, "Loading Rumpel/OS V3");
			ctrl_display->setstring(3, "");
		}
		else if (ticks > 1000)
		{
			effect->setParameter(n_mute, 0);
			ctrl_display->setline(-1);
			ctrl_display->setstring(1, "===    PLEASE DONATE   ===");
			ctrl_display->setstring(2, "");
			ctrl_display->setstring(3, "have fun");
			ctrl_display->setstring(4, "   flp");
		}
		else if (ticks <= 1000)
		{
			ctrl_display->setline((float)ticks / 1000);
		}

		if (ticks > 3500)
		{
			setParameter(n_display, -1);
			justloaded = false;
		}
		ctrl_display->setDirty(true);
	}
	else
	{
		long	t = getTicks();
		if (m1 < 0)
		{
			controls[n_1_midi]->setValue(1);
			m1 = t;
		}
		if (m2 < 0)
		{
			controls[n_2_midi]->setValue(1);
			m2 = t;
		}
		if (m3 < 0)
		{
			controls[n_3_midi]->setValue(1);
			m3 = t;
		}

		if (t > m1 + 50)
		{
			controls[n_1_midi]->setValue(0);
			m1 = 0;
		}
		if (t > m2 + 50)
		{
			controls[n_2_midi]->setValue(0);
			m2 = 0;
		}
		if (t > m3 + 50)
		{
			controls[n_3_midi]->setValue(0);
			m3 = 0;
		}
	}

	if (controls[n_vu] != NULL)
		controls[n_vu]->setValue(effect->getParameter(n_output));

	ticks = getTicks() - oldTicks;
	AEffGUIEditor::idle();
	if (ctrl_display != NULL)
		ctrl_display->setDirty(true);
}

bool Editor::open(void* ptr)
{
	oldTicks = getTicks();
	panelmode = n_voicemode;
	// panelmode = n_fxmode;
	ticks = 0;
	frame = 0;
	AEffGUIEditor::open(ptr);
	int version = getVstGuiVersion();
	int verMaj = (version & 0xFF00) >> 16;
	int verMin = (version & 0x00FF);
	int	x, c;

	mousedown = -1;

	// initialize control array
	for (x = 0; x < kNumControls; x++)
		controls[x] = NULL;

	CPoint point(0, 0);
	CRect rct(387, 17, 529, 68);

	bitmaps[b_panelvoice] = new CBitmap(IDB_panelvoice);
	int panel_width = bitmaps[b_panelvoice]->getWidth();
	int panel_height = bitmaps[b_panelvoice]->getHeight();
	rct(0, 0, panel_width, panel_height);
	frame = new CFrame(rct, ptr, this);
	frame->setBackground(bitmaps[b_panelvoice]);
	frame->setTransparency(false);

	bitmaps[b_panelfx] = new CBitmap(IDB_panelfx);
	bitmaps[b_cknob] = new CBitmap(IDB_cknob);
	bitmaps[b_dbblack] = new CBitmap(IDB_dbblack);
	bitmaps[b_dbbrown] = new CBitmap(IDB_dbbrown);
	bitmaps[b_dbwhite] = new CBitmap(IDB_dbwhite);
	bitmaps[b_inside] = new CBitmap(IDB_inside);
	bitmaps[b_miniledred] = new CBitmap(IDB_miniledred);
	bitmaps[b_minioffon] = new CBitmap(IDB_minioffon);
	bitmaps[b_onoffgreen] = new CBitmap(IDB_onoffgreen);
	bitmaps[b_vonoff] = new CBitmap(IDB_vonoff);
	bitmaps[b_yellow] = new CBitmap(IDB_yellow);
	bitmaps[b_vu] = new CBitmap(IDB_vu);

	CCircDisplay* ctrl;

	rct(387, 17, 529, 68);
	ctrl_display = new CDeviceDisplay(rct, this, n_display, bitmaps[b_panelvoice]);
	frame->addView(ctrl_display);

#ifdef DELAYED_PARAMS
	rct(537, 19, 537 + 11, 19 + 12);
	ctrl_compare = new COnOffButton(rct, this, n_compare, bitmaps[b_minioffon]);
	frame->addView(ctrl_compare);
#endif

	rct(537, 49, 537 + 11, 49 + 12);
	ctrl_split = new COnOffButton(rct, this, n_split, bitmaps[b_minioffon]);
	frame->addView(ctrl_split);

#ifdef DELAYED_PARAMS
	rct(537, 34, 537 + 11, 34 + 12);
	ctrl_save = new CKickButton(rct, this, n_save, 12, bitmaps[b_minioffon], point);
	frame->addView(ctrl_save);
#endif

	//-------- enumerated controls

	c = n_mono;
	rct(61, 105, 61 + 11, 105 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_click;
	rct(2 * 44, 2 * 44, 2 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_bender;
	rct(3 * 44, 2 * 44, 3 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_bendrange);
	frame->addView(controls[c]);
	{
		CCircDisplay* ctrl1 = (CCircDisplay*)controls[c];
		ctrl1->setStringConvert(parameterConversions::convert_bendrange);
	}
	ctyp[c] = cCCircDisplay;

	c = n_sustain;
	rct(4 * 44, 2 * 44, 4 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_shape;
	rct(5 * 44, 2 * 44, 5 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_shape);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_perc;
	rct(7 * 44, 2 * 44, 7 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_perc);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_percvol;
	rct(8 * 44, 2 * 44, 8 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_percfade;
	rct(9 * 44, 2 * 44, 9 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_vol1;
	rct(11 * 44, 2 * 44, 11 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_vol2;
	rct(12 * 44, 2 * 44, 12 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_vol3;
	rct(13 * 44, 2 * 44, 13 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_master;
	rct(14 * 44, 2 * 44, 14 * 44 + 44, 2 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;



	c = n_1_perc;
	rct(16, 173, 16 + 11, 173 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_1_sustain;
	rct(16, 213, 16 + 11, 213 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_1_midi;
	rct(15, 286, 15 + 14, 286 + 7);
	controls[c] = new CMovieBitmap(rct, this,
		c, 2, 7, bitmaps[b_miniledred], point);
	frame->addView(controls[c]);
	ctyp[c] = cCMovieBitmap;

	c = n_1_db1;
	rct(42, 159, 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db3;
	rct(1 * 24 + 42, 159, 1 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db2;
	rct(2 * 24 + 42, 159, 2 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db4;
	rct(3 * 24 + 42, 159, 3 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db5;
	rct(4 * 24 + 42, 159, 4 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db6;
	rct(5 * 24 + 42, 159, 5 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db7;
	rct(6 * 24 + 42, 159, 6 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db8;
	rct(7 * 24 + 42, 159, 7 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_1_db9;
	rct(8 * 24 + 42, 159, 8 * 24 + 42 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;



	c = n_2_perc;
	rct(279, 173, 279 + 11, 173 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_2_sustain;
	rct(279, 213, 279 + 11, 213 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_2_midi;
	rct(278, 286, 278 + 14, 286 + 7);
	controls[c] = new CMovieBitmap(rct, this,
		c, 2, 7, bitmaps[b_miniledred], point);
	frame->addView(controls[c]);
	ctyp[c] = cCMovieBitmap;

	c = n_2_db1;
	rct(305, 159, 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db3;
	rct(1 * 24 + 305, 159, 1 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db2;
	rct(2 * 24 + 305, 159, 2 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db4;
	rct(3 * 24 + 305, 159, 3 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db5;
	rct(4 * 24 + 305, 159, 4 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db6;
	rct(5 * 24 + 305, 159, 5 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db7;
	rct(6 * 24 + 305, 159, 6 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db8;
	rct(7 * 24 + 305, 159, 7 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_2_db9;
	rct(8 * 24 + 305, 159, 8 * 24 + 305 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;



	c = n_3_perc;
	rct(543, 173, 543 + 11, 173 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_3_sustain;
	rct(543, 213, 543 + 11, 213 + 12);
	controls[c] = new COnOffButton(rct, this, c, bitmaps[b_minioffon]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_3_midi;
	rct(542, 286, 542 + 14, 286 + 7);
	controls[c] = new CMovieBitmap(rct, this,
		c, 2, 7, bitmaps[b_miniledred], point);
	frame->addView(controls[c]);
	ctyp[c] = cCMovieBitmap;

	c = n_3_db1;
	rct(569, 159, 569 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_3_db3;
	rct(1 * 24 + 569, 159, 1 * 24 + 569 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbbrown], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_3_db2;
	rct(2 * 24 + 569, 159, 2 * 24 + 569 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_3_db4;
	rct(3 * 24 + 569, 159, 3 * 24 + 569 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbwhite], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;

	c = n_3_db5;
	rct(4 * 24 + 569, 159, 4 * 24 + 569 + 22, 159 + 150);
	controls[c] = new CDrawbar(rct, this,
		c, bitmaps[b_dbblack], bitmaps[b_panelfx], 31);
	frame->addView(controls[c]);
	ctyp[c] = cCFader;


	//------ mode switches --------------------------------------
	c = n_voicemode;
	rct(15, 322, 28, 362);
	controls[c] = new CTabButton(rct, this,
		c, bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCTab;

	c = n_fxmode;
	rct(15, 368, 28, 410);
	controls[c] = new CTabButton(rct, this,
		c, bitmaps[b_panelfx]);
	frame->addView(controls[c]);
	ctyp[c] = cCTab;


	//------ voiceController params ---------------------------------------

	c = n_1_vibrato;
	rct(39, 332, 39 + 25, 332 + 11);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_onoffgreen]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_1_vstrength;
	rct(2 * 44, 8 * 44, 2 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelvoice]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_1_vmix;
	rct(4 * 44, 8 * 44, 4 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelvoice]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;



	c = n_2_vibrato;
	rct(302, 332, 302 + 25, 332 + 11);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_onoffgreen]);
	frame->addView(controls[c]);
	ctyp[c] = cCOnOffButton;

	c = n_2_vstrength;
	rct(8 * 44, 8 * 44, 8 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelvoice]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;

	c = n_2_vmix;
	rct(10 * 44, 8 * 44, 10 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelvoice]);
	frame->addView(controls[c]);
	ctyp[c] = cCCircDisplay;


	//------ FX params ---------------------------------------

	c = n_mrvalve;
	rct(39, 332, 39 + 25, 332 + 11);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_onoffgreen]);
	ctyp[c] = cCOnOffButton;

	c = n_drive;
	rct(1 * 44, 8 * 44, 1 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctyp[c] = cCCircDisplay;

	c = n_set;
	rct(2 * 44, 8 * 44, 2 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctyp[c] = cCCircDisplay;

	c = n_tone;
	rct(3 * 44, 8 * 44, 3 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_tone);
	ctyp[c] = cCCircDisplay;

	c = n_mix;
	rct(4 * 44, 8 * 44, 4 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctyp[c] = cCCircDisplay;




	c = n_speakers;
	rct(302, 332, 302 + 25, 332 + 11);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_onoffgreen]);
	ctyp[c] = cCOnOffButton;

	c = n_complex;
	rct(444, 331, 444 + 11, 331 + 12);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_minioffon]);
	ctyp[c] = cCOnOffButton;

	c = n_pedalspeed;
	rct(512, 331, 512 + 11, 331 + 12);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_minioffon]);
	ctyp[c] = cCOnOffButton;

	c = n_speed;
	rct(322, 356, 322 + 15, 356 + 28);
	controls[c] = new COnOffButton(rct, this,
		c, bitmaps[b_vonoff]);
	ctyp[c] = cCOnOffButton;



	c = n_l_slow;
	rct(8 * 44, 8 * 44, 8 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_speed);
	ctyp[c] = cCCircDisplay;

	c = n_l_fast;
	rct(9 * 44, 8 * 44, 9 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_speed);
	ctyp[c] = cCCircDisplay;

	c = n_u_slow;
	rct(10 * 44, 8 * 44, 10 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_speed);
	ctyp[c] = cCCircDisplay;

	c = n_u_fast;
	rct(11 * 44, 8 * 44, 11 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctrl = (CCircDisplay*)controls[c];
	ctrl->setStringConvert(parameterConversions::convert_speed);
	ctyp[c] = cCCircDisplay;

	c = n_belt;
	rct(12 * 44, 8 * 44, 12 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctyp[c] = cCCircDisplay;

	c = n_spread;
	rct(13 * 44, 8 * 44, 13 * 44 + 44, 8 * 44 + 44);
	controls[c] = new CCircDisplay(rct, this,
		c, bitmaps[b_cknob], bitmaps[b_yellow], bitmaps[b_panelfx]);
	ctyp[c] = cCCircDisplay;

	//------ splash screen ---------------------------------------

	rct(677, 7, 707, 78);
	CRect rcti(0, 0, bitmaps[b_inside]->getWidth(), bitmaps[b_inside]->getHeight());
	ctrl_splash = new CSplashScreen(rct, this, n_splash, bitmaps[b_inside], rcti, point);
	frame->addView(ctrl_splash);

	c = n_vu;
	rct(470, 428, 470 + 113, 428 + 20);
	controls[c] = new CMovieBitmap(rct, this,
		c, 5, 20, bitmaps[b_vu], point);
	frame->addView(controls[c]);
	ctyp[c] = cCMovieBitmap;

	for (x = 0; x < kNumControls; x++)
		if (controls[x])
			controls[x]->setTransparency(false);

	ctrl_display->setTransparency(false);
#ifdef DELAYED_PARAMS
	ctrl_save->setTransparency(false);
	ctrl_compare->setTransparency(false);
#endif
	ctrl_splash->setTransparency(false);

	frame->draw();

		for (x = 0; x < kNumParams; x++)	// let's fetch all values since effect does not do it
	{
		if (controls[x] != NULL)
			controls[x]->setValue(effect->getParameter(x));
	}

	setParameter(n_display, 1);

	frame_ok = true;
	setParameter(n_fxmode, 1);
	frame->setDirty(true);
	return true;
}

void Editor::close()
{
	int x;
	// attach the missing controls so they get removed automatically
	if (panelmode == n_fxmode)
	{
		c_visible(n_1_vibrato, true);
		c_visible(n_1_vstrength, true);
		c_visible(n_1_vmix, true);
		c_visible(n_2_vibrato, true);
		c_visible(n_2_vstrength, true);
		c_visible(n_2_vmix, true);
	}
	else
	{
		c_visible(n_mrvalve, true);
		c_visible(n_drive, true);
		c_visible(n_set, true);
		c_visible(n_tone, true);
		c_visible(n_mix, true);
		c_visible(n_speakers, true);
		c_visible(n_speed, true);
		c_visible(n_l_slow, true);
		c_visible(n_l_fast, true);
		c_visible(n_u_slow, true);
		c_visible(n_u_fast, true);
		c_visible(n_belt, true);
		c_visible(n_spread, true);
		c_visible(n_complex, true);
		c_visible(n_pedalspeed, true);
	}

	for (x = 0; x < kNumBitmaps; x++)
		if (bitmaps[x] != NULL)
			bitmaps[x]->forget();

	if (frame)
		delete frame;
	frame = 0;

}

void Editor::valueChanged(CDrawContext* context, CControl* control)
{
	if (!control)
		return;
	long	tag = control->getTag();
	float	value = control->getValue();

	effect->setParameter(n_param_is_manual, 1);

#ifdef DELAYED_PARAMS
	if (tag == n_compare)
		effect->setParameterAutomated(tag, value);
	else
#endif
		if (tag == n_display)
		{
			effect->setParameterAutomated(tag, value);
			ctrl_display->redraw();
			setParameter(tag, value);
		}
#ifdef DELAYED_PARAMS
		else if (tag == n_save)
		{
			if (control->getValue() == 0)
			{
				ctrl_display->setstring(3, "program saved");
				ctrl_compare->setValue(0);
				ctrl_compare->draw(context);
				ctrl_display->draw(context);
				effect->setParameterAutomated(tag, 1);
				effect->setParameterAutomated(n_compare, 0);
			}
			control->draw(context);
			// control->update(context);
			control->setDirty(true);
		}
#endif
		else if (tag == n_split)
		{
			if (value > .5f)
			{
				ctrl_display->setstring(3, "Select Pedal Splitpoint");
				effect->setParameter(n_split, 1);
			}
			else
			{
				ctrl_display->setstring(3, "Splitpoint removed");
				effect->setParameter(n_split, 0);
			}
		}
		else if (tag == n_splash)
			effect->setParameter(tag, value);


	if (tag >= 0 && tag < kNumParams)
	{
#ifdef DELAYED_PARAMS
		if (ctrl_compare->getValue() > 0)
		{
			control->setValue(effect->getParameter(tag));
			control->redraw();
			control->setDirty(false);
		}
		else
#endif
		{
			control->redraw();
			effect->setParameterAutomated(tag, value);

#ifdef DELAYED_PARAMS
			ctrl_display->setstring(3, " * modified *");
#else
			ctrl_display->setstring(3, "");
#endif
			ctrl_display->redraw();
		}

		if (mousedown >= 0)		// show actual value in parameter display
		{
			char	t[256], t2[40];
			effect->getParameterDisplay(tag, t2);
			effect->getParameterName(tag, t);
			sprintf(t, "%s :  %s", t, t2);
			ctrl_display->setstring(4, t);
			ctrl_display->setline(value);
			ctrl_display->setDirty(true);
		}
	}
	effect->setParameter(n_param_is_manual, 0);
}

void Editor::setParameter(VstInt32 index, float value)
{
	CControl* ctrl = NULL;

	if (!frame || !frame_ok)
		return;

	if (index == n_mousedown)			// mouse button down on control
	{
		int	tag = (int)value;
		if (value == -1)
		{
			ctrl_display->setstring(4, "");
			ctrl_display->setline(-1);
			mousedown = -1;
		}
		else if (tag >= 0 && tag < kNumParams)
		{
			char	t[256], t2[40];
			effect->getParameterDisplay(tag, t2);
			effect->getParameterName(tag, t);
			sprintf(t, "%s :  %s", t, t2);
			ctrl_display->setline(effect->getParameter(tag));
			ctrl_display->setstring(4, t);
			mousedown = tag;
		}
		ctrl_display->setDirty(true);
	}
	else if (index == n_1_midi)
		m1 = -1;
	else if (index == n_2_midi)
		m2 = -1;
	else if (index == n_3_midi)
		m3 = -1;
	else if (index == n_display)
	{
		char	t[256];
		long curProgram = effect->getProgram();
		ctrl_display->setValue((float)curProgram);
		effect->getProgramName(t);
		ctrl_display->setstring(2, t);
		sprintf(t, "Rumpelrausch AZR3 V%s  P%02d", VERSION, curProgram);
		ctrl_display->setstring(1, t);
		ctrl_display->setstring(4, "");
		ctrl_display->setline(-1);
#ifdef DELAYED_PARAMS
		if (ctrl_compare->getValue() == 0)
			ctrl_display->setstring(3, "");
#endif
		ctrl_display->setDirty(true);
		return;
	}
	else if (index == n_voicemode && index != panelmode)
	{
		c_visible(n_mrvalve, false);
		c_visible(n_drive, false);
		c_visible(n_set, false);
		c_visible(n_tone, false);
		c_visible(n_mix, false);
		c_visible(n_speakers, false);
		c_visible(n_speed, false);
		c_visible(n_l_slow, false);
		c_visible(n_l_fast, false);
		c_visible(n_u_slow, false);
		c_visible(n_u_fast, false);
		c_visible(n_belt, false);
		c_visible(n_spread, false);
		c_visible(n_complex, false);
		c_visible(n_pedalspeed, false);
		frame->setBackground(bitmaps[b_panelvoice]);
		c_visible(n_1_vibrato, true);
		c_visible(n_1_vstrength, true);
		c_visible(n_1_vmix, true);
		c_visible(n_2_vibrato, true);
		c_visible(n_2_vstrength, true);
		c_visible(n_2_vmix, true);

		frame->setDirty(true);
		panelmode = index;
	}
	else if (index == n_fxmode && index != panelmode)
	{
		c_visible(n_1_vstrength, false);
		c_visible(n_1_vibrato, false);
		c_visible(n_1_vmix, false);
		c_visible(n_2_vstrength, false);
		c_visible(n_2_vibrato, false);
		c_visible(n_2_vmix, false);
		frame->setBackground(bitmaps[b_panelfx]);
		c_visible(n_mrvalve, true);
		c_visible(n_drive, true);
		c_visible(n_set, true);
		c_visible(n_tone, true);
		c_visible(n_mix, true);
		c_visible(n_speakers, true);
		c_visible(n_speed, true);
		c_visible(n_l_slow, true);
		c_visible(n_l_fast, true);
		c_visible(n_u_slow, true);
		c_visible(n_u_fast, true);
		c_visible(n_belt, true);
		c_visible(n_spread, true);
		c_visible(n_complex, true);
		c_visible(n_pedalspeed, true);

		frame->setDirty(true);
		panelmode = index;
	}
	else if (index == n_split)
	{
		char txt[64];
		sprintf(txt, "Splitpoint=%s", noteController::note2str(long(value * 128)));
		ctrl_display->setstring(3, txt);
		ctrl_split->setValue(0);
	}
	else if ((index >= 0 && index < kNumParams) || index == n_compare)
	{
		if (value == -1)
			value = effect->getParameter(index);

		if (controls[index] != NULL)
			ctrl = controls[index];

		if (ctrl != NULL)
		{
			if (value == ctrl->getValue())
				return;
			ctrl->setValue(value);
			ctrl->setDirty(true);
			//			postUpdate();
		}
	}
}

void Editor::c_visible(long tag, bool show)
{
	CControl* control = controls[tag];
	if (control == NULL)
		return;
	if (show)
		frame->addView(control);
	else
		frame->removeView(control, false);
}