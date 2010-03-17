// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltra.cpp
// Class implementation of Ascension Ultra vessel.
// ==============================================================

#define ORBITER_MODULE

#include "AscensionUltra.h"
#include "ScnEditorAPI.h"
#include "DlgCtrl.h"
#include "meshres.h"
#include <stdio.h>
#include <math.h>
#include "KeyboardFilter.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))
#define CRANEXOFFSET 130.0
#define CRANEYOFFSET 18.0
#define CRANEREELUPPERPOINT 31.0
#define CRANEREELLOWERPOINT 29.0
#define CRANEREELHEIGHT (CRANEREELUPPERPOINT-CRANEREELLOWERPOINT)

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static HELPCONTEXT g_hc = {
	"html/vessels/AscensionUltra.chm",
	0,
	"html/vessels/AscensionUltra.chm::/AscensionUltra.hhc",
	"html/vessels/AscensionUltra.chm::/AscensionUltra.hhk"
};

// ==============================================================
// Local prototypes

BOOL CALLBACK Ctrl_DlgProc (HWND, UINT, WPARAM, LPARAM);
void UpdateCtrlDialog (AscensionUltra *au, HWND hWnd = 0);

// Constructor
AscensionUltra::AscensionUltra (OBJHANDLE hObj, int fmodel)
: VESSEL2 (hObj, fmodel)
{
	int i, j;

	modelidx = (fmodel ? 1 : 0);
	olock_status      = DOOR_CLOSED;
	olock_proc        = 0.0;
	visual            = NULL;
	exmesh            = NULL;
	vcmesh            = NULL;
	vcmesh_tpl        = NULL;
	insignia_tex      = NULL;
	campos            = CAM_GENERIC;

	skinpath[0] = '\0';
	for (i = 0; i < 3; i++)
		skin[i] = 0;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++) rotidx[i][j] = 0;

	DefineAnimations();

	crane1.SetSpeed(_V(10,10,10));
	crane1.SetCrawl(_V(1,1,1));
}

// --------------------------------------------------------------
// Destructor
// --------------------------------------------------------------
AscensionUltra::~AscensionUltra ()
{
	if (insignia_tex) oapiDestroySurface(insignia_tex);
}

// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void AscensionUltra::DefineAnimations ()
{
	// ***** Hangar door animation *****
	static UINT DoorGrp[8] = {10,9,11,12,6,8,7,5};
	static MGROUP_ROTATE Door1 (0, DoorGrp, 1,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD));
	static MGROUP_ROTATE Door2 (0, DoorGrp+1, 1,	_V(0,0,0), _V(1,0,0), (float)(30*RAD));
	static MGROUP_TRANSLATE Door3 (0, DoorGrp+2, 1, _V(0,6,0));
	static MGROUP_TRANSLATE Door4 (0, DoorGrp+3, 1, _V(0,6,0));
	static MGROUP_TRANSLATE CraneX (0, DoorGrp+4, 1, _V(CRANEXOFFSET,0,0));
	static MGROUP_TRANSLATE CraneY (0, DoorGrp+5, 1, _V(0,0,CRANEYOFFSET));
	static MGROUP_TRANSLATE CraneZ (0, DoorGrp+7, 1, _V(0,-CRANEREELLOWERPOINT,0));
	static MGROUP_SCALE CraneReel (0, DoorGrp+6, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1));

	anim_olock = CreateAnimation (0);
	AddAnimationComponent (anim_olock, 0, 1, &Door1);
	AddAnimationComponent (anim_olock, 0, 1, &Door2);
	AddAnimationComponent (anim_olock, 0, 1, &Door3);
	AddAnimationComponent (anim_olock, 0, 1, &Door4);

	crane1.Init(this, &CraneX, &CraneY, &CraneZ, &CraneReel);
	crane1.DefineAnimations();	

}

// --------------------------------------------------------------
// Apply custom skin to the current mesh instance
// --------------------------------------------------------------
void AscensionUltra::ApplySkin ()
{
	if (!exmesh) return;
	if (skin[0]) oapiSetTexture (exmesh, 2, skin[0]);
	if (skin[1]) oapiSetTexture (exmesh, 3, skin[1]);
	oapiSetTexture (exmesh, 5, insignia_tex);
}

