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
: MFD2 (w, h, vessel)
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
bool AscensionTower::Update (oapi::Sketchpad *skp)
{
	hDC=skp->GetDC();

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
	AscensionTowerState state=data->GetState();
	AscensionUltra *au=NULL;
	//Get Ascension object - lost or changed Ascension objects will cause state to change
	if (state!=AscensionTowerState::BaseSelect)
	{
		au=data->GetAscension();
		if (au!=ascension)
		{
			ascension=au;
			state=data->GetState();
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
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::DoorSelection:
	case AscensionTowerState::RoomSelection:
	case AscensionTowerState::TaxiRouteStartSelection:
	case AscensionTowerState::TaxiRouteEndSelection:
	case AscensionTowerState::LandingRunwaySelection:
	case AscensionTowerState::Rooster:
		RenderSelectionPage();	
		break;
	case AscensionTowerState::DoorControl:
		RenderSelectionPage();
		RenderDoorControlPage();
		break;
	case AscensionTowerState::CraneControl:
		RenderCraneControlPage();
		break;
	case AscensionTowerState::PersonControl:
		RenderPersonPage();
		break;
	}

	skp->SetFont(GetDefaultFont(1));
	
	Title (skp, data->GetTitle());	
	WriteMFD(data->GetSubtitle(), 2, 2);

	return true;
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

void AscensionTower::RenderDoorControlPage()
{
	SetTextColor(hDC, RGB(255,255,255));
	Door* door=(Door *)data->GetObject();
	if (door->GetPosition()<=0) WriteMFD("Closed", 15);
	else if (door->GetPosition()>=1) WriteMFD("Open", 15);
	else WriteMFD("Moving", 15);
}

void AscensionTower::RenderCraneControlPage()
{
	SetTextColor(hDC, RGB(255,255,255));
	Crane* crane=(Crane *)data->GetObject();
	VECTOR3 pos=crane->GetPosition();
	char line[40];
	sprintf(line, "X: %f", pos.x);
	WriteMFD(line, 6, 4);
	sprintf(line, "Y: %f", pos.y);
	WriteMFD(line, 8, 4);
	sprintf(line, "Z: %f", pos.z);
	WriteMFD(line, 10, 4);
}

void AscensionTower::RenderPersonPage()
{
	SetTextColor(hDC, RGB(255,255,255));
	char line[40];
	sprintf(line, "    Name: %s", "Charly");
	WriteMFD(line, 6, 4);
	sprintf(line, "Function: %s", "Captain");
	WriteMFD(line, 8, 4);
	sprintf(line, "     Age: %d", 25);
	WriteMFD(line, 10, 4);
	sprintf(line, "    Puls: %d", 67);
	WriteMFD(line, 12, 4);
	sprintf(line, "  Weight: %d", 70);
	WriteMFD(line, 14, 4);
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

