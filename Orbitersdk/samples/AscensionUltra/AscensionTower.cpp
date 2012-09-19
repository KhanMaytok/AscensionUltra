// ==============================================================
//                ORBITER MODULE: ASCENSIONTOWER
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionTower.cpp
// Class implementation of Ascension tower MFD.
// ==============================================================

#pragma once
#pragma warning(disable : 4482)
#define STRICT
#define ORBITER_MODULE
#include "windows.h"
#include <map>
#include "orbitersdk.h"
#include "AscensionTower.h"

// ==============================================================
// Global variables

std::map<VESSEL *, std::map<UINT, AscensionTowerData *> *> g_MFDData;
HBRUSH g_Bar;
COLORREF g_MiddleGreen;
int g_MFDmode; // identifier for new MFD mode

// ==============================================================
// API interface

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "Ascension Tower";
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;
	spec.msgproc = AscensionTower::MsgProc;
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_Bar=NULL;
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	if (g_Bar!=NULL) DeleteObject(g_Bar);
	oapiUnregisterMFDMode (g_MFDmode);
}

DLLCLBK void opcCloseRenderViewport (void)
{
	for(std::map<VESSEL *, std::map<UINT, AscensionTowerData *> *>::iterator i=g_MFDData.begin();i!=g_MFDData.end();i++)
	{
		for (std::map<UINT, AscensionTowerData *>::iterator j=i->second->begin();j!=i->second->end();j++) delete j->second;
		i->second->clear();
	}
	g_MFDData.clear();
}

// ==============================================================
// MFD class implementation

// Constructor
AscensionTower::AscensionTower (UINT mfd, DWORD w, DWORD h, VESSEL *vessel)
: MFD (w, h, vessel)
{	
	std::map<UINT, AscensionTowerData *> *mfds=g_MFDData[vessel];
	if (mfds==NULL)
	{
		mfds=new std::map<UINT, AscensionTowerData *>;
		g_MFDData[vessel]=mfds;
	}
	data=(*mfds)[mfd];
	if (data==NULL) (*mfds)[mfd]=data=new AscensionTowerData(this);
}

// Return button labels
char *AscensionTower::ButtonLabel (int bt){return data->GetButtonLabel(bt);}

// Return button menus
int AscensionTower::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static MFDBUTTONMENU mnu[12];
	if (menu) *menu = mnu;
	return data->GetButtonMenu(mnu);	
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

	this->hDC=hDC;
	data->Update(hDC);
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
	if (!data->SetKey(key)) return false;
	InvalidateButtons();
	InvalidateDisplay();
	return true;
}

// Handling button presses
bool AscensionTower::ConsumeButton(int bt, int event)
{
	if (!((event & PANEL_MOUSE_LBDOWN)?data->SetButton(bt):false)) return false;
	InvalidateButtons();
	InvalidateDisplay();
	return true;
}

void AscensionTower::WriteMFD(char *text, int line, int column, int flags)
{
	int l=strlen(text);
	int x=0;
	int y=0;
	if (line<0)
	{
		x=(1+(36-l)/2)*cw;
		y=13*ch;		
	}
	else
	{
		y=(int)(line*ch) >> (HALFLINES(flags)?1:0);
		if (column<0 && !(RIGHTALINED(flags))) x=(1+(36-l)/2)*cw;
		else
		{
			if (RIGHTALINED(flags)) x=(column<0?W:column*cw)-(l+1)*cw;
			else x=column*cw;
		}
	}
	if (HIGHLIGHTED(flags))
	{
		SelectObject(hDC, g_Bar);
		Rectangle(hDC, cw-2, y-2, W-cw+2, y+ch+6 );
	}
	TextOut(hDC, x, y, text, l);
}