// --------------------------------------------------------------
// Paint individual vessel markings
// --------------------------------------------------------------
void AscensionUltra::PaintMarkings (SURFHANDLE tex)
{
	HDC hDC = oapiGetDC (tex);
	HFONT hFont = CreateFont(38, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	HFONT pFont = (HFONT)SelectObject (hDC, hFont);
	SetTextColor (hDC, 0xD0D0D0);
	SetBkMode (hDC, TRANSPARENT);
	SetTextAlign (hDC, TA_CENTER);
	char cbuf[32];
	strncpy (cbuf, GetName(), 10);
	int len = min(strlen(GetName()), 10);
	TextOut (hDC, 196, 10, cbuf, len);
	TextOut (hDC, 196, 74, cbuf, len);
	SetTextColor (hDC, 0x808080);
	SetTextAlign (hDC, TA_RIGHT);
	TextOut (hDC, 120, 158, cbuf, len);
	SetTextAlign (hDC, TA_LEFT);
	TextOut (hDC, 133, 158, cbuf, len);
	SelectObject (hDC, pFont);
	DeleteObject (hFont);
	oapiReleaseDC (tex, hDC);
}

void AscensionUltra::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
	// draw the default HUD
	VESSEL2::clbkDrawHUD (mode, hps, hDC);

	// TODO: draw vessel status	
}

void AscensionUltra::ActivateOuterAirlock (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	olock_status = action;
	if (action <= DOOR_OPEN) {
		olock_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_olock, olock_proc);
	}
	UpdateCtrlDialog (this);
	RecordEvent ("OLOCK", close ? "CLOSE" : "OPEN");
}

void AscensionUltra::RevertOuterAirlock ()
{
	ActivateOuterAirlock (olock_status == DOOR_CLOSED || olock_status == DOOR_CLOSING ?
		                  DOOR_OPENING : DOOR_CLOSING);
}

void AscensionUltra::SetNavlight (bool on)
{
	beacon[0].active = beacon[1].active = beacon[2].active = on;
	UpdateCtrlDialog (this);
}

void AscensionUltra::SetBeacon (bool on)
{
	beacon[3].active = beacon[4].active = on;
	UpdateCtrlDialog (this);
}

void AscensionUltra::SetStrobe (bool on)
{
	beacon[5].active = beacon[6].active = on;
	UpdateCtrlDialog (this);
}

static UINT HatchGrp[2] = {12,88};

static UINT AileronGrp[8] = {29,51,30,52,35,55,36,54};

void AscensionUltra::UpdateVCMesh()
{
	if (vcmesh) {
		// hide pilot head in VCPILOT position
		if (campos == CAM_VCPILOT) {
			oapiMeshGroup(vcmesh, 138)->UsrFlag |= 0x00000003;
			oapiMeshGroup(vcmesh, 139)->UsrFlag |= 0x00000003;
		} else {
			oapiMeshGroup(vcmesh, 138)->UsrFlag &= 0xFFFFFFFC;
			oapiMeshGroup(vcmesh, 139)->UsrFlag &= 0xFFFFFFFC;
		}
	}
}

static float tv0[8] = {0,0,0.0469f,0.0469f,0,0,0.0469f,0.0469f};

