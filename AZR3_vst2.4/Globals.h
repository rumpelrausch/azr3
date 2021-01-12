#pragma once
#define	VERSION	"2.0"

// #define DELAYED_PARAMS 1

#define	WAVETABLESIZE	256

const float Pi = 3.14159265358979323846;

#define	NUMOFVOICES		11

#define _fabsf(fv) fabsf(fv)

/*
	ID and Effect name are used by the host application to distinguish
	between plugins. You can register a specific ID through a Steinberg
	service.
*/
#define MY_ID			'FLP5'
#define	EFFECT_NAME		"AZR3"
#define	VENDOR_STRING	"Rumpelrausch Täips"
#define	PRODUCT_STRING	"AZR3"

#define	EVTBUFSIZE	1024	// number of events to memorize in buffer

enum
{
	TONEWHEEL_SHAPE_SINE=0,
	TONEWHEEL_SHAPE_SINE1,
	TONEWHEEL_SHAPE_SINE2,
	TONEWHEEL_SHAPE_SINE3,
	TONEWHEEL_SHAPE_TRI,
	TONEWHEEL_SHAPE_SAW,
	NUM_OF_TONEWHEEL_SHAPES
};

enum	// bitmap indices
{
	b_panelfx=0,
	b_panelvoice,
	b_cknob,
	b_dbblack,
	b_dbwhite,
	b_dbbrown,
	b_inside,
	b_miniledred,
	b_minioffon,
	b_onoffgreen,
	b_vonoff,
	b_yellow,
	b_vu,
	kNumBitmaps,
};

enum
{
	kNumPrograms = 32,
	kNumOutputs = 2,
// The parameters
// The order shows how the code has grown in time...
	n_mono=0,
	n_click,
	n_bender,
	n_shape,
	n_perc,
	n_percvol,
	n_percfade,
	n_vol1,
	n_vol2,
	n_vol3,
	n_master,
//11
	n_1_perc,
	n_1_db1,
	n_1_db2,
	n_1_db3,
	n_1_db4,
	n_1_db5,
	n_1_db6,
	n_1_db7,
	n_1_db8,
	n_1_db9,
//21
	n_1_vibrato,
	n_1_vstrength,
	n_1_vmix,
	n_2_perc,
	n_2_db1,
	n_2_db2,
	n_2_db3,
	n_2_db4,
	n_2_db5,
	n_2_db6,
//31
	n_2_db7,
	n_2_db8,
	n_2_db9,
	n_2_vibrato,
	n_2_vstrength,
	n_2_vmix,
	n_3_perc,
	n_3_db1,
	n_3_db2,
	n_3_db3,
//41
	n_3_db4,
	n_3_db5,
	n_mrvalve,
	n_drive,
	n_set,
	n_tone,
	n_mix,
	n_speakers,
	n_speed,
	n_l_slow,
//51
	n_l_fast,
	n_u_slow,
	n_u_fast,
	n_belt,
	n_spread,
	n_complex,
	n_pedalspeed,
	n_splitpoint,
	n_sustain,
	n_1_sustain,
// 61
	n_2_sustain,
	n_3_sustain,

	n_spare_1,
	n_spare_2,
	n_spare_3,

	kNumParams,

	n_1_midi,
	n_2_midi,
	n_3_midi,
	n_vu,
	n_compare,
	n_save,
	n_display,
	n_redraw,
	n_setprogname,
	n_param_is_manual,
	n_splash,
	n_voicemode,
	n_fxmode,
	n_split,
	n_output,

	kNumControls,

	n_mute,
	n_mousedown
};

// Let's be friendly and do at least some GUI-less support...
static char	labels[kNumParams][32]=
{
	"Mono    ",
	"Click   ",
	"Bender  ",
	"Shape   ",
	"Percussion register",
	"Percussion volume",
	"Percussion fadetime",
	"Volume 1",
	"Volume 2",
	"Volume 3",
	"Volume Master",

	"1/ Percussion on",
	"1/ 16   ",
	"1/ 5 1/3",
	"1/ 8    ",
	"1/ 4    ",
	"1/ 2 2/3",
	"1/ 2    ",
	"1/ 1 3/5",
	"1/ 1 1/3",
	"1/ 1    ",

	"1/ Vibrato",
	"1/ Vibrato Strength",
	"1/ Vibrato Mix",
	"2/ Percussion on",
	"2/ 16   ",
	"2/ 5 1/3",
	"2/ 8    ",
	"2/ 4    ",
	"2/ 2 2/3",
	"2/ 2    ",

	"2/ 1 3/5",
	"2/ 1 1/3",
	"2/ 1    ",
	"2/ Vibrato",
	"2/ Vibrato Strength",
	"2/ Vibrato Mix",
	"3/ Percussion on",
	"3/ 16   ",
	"3/ 5 1/3",
	"3/ 8    ",

	"3/ 4    ",
	"3/ 2 2/3",
	"Mr. Valve on",
	"Drive   ",
	"Set     ",
	"Tone    ",
	"Mix     ",
	"Speakers on",
	"Speed   ",
	"Speed lower/slow",

	"Speed lower/fast",
	"Speed upper/slow",
	"Speed upper/fast",
	"Belt friction",
	"Microhpone spread",
	"Complex on",
	"Pedal speed on",
	"Splitpoint",
	"Sustain time",
	"Sustain 1 on",
	"Sustain 2 on",
	"Sustain 3 on",
	"spare  1",
	"spare  2",
	"spare  3"
};
