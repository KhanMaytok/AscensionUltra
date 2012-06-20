#define ORBITER_MODULE
#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))
#define PREFIXSIZE	40
#define LINESIZE	1024
#define INIFILE		"Modules\\AscensionUltra.ini"

#include "AscensionUltra.h"
#include "Module.h"
#include "ScnEditorAPI.h"
#include "DlgCtrl.h"
#include "meshres.h"
#include "resource.h"

#define STRICT

// ==============================================================
// Global parameters
// ==============================================================

typedef struct {
	HINSTANCE hDLL;
	HFONT font[2];
	DWORD col[4];
	HBRUSH brush[4];
	HPEN pen[2];
} GDIParams;

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

// Module initialisation
DLLCLBK void InitModule (HINSTANCE hModule)
{
	g_Param.hDLL = hModule;
//	oapiRegisterCustomControls (hModule);

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

void RotateMesh(MESHHANDLE mesh, float angle, VECTOR3 v, VECTOR3 ref)
{
	//Rotate all mesh groups of the mesh handle
	double c=cos(angle);
	double s=sin(angle);
	double c1=1-c;
	MATRIX3 M=_M(	c+v.x*v.x*c1	, v.x*v.y*c1-v.z*s	, v.x*v.z*c1+v.y*s,
					v.y*v.x*c1+v.z*s, c+v.y*v.y*c1		, v.y*v.z*c1-v.x*s,
					v.z*v.x*c1-v.y*s, v.z*v.y*c1+v.x*s	, c+v.z*v.z*c1		);
	int k=oapiMeshGroupCount(mesh);
	for (int i=0;i<k;i++)
	{
		MESHGROUP *m=oapiMeshGroup(mesh, i);
		DWORD l=m->nVtx;
		for(DWORD j=0;j<l;j++)
		{
			VECTOR3 p=_V(m->Vtx[j].x, m->Vtx[j].y, m->Vtx[j].z);
			VECTOR3 n=_V(m->Vtx[j].nx, m->Vtx[j].ny, m->Vtx[j].nz);
			p=mul(M, (p-ref))+ref;
			n=mul(M, (n-ref))+ref;
			m->Vtx[j].x=p.x;
			m->Vtx[j].y=p.y;
			m->Vtx[j].z=p.z;
			m->Vtx[j].nx=n.x;
			m->Vtx[j].ny=n.y;
			m->Vtx[j].nz=n.z;
		}
	}	
}

void ReadBeaconDefinition(BeaconArray *beacons, int count, const char *section, VECTOR3 position, VESSEL *owner)
{
	char pf[PREFIXSIZE]="";
	char line[LINESIZE]="";
	char defaults[LINESIZE]="";
	GetPrivateProfileString(section, "BeaconParams", "1,1,1,1,0", defaults, LINESIZE, INIFILE);
	for(int i=0;i<count;i++)
	{
		sprintf(pf, "BeaconArray%d", i);
		GetPrivateProfileString(section, pf, "0,0,0 0,0,0 0,0,0 1", line, LINESIZE, INIFILE);
		VECTOR3 start, end, color;
		int length;
		sscanf(line, "%lf,%lf,%lf %lf,%lf,%lf %lf,%lf,%lf %d", &start.x, &start.y, &start.z, &end.x, &end.y, &end.z, &color.x, &color.y, &color.z, &length);
		sprintf(pf, "BeaconParams%d", i);
		GetPrivateProfileString(section, pf, defaults, line, LINESIZE, INIFILE);
		double size, falloff, period, duration, propagation;
		sscanf(line, "%lf,%lf,%lf,%lf,%lf", &size, &falloff, &period, &duration, &propagation);
		//Coordinate transformation
		start.x=-start.x;
		start+=position;
		end.x=-end.x;
		end+=position;
		color/=255;
		beacons[i].Init(owner, start, end, color, length);
		beacons[i].SetSize(size);
		beacons[i].SetFallOff(falloff);
		beacons[i].SetPeriod(period);
		beacons[i].SetDuration(duration);
		beacons[i].SetPropagate(propagation);
		beacons[i].Switch(true);
	}
}

void ReadBeaconPaths(BeaconPath *paths, int count, BeaconArray *beacons, const char *section, VESSEL *owner)
{
	char pf[PREFIXSIZE]="";
	char line[LINESIZE]="";
	for(int i=0;i<count;i++)
	{
		paths[i].Init(owner, NULL, _V(0,0,0), 0, 0);
		sprintf(pf, "BeaconPath%d", i);
		GetPrivateProfileString(section, pf, "", line, LINESIZE, INIFILE);
		int k=strlen(line);
		int s=0;
		bool valid=false;
		for(int j=0;j<k;j++) switch(line[j])
		{
		case ';':
			line[j]=0x00;
			j=k;
			break;
		case ',':
			if (valid)
			{
				line[j]=0x00;
				int val=atoi(line+s);
				if (val<0) paths[i].Add(beacons-val, true);
				else paths[i].Add(beacons+val, false);
			}
			s=j+1;
			valid=false;
			break;
		default:
			if (line[j]<'0' || line[j]>'9') break;
			valid=true;
			break;
		}		
		if (valid)
		{
			int val=atoi(line+s);
			if (val<0) paths[i].Add(beacons-val, true);
			else paths[i].Add(beacons+val);
		}
	}
}

void ReadBeaconEndPoints(std::vector<char *> *endPoints, const char *section)
{
	char pf[PREFIXSIZE]="";
	char line[LINESIZE]="";
	int i=0;
	while(true)
	{
		sprintf(pf, "BeaconEndPoint%d", i++);
		GetPrivateProfileString(section, pf, "", line, LINESIZE, INIFILE);
		if (line[0]==0x00) break;
		char *p=new char[strlen(line)+1];
		strcpy(p, line);
		endPoints->push_back(p);
	}
}

void ReadBeaconRoutes(Routes *routes, BeaconPath *paths, std::vector<char *> *endPoints, const char *section)
{
	char pf[PREFIXSIZE]="";
	char line[LINESIZE]="";
	int i=0;
	while(true)
	{
		sprintf(pf, "BeaconRoute%d", i++);
		GetPrivateProfileString(section, pf, "", line, LINESIZE, INIFILE);
		if (line[0]==0x00) break;
		int path, start, end, reverse, priority;
		sscanf(line, "%d %d:%d %d %d", &path, &start, &end, &reverse, &priority);
		routes->Add(&paths[path], (*endPoints)[start], (*endPoints)[end], reverse?true:false, priority);
	}
}

void OverwriteBeaconParamsDefinition(BeaconArray *beacons, int count, const char *section)
{
	char pf[PREFIXSIZE]="";
	char line[LINESIZE]="";
	for(int i=0;i<count;i++)
	{
		sprintf(pf, "BeaconParams%d", i);
		GetPrivateProfileString(section, pf, "", line, LINESIZE, INIFILE);
		if (line[0]==0x00) continue;
		double size, falloff, period, duration, propagation;
		sscanf(line, "%lf,%lf,%lf,%lf,%lf", &size, &falloff, &period, &duration, &propagation);
		beacons[i].SetSize(size);
		beacons[i].SetFallOff(falloff);
		beacons[i].SetPeriod(period);
		beacons[i].SetDuration(duration);
		beacons[i].SetPropagate(propagation);
	}
}

// --------------------------------------------------------------
// Module cleanup
// --------------------------------------------------------------
DLLCLBK void ExitModule (HINSTANCE hModule)
{
//	oapiUnregisterCustomControls (hModule);

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
			GetDG(hTab)->GetHangar(HANGARTYPETA, 0)->GetDoor(0)->Close();
			return TRUE;
		case IDC_OLOCK_OPEN:
			GetDG(hTab)->GetHangar(HANGARTYPETA, 0)->GetDoor(0)->Open();
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
			dg->GetHangar(HANGARTYPETA, 0)->GetDoor(0)->Close();
			return 0;
		case IDC_OLOCK_OPEN:
			dg->GetHangar(HANGARTYPETA, 0)->GetDoor(0)->Open();
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

	op = dg->GetHangar(HANGARTYPETA, 0)->GetDoor(0)->GetPosition()==0.0?0:1;
	SendDlgItemMessage (hWnd, IDC_OLOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_OLOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);	
}