// Set vessel class parameters
void AscensionUltra::clbkSetClassCaps (FILEHANDLE cfg)
{
	// *************** physical parameters **********************

	VESSEL2::SetEmptyMass (EMPTY_MASS);
	VECTOR3 r[2] = {{0,0,6}, {0,0,-4}};
	SetSize (10.0);
	SetVisibilityLimit (7.5e-4, 1.5e-3);
	SetAlbedoRGB (_V(0.77,0.20,0.13));
	SetGravityGradientDamping (20.0);
	SetCW (0.09, 0.09, 2, 1.4);
	SetWingAspect (0.7);
	SetWingEffectiveness (2.5);
	SetCrossSections (_V(53.0,186.9,25.9));
	SetSurfaceFrictionCoeff (0.07, 0.3);
	SetMaxWheelbrakeForce (2e5);
	SetPMI (_V(15.5,22.1,7.7));

	SetDockParams (_V(0,-0.49,10.076), _V(0,0,1), _V(0,1,0));
	SetTouchdownPoints (_V(0,0,10), _V(-3.5,0,-3), _V(3.5,0,-3));
	EnableTransponder (true);
	bool render_cockpit = true;

	// ******************** NAV radios **************************

	InitNavRadios (4);

	SetRotDrag (_V(0.10,0.13,0.04));

	// ************************* mesh ***************************

	// ********************* beacon lights **********************
	static VECTOR3 beaconpos[8] = {{-8.6,0,-3.3}, {8.6,0,-3.3}, {0,0.5,-7.5}, {0,2.2,2}, {0,-1.4,2}, {-8.9,2.5,-5.4}, {8.9,2.5,-5.4}, {0,-1.8,2}};
	static VECTOR3 beaconcol[7] = {{1.0,0.5,0.5}, {0.5,1.0,0.5}, {1,1,1}, {1,0.6,0.6}, {1,0.6,0.6}, {1,1,1}, {1,1,1}};
	for (int i = 0; i < 7; i++) {
		beacon[i].shape = (i < 3 ? BEACONSHAPE_DIFFUSE : BEACONSHAPE_STAR);
		beacon[i].pos = beaconpos+i;
		beacon[i].col = beaconcol+i;
		beacon[i].size = (i < 3 ? 0.3 : 0.55);
		beacon[i].falloff = (i < 3 ? 0.4 : 0.6);
		beacon[i].period = (i < 3 ? 0 : i < 5 ? 2 : 1.13);
		beacon[i].duration = (i < 5 ? 0.1 : 0.05);
		beacon[i].tofs = (6-i)*0.2;
		beacon[i].active = false;
		AddBeacon (beacon+i);
	}

	SetMeshVisibilityMode (AddMesh (exmesh_tpl = oapiLoadMeshGlobal ("AscensionUltra\\TA1-1")), MESHVIS_EXTERNAL);
	SetMeshVisibilityMode (AddMesh (vcmesh_tpl = oapiLoadMeshGlobal ("DG\\DeltaGliderCockpit")), MESHVIS_VC);

	// **************** vessel-specific insignia ****************

	insignia_tex = oapiCreateTextureSurface (256, 256);
	SURFHANDLE hTex = oapiGetTextureHandle (exmesh_tpl, 5);
	if (hTex) oapiBlt (insignia_tex, hTex, 0, 0, 0, 0, 256, 256);
}

// Read status from scenario file
void AscensionUltra::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;

	while (oapiReadScenario_nextline (scn, line)) {
        if (!strnicmp (line, "AIRLOCK", 7)) {
			sscanf (line+7, "%d%lf", &olock_status, &olock_proc);
		} else if (!strnicmp (line, "SKIN", 4)) {
			sscanf (line+4, "%s", skinpath);
			char fname[256];
			strcpy (fname, "DG\\Skins\\");
			strcat (fname, skinpath);
			int n = strlen(fname); fname[n++] = '\\';
			strcpy (fname+n, "dgmk4_1.dds");  skin[0] = oapiLoadTexture (fname);
			strcpy (fname+n, "dgmk4_2.dds");  skin[1] = oapiLoadTexture (fname);
			strcpy (fname+n, "idpanel1.dds"); skin[2] = oapiLoadTexture (fname);
			if (skin[2]) oapiBlt (insignia_tex, skin[2], 0, 0, 0, 0, 256, 256);
		} else if (!strnicmp (line, "LIGHTS", 6)) {
			int lgt[3];
			sscanf (line+6, "%d%d%d", lgt+0, lgt+1, lgt+2);
			SetNavlight (lgt[0] != 0);
			SetBeacon (lgt[1] != 0);
			SetStrobe (lgt[2] != 0);
        } else {
            ParseScenarioLineEx (line, vs);
			// unrecognised option - pass to Orbiter's generic parser
        }
    }
}

