// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltra.h
// Class interface of Ascension Ultra vessel.
// ==============================================================

#ifndef __ASCENSIONULTRA_H
#define __ASCENSIONULTRA_H

#define STRICT

#include "orbitersdk.h"
#include "resource.h"
#include "TurnAroundHangar.h"
#include "LightStorageHangar.h"
#include "BeaconLinks.h"

const double EMPTY_MASS    = 11000.0;  // standard configuration

#define TURNAROUNDHANGARS 5
#define LIGHTSTORAGEHANGARS 12
#define TAXIWAYSUBSECTIONS 32
#define TAXIWAYPATHS 36
#define RUNWAYSUBSECTIONS 56
#define RUNWAYPATHS 5

class AscensionUltra: public VESSEL2 {
public:
	AscensionUltra (OBJHANDLE hObj, int fmodel);
	~AscensionUltra ();
	virtual double GetVersion();
	void DefineAnimations ();
	void DrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
	
	// Overloaded callback functions
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkLoadStateEx (FILEHANDLE scn, void *vs);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
	int  clbkConsumeBufferedKey (DWORD key, bool down, char *kstate);	
	bool clbkLoadGenericCockpit ();

	MESHHANDLE meshHangar, meshWindow, meshTopo, meshPlaceHolder, meshLightStorage;  // local external mesh and global template
	
	enum {CAM_GENERIC, CAM_PANELMAIN, CAM_PANELUP, CAM_PANELDN, CAM_VCPILOT, CAM_VCPSNGR1, CAM_VCPSNGR2, CAM_VCPSNGR3, CAM_VCPSNGR4} campos;

	BEACONLIGHTSPEC beacon[7];                   // light beacon definitions
	void SetNavlight (bool on);
	void SetBeacon (bool on);
	void SetStrobe (bool on);
	void MoveGroup(int mesh, VECTOR3 v);
	void RotateGroup(int mesh, float angle, VECTOR3 v, VECTOR3 ref);

	virtual Hangar *GetHangar(HangarType type, int index);
	virtual int GetHangars(HangarType type);
	
private:
	TurnAroundHangar turnAround[TURNAROUNDHANGARS];
	LightStorageHangar lightStorage[LIGHTSTORAGEHANGARS];
	BeaconArray taxiwaySubsection[TAXIWAYSUBSECTIONS];
	BeaconPath taxiwayPath[TAXIWAYPATHS];
	BeaconArray runwaySubsection[RUNWAYSUBSECTIONS];
	BeaconPath runwayPath[RUNWAYPATHS];
	BeaconLinks taxiways;

	int modelidx;                                // flight model index
	VISHANDLE visual;                            // handle to DG visual representation	
	
	int cur_TurnAround, cur_LightStorage;

	//DEBUG
	double disx, disy, disz, stp;
	int mnr, dnr;
};

typedef struct {
	HINSTANCE hDLL;
	HFONT font[2];
	DWORD col[4];
	HBRUSH brush[4];
	HPEN pen[2];
} GDIParams;

// Some mesh groups referenced in the code
#define MESHGRP_VC_HUDMODE          0
#define MESHGRP_VC_HBALANCECNT     18
#define MESHGRP_VC_SCRAMGIMBALCNT  19
#define MESHGRP_VC_PGIMBALCNT      20
#define MESHGRP_VC_YGIMBALCNT      21
#define MESHGRP_VC_NAVMODE         59
#define MESHGRP_VC_LMFDDISP       109
#define MESHGRP_VC_RMFDDISP       110
#define MESHGRP_VC_STATUSIND      118
#define MESHGRP_VC_HORIZON        120
#define MESHGRP_VC_HUDDISP        136

