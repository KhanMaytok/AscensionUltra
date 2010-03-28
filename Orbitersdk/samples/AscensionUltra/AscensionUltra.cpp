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
#define TOPOOFFSET _V(-2958,0,-3890)
#define TA1MATRIXOFFSET _V(266,0,0)
#define ALLOFFSET _V(3,0,-3)
#define PLACEHOLDEROFFSET _V(-2304,0,-4552)

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

	char prefix[8]="HANGARx";
	for(i=0;i<5;i++)
	{
		prefix[6]=0x30+i;
		hangars[i].Init(this, i, prefix);
	}

	DefineAnimations();

	cur_hangar=-1;

	//DEBUG
	disx=0.0;
	disy=0.0;
	disz=0.0;
	stp=10.0;
	mnr=0;
	dnr=0;
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
	for(int i=0;i<5;i++) hangars[i].DefineAnimations();
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
	SetSize (5000);
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
	SetCOG_elev(0.001);
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

	meshHangar = oapiLoadMeshGlobal ("AscensionUltra\\TA1-1");
	for(int i=0;i<5;i++) SetMeshVisibilityMode (AddMesh (meshHangar, &(ALLOFFSET+TA1MATRIXOFFSET*i)), MESHVIS_EXTERNAL);
	SetMeshVisibilityMode (AddMesh (vcmesh_tpl = oapiLoadMeshGlobal ("DG\\DeltaGliderCockpit")), MESHVIS_VC);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Island1"), &(ALLOFFSET+TOPOOFFSET)), MESHVIS_EXTERNAL);	
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Place_Holders"), &(ALLOFFSET-PLACEHOLDEROFFSET+TOPOOFFSET)), MESHVIS_EXTERNAL);

	// **************** vessel-specific insignia ****************

	insignia_tex = oapiCreateTextureSurface (256, 256);
	SURFHANDLE hTex = oapiGetTextureHandle (meshHangar, 5);
	if (hTex) oapiBlt (insignia_tex, hTex, 0, 0, 0, 0, 256, 256);
}

// Read status from scenario file
void AscensionUltra::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {		
		if (!strnicmp (line, "HANGAR", 6)) {
			sscanf (line+6, "%d", &cur_hangar);
		} else if (cur_hangar>=0 && cur_hangar<5) {
			if (!hangars[cur_hangar].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
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
	for(i=0;i<5;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "HANGAR", cbuf);
		hangars[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "HANGAR", cbuf);
	
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

	for(int i=0;i<5;i++) hangars[i].clbkPostCreation();

	PaintMarkings (insignia_tex);
}

// Respond to playback event
bool AscensionUltra::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "HANGAR", 6))
	{
		//Hangar event
		int hangar=(int)(event_type+6)[0]-0x30;
		if (hangar>=0 && hangar<5) return hangars[hangar].clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	return false;
}

// Create DG visual
void AscensionUltra::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	exmesh = GetMesh (vis, 0);
	vcmesh = GetMesh (vis, 5);

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
	for(int i=0;i<5;i++) hangars[i].clbkPostStep(simt, simdt, mjd);
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
		int res;
		switch (key) {
		//DEBUG
		case OAPI_KEY_1:
			MoveGroup(mnr, _V(stp, 0, 0));
			disx+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_2:
			MoveGroup(mnr, _V(-stp, 0, 0));
			disx-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_3:
			MoveGroup(mnr, _V(0, stp, 0));
			disy+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_4:
			MoveGroup(mnr, _V(0, -stp, 0));
			disy-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_5:
			MoveGroup(mnr, _V(0, 0, stp));
			disz+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_6:
			MoveGroup(mnr, _V(0, 0, -stp));
			disz-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_7:
			stp/=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_8:
			stp*=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_9:
			if (mnr>0) mnr--;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_0:
			mnr++;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;	
		case OAPI_KEY_C:
			if (++dnr>=4) dnr=0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_F:
			if (dnr<4 && mnr<5) hangars[mnr].GetDoor(dnr)->Open();
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_G:
			if (dnr<4 && mnr<5) hangars[mnr].GetDoor(dnr)->Close();
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_H:
			if (dnr<4 && mnr<5) hangars[mnr].GetDoor(dnr)->Stop();
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f - door %d", mnr, disx, disy, disz, stp, dnr);
			return 1;
		case OAPI_KEY_V:
			if (mnr<5) hangars[mnr].GetCrane()->StartManual();
			return 1;
		case OAPI_KEY_B:
			if (mnr<5) hangars[mnr].GetCrane()->Stop();
			return 1;
		//DEBUG END
		}
	}
	return 0;
}

void AscensionUltra::MoveGroup(int mesh, VECTOR3 v)
{
	//Transfer mesh groups
	//TODO: This can be removed if mesh is designed appropriately
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=mesh;
	mt.transform=mt.TRANSLATE;
	mt.P.transparam.shift=v;
	int k=oapiMeshGroupCount(GetMesh(visual, mesh));
	for(mt.ngrp=0;mt.ngrp<k;mt.ngrp++) MeshgroupTransform(visual, mt);	
}

TurnAroundHangar *AscensionUltra::GetHangar(int index){return (index>=0 && index<5)?hangars+index:NULL;}

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


AscensionUltra *GetDG (HWND hDlg)
{
	// retrieve AscensionUltra interface from scenario editor
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
			GetDG(hTab)->GetHangar(0)->GetDoor(0)->Close();
			return TRUE;
		case IDC_OLOCK_OPEN:
			GetDG(hTab)->GetHangar(0)->GetDoor(0)->Open();
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
			dg->GetHangar(0)->GetDoor(0)->Close();
			return 0;
		case IDC_OLOCK_OPEN:
			dg->GetHangar(0)->GetDoor(0)->Open();
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

	op = dg->GetHangar(0)->GetDoor(0)->GetPosition()==0.0?0:1;
	SendDlgItemMessage (hWnd, IDC_OLOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_OLOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->beacon[0].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_NAVLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[3].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_BEACONLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[5].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_STROBELIGHT, BM_SETCHECK, bstatus[op], 0);
}