// Write status to scenario file
void AscensionUltra::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;

	// Write default vessel parameters
	VESSEL2::clbkSaveState (scn);

	// Write custom parameters
	if (olock_status) {
		sprintf (cbuf, "%d %0.4f", olock_status, olock_proc);
		oapiWriteScenario_string (scn, "AIRLOCK", cbuf);
	}
	if (skinpath[0])
		oapiWriteScenario_string (scn, "SKIN", skinpath);
	for (i = 0; i < 7; i++)
		if (beacon[i].active) {
			sprintf (cbuf, "%d %d %d", beacon[0].active, beacon[3].active, beacon[5].active);
			oapiWriteScenario_string (scn, "LIGHTS", cbuf);
			break;
		}
}

// Finalise vessel creation
void AscensionUltra::clbkPostCreation ()
{
	SetEmptyMass (EMPTY_MASS);

	// update animation states
	SetAnimation (anim_olock, olock_proc);

	PaintMarkings (insignia_tex);
}

// Respond to playback event
bool AscensionUltra::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!stricmp (event_type, "OLOCK")) {
		ActivateOuterAirlock (!stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	}
	return false;
}

// Create DG visual
void AscensionUltra::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	exmesh = GetMesh (vis, 0);
	vcmesh = GetMesh (vis, 1);

	ApplySkin();

	// set VC state
	UpdateVCMesh();

}

// Destroy DG visual
void AscensionUltra::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	visual = NULL;
	exmesh = NULL;
	vcmesh = NULL;
}

// --------------------------------------------------------------
// Frame update
// --------------------------------------------------------------
void AscensionUltra::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate outer airlock
	if (olock_status >= DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (olock_status == DOOR_CLOSING) {
			if (olock_proc > 0.0)
				olock_proc = max (0.0, olock_proc-da);
			else {
				olock_status = DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		} else { // door opening
			if (olock_proc < 1.0)
				olock_proc = min (1.0, olock_proc+da);
			else {
				olock_status = DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		}
		SetAnimation (anim_olock, olock_proc);
	}
	crane1.PostStep(simt, simdt, mjd);
}

bool AscensionUltra::clbkLoadGenericCockpit ()
{
	SetCameraOffset (_V(0,1.467,6.782));
	oapiSetDefNavDisplay (1);
	oapiSetDefRCSDisplay (1);
	campos = CAM_GENERIC;
	return true;
}

// Process buffered key events
int AscensionUltra::clbkConsumeBufferedKey (DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (KEYMOD_SHIFT (kstate)) {
	} else if (KEYMOD_CONTROL (kstate)) {
		switch (key) {
		case OAPI_KEY_SPACE: // open control dialog
			oapiOpenDialogEx (g_Param.hDLL, IDD_CTRL, Ctrl_DlgProc, DLG_CAPTIONCLOSE, this);
			return 1;
		}
	} else {
		switch (key) {
		case OAPI_KEY_O:  // "operate outer airlock"
			RevertOuterAirlock ();
			return 1;
		case OAPI_KEY_V:
			crane1.StartManual();
			return 1;
		case OAPI_KEY_B:
			crane1.Stop();
			return 1;
		}
	}
	return 0;
}

// Module initialisation
DLLCLBK void InitModule (HINSTANCE hModule)
{
	g_Param.hDLL = hModule;
	oapiRegisterCustomControls (hModule);

	// allocate GDI resources
	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(80,80,224));  // blue
	g_Param.brush[3] = CreateSolidBrush (RGB(160,120,64)); // brown
	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224,224,224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164,164,164));
}

// --------------------------------------------------------------
// Module cleanup
// --------------------------------------------------------------
DLLCLBK void ExitModule (HINSTANCE hModule)
{
	oapiUnregisterCustomControls (hModule);

	int i;
	// deallocate GDI resources
	for (i = 0; i < 2; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 2; i++) DeleteObject (g_Param.pen[i]);
}

// --------------------------------------------------------------
// Vessel initialisation
// --------------------------------------------------------------
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	// need to init device-dependent resources here in case the screen mode has changed
	g_Param.col[2] = oapiGetColour(80,80,224);
	g_Param.col[3] = oapiGetColour(160,120,64);

	return new AscensionUltra (hvessel, flightmodel);
}

