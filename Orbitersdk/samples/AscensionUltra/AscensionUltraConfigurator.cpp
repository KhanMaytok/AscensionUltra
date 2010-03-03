#define STRICT
#define ORBITER_MODULE
#include "orbitersdk.h"
#include "Conf_resource.h"
#include <stdio.h>
#include <io.h>

//class VesselConfig;
class AscensionConfig;
class RootConfig;

struct {
	HINSTANCE hInst;
	AscensionConfig *item;
	RootConfig *root;
} gParams;

class AscensionConfig: public LaunchpadItem {
public:
	AscensionConfig(): LaunchpadItem() {}

	void Init (HWND hWnd);
	static BOOL CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

	//LaunchpadItem members
	char *Name() { return "Ascension Configuration"; }
	char *Description() { return "Global configuration for Ascension Island."; }
	bool clbkOpen (HWND hLaunchpad) { return OpenDialog (gParams.hInst, hLaunchpad, IDD_DGCONFIG, WndProc); }	
};

void AscensionConfig::Init (HWND hWnd)
{
	//TODO: dialog initialization
}

BOOL CALLBACK AscensionConfig::WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		((AscensionConfig*)lParam)->Init (hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDOK:
			//TODO: use settings
			EndDialog (hWnd, 0);
			return 0;
		case IDCANCEL:
			EndDialog (hWnd, 0);
			return 0;
		}
		break;
	}
	return 0;
}

class RootConfig: public LaunchpadItem {
public:
	RootConfig(): LaunchpadItem() {}
	char *Name() { return "Base configuration"; }
	char *Description() { return "Configuration for bases."; }
};

// ==============================================================
// The DLL entry point
// ==============================================================

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	gParams.hInst = hDLL;
	gParams.item = new AscensionConfig;	
	gParams.root = NULL;
	
	LAUNCHPADITEM_HANDLE root = oapiFindLaunchpadItem ("Base configuration");
	if (root==NULL)
	{
		gParams.root = new RootConfig;
		root=oapiRegisterLaunchpadItem(gParams.root);
	}
		
	oapiRegisterLaunchpadItem (gParams.item, root);
}

// ==============================================================
// The DLL exit point
// ==============================================================

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the launchpad items
	oapiUnregisterLaunchpadItem (gParams.item);
	delete gParams.item;
	if (gParams.root!=NULL)
	{
		oapiUnregisterLaunchpadItem (gParams.root);	
		delete gParams.root;
	}
}