// Panel area identifiers:
// Panel 0
#define AID_PROPELLANTSTATUS     0
#define AID_ENGINEMAIN           2
#define AID_ENGINEHOVER          3
#define AID_ENGINESCRAM          4
#define AID_ATTITUDEMODE         5
#define AID_ADCTRLMODE           6
#define AID_NAVMODE              7
#define AID_HUDMODE              8
#define AID_AOAINSTR             9
#define AID_SLIPINSTR           10
#define AID_LOADINSTR           11
#define AID_HORIZON             13
#define AID_MFD1_BBUTTONS       14
#define AID_MFD1_LBUTTONS       15
#define AID_MFD1_RBUTTONS       16
#define AID_MFD2_BBUTTONS       17
#define AID_MFD2_LBUTTONS       18
#define AID_MFD2_RBUTTONS       19
#define AID_PGIMBALMAIN         22
#define AID_PGIMBALMAINMODE     23
#define AID_YGIMBALMAIN         24
#define AID_YGIMBALMAINMODE     25
#define AID_GIMBALSCRAM         26
#define AID_GIMBALSCRAMMODE     27
#define AID_ELEVATORTRIM        28
#define AID_PGIMBALMAINDISP     29
#define AID_YGIMBALMAINDISP     30
#define AID_GIMBALSCRAMDISP     31
#define AID_MAINDISP1           32
#define AID_MAINDISP2           33
#define AID_MAINDISP3           34
#define AID_MAINDISP4           35
#define AID_SCRAMDISP2          36
#define AID_SCRAMDISP3          37
#define AID_MAINPROP            38
#define AID_MAINPROPMASS        39
#define AID_RCSPROP             40
#define AID_RCSPROPMASS         41
#define AID_SCRAMPROP           42
#define AID_SCRAMPROPMASS       43
#define AID_HOVERBALANCE        44
#define AID_HBALANCEMODE        45
#define AID_HBALANCEDISP        46
#define AID_RADIATORSWITCH      47
#define AID_RETRODOORSWITCH     48
#define AID_HATCHSWITCH         49
#define AID_LADDERSWITCH        50
#define AID_MWS                 51

// Panel 1
#define AID_OUTERAIRLOCKBUTTON 100
#define AID_INNERAIRLOCKBUTTON 101
#define AID_NAVLIGHTBUTTON     102
#define AID_BEACONBUTTON       103
#define AID_STROBEBUTTON       104
#define AID_VPITCH             105
#define AID_VBANK              106
#define AID_VYAW               107
#define AID_APITCH             108
#define AID_ABANK              109
#define AID_AYAW               110
#define AID_MPITCH             111
#define AID_MBANK              112
#define AID_MYAW               113
#define AID_SCRAMTEMPDISP      114

// Panel 2
#define AID_WBRAKE_BOTH        200
#define AID_WBRAKE_LEFT        201
#define AID_WBRAKE_RIGHT       202
#define AID_GEARLEVER          203
#define AID_NOSECONELEVER      204
#define AID_GEARINDICATOR      205
#define AID_NOSECONEINDICATOR  206
#define AID_DOCKRELEASE        207

// Virtual cockpit-specific area identifiers:
#define AID_MFD1_PWR          1024
#define AID_MFD1_SEL          1025
#define AID_MFD1_MNU          1026
#define AID_MFD2_PWR          1027
#define AID_MFD2_SEL          1028
#define AID_MFD2_MNU          1029
#define AID_HUDBUTTON1        1030
#define AID_HUDBUTTON2        1031
#define AID_HUDBUTTON3        1032
#define AID_HUDBUTTON4        1033
#define AID_HUDCOLOUR         1034
#define AID_HUDINCINTENS      1035
#define AID_HUDDECINTENS      1036
#define AID_NAVBUTTON1        1037
#define AID_NAVBUTTON2        1038
#define AID_NAVBUTTON3        1039
#define AID_NAVBUTTON4        1040
#define AID_NAVBUTTON5        1041
#define AID_NAVBUTTON6        1042
#define AID_RADIATOREX        1043
#define AID_RADIATORIN        1044
#define AID_HATCHOPEN         1045
#define AID_HATCHCLOSE        1046
#define AID_LADDEREX          1047
#define AID_LADDERIN          1048
#define AID_RCOVEROPEN        1049
#define AID_RCOVERCLOSE       1050
#define AID_ILOCKOPEN         1051
#define AID_ILOCKCLOSE        1052
#define AID_OLOCKOPEN         1053
#define AID_OLOCKCLOSE        1054
#define AID_NCONEOPEN         1055
#define AID_NCONECLOSE        1056
#define AID_GEARDOWN          1057
#define AID_GEARUP            1058

#endif // !__ASCENSIONULTRA_H