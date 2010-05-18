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
	static char *name = "Ascension Tower";
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;
	spec.msgproc = AscensionTower::MsgProc;
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_Bar=NULL;
}

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	if (g_Bar!=NULL) DeleteObject(g_Bar);
	oapiUnregisterMFDMode (g_MFDmode);
}

// ==============================================================
// MFD class implementation

// Constructor
AscensionTower::AscensionTower (UINT mfd, DWORD w, DWORD h, VESSEL *vessel)
: MFD (w, h, vessel)
{	
	width=(int)w/35;
	height=(int)h/28;

	OBJHANDLE obj=vessel->GetHandle();
	std::map<UINT, AscensionTowerData *> *mfds=g_MFDData[obj];
	if (mfds==NULL)
	{
		mfds=new std::map<UINT, AscensionTowerData *>;
		g_MFDData[obj]=mfds;
	}
	data=(*mfds)[mfd];
	if (data==NULL) (*mfds)[mfd]=data=new AscensionTowerData();	
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
		{"Move up", 0, 'U'},
		{"Move down", 0, 'D'}
	};
	if (menu) *menu = mnu;
	return 2; // return the number of buttons used
}


// Repaint the MFD
void AscensionTower::Update (HDC hDC)
{
	//
	//Creating the pen for drawing the progress bar
	if (g_Bar==NULL)
	{
		LOGPEN pen;
		SelectDefaultPen(hDC, 2);
		GetObject(GetCurrentObject(hDC, OBJ_PEN), sizeof(LOGPEN), &pen);
		g_MiddleGreen=pen.lopnColor;
		g_Bar=CreateSolidBrush(g_MiddleGreen);
	}

	Title (hDC, "Ascension Tower");
	char line[20];
	int l=0;
	AscensionUltra *ascension=data->GetAscension();
	int page=data->GetPage();

	//Descriptions (normal, light green)
	SelectDefaultFont (hDC, 0);

	if (page<0)
	{
		if (data->StartList())
		{
			//Base selection screens	
			sprintf(line, "Select base:");
			l=strlen(line);
			TextOut(hDC, (1+(37-l)/2)*width, (2*height) >> 1, line, l);
			int i=4;
			do
			{
				sprintf(line, "[%d] %s", data->GetListIndex(), data->GetListName());
				l=strlen(line);
				TextOut(hDC, 1*width, i++*height, line, l);
			}
			while (data->NextList());
		}
		else
		{
			//No bases available
			sprintf(line, "N O   B A S E S   A V A I L A B L E");
			l=strlen(line);
			TextOut(hDC, (1+(37-l)/2)*width, (12*height) >> 1, line, l);
		}
	}
	else
	{
		//Base interaction screens
		sprintf(line, "Base %s", ascension->GetName());
		l=strlen(line);
		TextOut(hDC, (1+(37-l)/2)*width, (6*height) >> 1, line, l);
	}	
}

// MFD message parser
int AscensionTower::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new AscensionTower (mfd, LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

// Handling shortcut keys
bool AscensionTower::ConsumeKeyBuffered(DWORD key)
{	
	bool result=true;
	switch(key)
	{
	case OAPI_KEY_U://Up
		data->SetPage(data->GetPage()+1);
		break;
	case OAPI_KEY_D://Down
		data->SetPage(data->GetPage()-1);
		break;
	default:
		result=false;
	}
	if (!result) return false;
	InvalidateDisplay();
	return true;
}

// Handling button presses by forwarding shortcuts
bool AscensionTower::ConsumeButton(int bt, int event)
{	
	if (event & PANEL_MOUSE_LBDOWN)
	{
		switch(bt)
		{
		case 0:
			return ConsumeKeyBuffered(OAPI_KEY_U);
		case 1:
			return ConsumeKeyBuffered(OAPI_KEY_D);
		}
	}	
	return false;
}