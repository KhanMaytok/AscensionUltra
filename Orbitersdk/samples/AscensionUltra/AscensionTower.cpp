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
int g_MFDmode; // identifier for new MFD mode
oapi::Brush *myBrush;

// ==============================================================
// API interface

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	static char *name = "Ascension Tower";
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;
	spec.msgproc = AscensionTower::MsgProc;
	spec.context = NULL;
	g_MFDmode = oapiRegisterMFDMode (spec);
	myBrush=oapiCreateBrush(0x00008000);
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	oapiUnregisterMFDMode (g_MFDmode);
	oapiReleaseBrush(myBrush);
}

DLLCLBK void opcCloseRenderViewport (void)
{
	//TODO: implement proper destructors for deleted vessels instead of this poor-man's garbgage collector
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
: MFD2 (w, h, vessel)
{	
	std::map<UINT, AscensionTowerData *> *mfds=g_MFDData[vessel];
	if (mfds==NULL)
	{
		mfds=new std::map<UINT, AscensionTowerData *>;
		g_MFDData[vessel]=mfds;
	}
	data=(*mfds)[mfd];
	this->vessel=vessel;
	if (data==NULL) (*mfds)[mfd]=data=new AscensionTowerData();
	data->SetMFD(this);
}

// Return button labels
char *AscensionTower::ButtonLabel (int bt){return data->GetButtonLabel(bt);}

// Return button menus
int AscensionTower::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static MFDBUTTONMENU mnu[12];
	static MFDBUTTONMENU empty = {NULL, NULL, 0};
	if (menu) *menu = mnu;
	for(int i=0;i<12;i++) mnu[i]=empty;
	return data->GetButtonMenu(mnu);	
}

// Repaint the MFD
bool AscensionTower::Update (oapi::Sketchpad *pad)
{
	this->pad=pad;

	data->Update();

	return true;
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

void AscensionTower::Write(char *text, int line, int column, int flags)
{
	int l=strlen(text);
	int x=0;
	int y=0;
	int w=W/36;
	int h=H/28;

	if (line<0)
	{
		x=(1+(36-l)/2)*w;
		y=13*h;		
	}
	else
	{
		y=(int)(line*h) >> (HALFLINES(flags)?1:0);
		if (column<0 && !(RIGHTALIGNED(flags))) x=(1+(36-l)/2)*w;
		else
		{
			if (RIGHTALIGNED(flags)) x=(column<0?W:column*w)-(l+1)*w;
			else x=column*w;
		}
	}
	if (HIGHLIGHTED(flags))	pad->Rectangle(w-2, y-2, W-w+2, y+h+6);
	pad->Text(x, y, text, l);
}

void AscensionTower::SetWriteStyle(int font, int color, int intensity, int style)
{
	static DWORD colors[4] = {0x0000FF00, 0x0000FFFF, 0x00FFFFFF, 0x000000FF};
	pad->SetFont(GetDefaultFont(font));
	pad->SetTextColor(colors[color]);
	pad->SetPen(GetDefaultPen(color, intensity, style));
	pad->SetBrush(myBrush);
}

VESSEL *AscensionTower::GetVessel(){return vessel;}