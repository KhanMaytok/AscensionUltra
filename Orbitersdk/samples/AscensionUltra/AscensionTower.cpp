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
	width=(int)w/36;
	height=(int)h/28;
	mfdWidth=w;
	mfdHeight=h;

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
char *AscensionTower::ButtonLabel (int bt){return data->GetButtonLabel(bt);}

// Return button menus
int AscensionTower::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static MFDBUTTONMENU mnu[12];
	if (menu) *menu = mnu;
	return data->GetButtonMenu(mnu);	
}

void AscensionTower::WriteMFD(char *text, int line, int column, bool halfLines, bool rightAligned, bool highlight)
{
	int l=strlen(text);
	int x=0;
	int y=0;
	if (line<0)
	{
		x=(1+(36-l)/2)*width;
		y=13*height;		
	}
	else
	{
		y=(int)(line*height) >> (halfLines?1:0);
		if (column<0 && !rightAligned) x=(1+(36-l)/2)*width;
		else
		{
			if (rightAligned) x=mfdWidth-(l+1)*width;
			else x=column*width;
		}
	}
	if (highlight)
	{
		SelectObject(hDC, g_Bar);
		Rectangle(hDC, width-2, y-2, mfdWidth-width+2, y+height+6 );		
	}
	TextOut(hDC, x, y, text, l);
}

// Repaint the MFD
void AscensionTower::Update (HDC hDC)
{
	this->hDC=hDC;

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

	char line[40];
	AscensionUltra *ascension=NULL;	
	AscensionTowerState state=data->GetState();
	if (state!=AscensionTowerState::BaseSelect)
	{
		if ((ascension=data->GetAscension())==NULL)
		{
			data->SetState(state=AscensionTowerState::BaseSelect);
			InvalidateButtons();
		}
	}

	switch(state)
	{
	case AscensionTowerState::BaseSelect:
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::GroundMenu:
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::HangarForDoorSelection:
	case AscensionTowerState::DoorSelection:
		RenderSelectionPage();	
		break;
	}

	SelectDefaultFont(hDC, 1);

	Title (hDC, data->GetTitle());	
	WriteMFD(data->GetSubtitle(), 2, 2);
}

void AscensionTower::RenderSelectionPage()
{
	static int atButton[6]={8, 16, 24, 33, 41, 50}; //Best choice for certain MFD size in half-height units
	char line[40];
	int size=data->GetListSize();
	int page=data->GetPage();
	int pages=(size+5)/6;
	if (page>=pages)
	{
		data->SetPage(page=max(pages-1,0));
		InvalidateButtons();
	}
	
	//Base selection screens
	//Descriptions (normal, light green)
	SelectDefaultFont (hDC, 0);
	int selection=data->GetSelection();
	for(int i=0; i+page*6<size && i<6; i++) WriteMFD(data->GetListItem(i+page*6).Name, atButton[i], 1, true, false, i==selection);
	if (pages>1)
	{
		sprintf(line, "p.%d/%d", page+1, pages);
		WriteMFD(line, 27, NULL, false, true);
	}
	else if (pages==0) WriteMFD("N O   B A S E S   A V A I L A B L E");
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