// --------------------------------------------------------------
// Vessel cleanup
// --------------------------------------------------------------
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (AscensionUltra*)vessel;
}


// ==============================================================
// Scenario editor interface
// ==============================================================

AscensionUltra *GetDG (HWND hDlg)
{
	// retrieve DG interface from scenario editor
	OBJHANDLE hVessel;
	SendMessage (hDlg, WM_SCNEDITOR, SE_GETVESSEL, (LPARAM)&hVessel);
	return (AscensionUltra*)oapiGetVesselInterface (hVessel);
}

// Message procedure for editor page 1 (animation settings)
BOOL CALLBACK EdPg1Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDHELP:
			g_hc.topic = "/SE_Anim.htm";
			oapiOpenHelp (&g_hc);
			return TRUE;
		case IDC_OLOCK_CLOSE:
			GetDG(hTab)->ActivateOuterAirlock (AscensionUltra::DOOR_CLOSED);
			return TRUE;
		case IDC_OLOCK_OPEN:
			GetDG(hTab)->ActivateOuterAirlock (AscensionUltra::DOOR_OPEN);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// Message procedure for editor page 2 (passengers)
BOOL CALLBACK EdPg2Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AscensionUltra *dg;
	int i;

	switch (uMsg) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		break;
	}
	return FALSE;
}

// Message procedure for editor page 3 (damage)
BOOL CALLBACK EdPg3Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AscensionUltra *dg;

	switch (uMsg) {
	case WM_INITDIALOG: {
		dg = (AscensionUltra*)oapiGetVesselInterface ((OBJHANDLE)lParam);		
		} break;
	case WM_COMMAND:
		break;
	case WM_HSCROLL:
		
		break;
	}
	return FALSE;
}

// Add vessel-specific pages into scenario editor
DLLCLBK void secInit (HWND hEditor, OBJHANDLE hVessel)
{
	AscensionUltra *dg = (AscensionUltra*)oapiGetVesselInterface (hVessel);

	EditorPageSpec eps1 = {"Animations", g_Param.hDLL, IDD_EDITOR_PG1, EdPg1Proc};
	SendMessage (hEditor, WM_SCNEDITOR, SE_ADDPAGEBUTTON, (LPARAM)&eps1);
	EditorPageSpec eps2 = {"Passengers", g_Param.hDLL, IDD_EDITOR_PG2, EdPg2Proc};
	SendMessage (hEditor, WM_SCNEDITOR, SE_ADDPAGEBUTTON, (LPARAM)&eps2);
	
}

// Message callback function for control dialog box
BOOL CALLBACK Ctrl_DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AscensionUltra *dg = (uMsg == WM_INITDIALOG ? (AscensionUltra*)lParam : (AscensionUltra*)oapiGetDialogContext (hWnd));
	// pointer to vessel instance was passed as dialog context

	switch (uMsg) {
	case WM_INITDIALOG:
		UpdateCtrlDialog (dg, hWnd);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			oapiCloseDialog (hWnd);
			return TRUE;
		case IDC_OLOCK_CLOSE:
			dg->ActivateOuterAirlock (AscensionUltra::DOOR_CLOSING);
			return 0;
		case IDC_OLOCK_OPEN:
			dg->ActivateOuterAirlock (AscensionUltra::DOOR_OPENING);
			return 0;
		}
		break;
	}
	return oapiDefDialogProc (hWnd, uMsg, wParam, lParam);
}

void UpdateCtrlDialog (AscensionUltra *dg, HWND hWnd)
{
	static int bstatus[2] = {BST_UNCHECKED, BST_CHECKED};

	if (!hWnd) hWnd = oapiFindDialog (g_Param.hDLL, IDD_CTRL);
	if (!hWnd) return;

	int op;

	op = dg->olock_status & 1;
	SendDlgItemMessage (hWnd, IDC_OLOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_OLOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->beacon[0].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_NAVLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[3].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_BEACONLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[5].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_STROBELIGHT, BM_SETCHECK, bstatus[op], 0);
}