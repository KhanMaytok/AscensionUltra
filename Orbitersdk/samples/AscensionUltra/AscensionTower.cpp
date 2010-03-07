// ==============================================================
//                ORBITER MODULE: ASCENSIONTOWER
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionTower.cpp
// Class implementation of Ascension tower MFD.
// ==============================================================

#define STRICT
#define ORBITER_MODULE
#include "windows.h"
#include "orbitersdk.h"
#include "AscensionTower.h"

// ==============================================================
// Global variables

int g_MFDmode; // identifier for new MFD mode

// ==============================================================
// API interface

DLLCLBK void opcDLLInit (HINSTANCE hDLL)
{
	static char *name = "Ascension Tower";   // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;                // MFD mode selection key
	spec.msgproc = AscensionTower::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
}

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
}

// ==============================================================
// MFD class implementation

// Constructor
AscensionTower::AscensionTower (DWORD w, DWORD h, VESSEL *vessel)
: MFD (w, h, vessel)
{
	// Add MFD initialisation here
}

// Destructor
AscensionTower::~AscensionTower ()
{
	// Add MFD cleanup code here
}

// Return button labels
char *AscensionTower::ButtonLabel (int bt)
{
	// The labels for the two buttons used by our MFD mode
	static char *label[2] = {"UP", "DN"};
	return (bt < 2 ? label[bt] : 0);
}

// Return button menus
int AscensionTower::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the two buttons
	static const MFDBUTTONMENU mnu[2] = {
		{"Move up", 0, '['},
		{"Move down", 0, ']'}
	};
	if (menu) *menu = mnu;
	return 2; // return the number of buttons used
}


// Repaint the MFD
void AscensionTower::Update (HDC hDC)
{
	Title (hDC, "Ascension Tower");
	// Draws the MFD title

	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.
}

// MFD message parser
int AscensionTower::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new AscensionTower (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

