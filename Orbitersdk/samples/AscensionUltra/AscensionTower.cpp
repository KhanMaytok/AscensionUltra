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
	int page=data->GetPage();
	switch (page)
	{
	case 0:
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		if (page<0)
		{
			switch (bt)
			{
				case 6: return (data->GetListSize()>1)?"SEL":NULL;
				case 7: return (data->GetListSize()>1)?"UP":NULL;
				case 8: return (data->GetListSize()>1)?"DWN":NULL;
				case 9: return (data->GetListSize()>6)?"NXT":NULL;
				case 10: return (data->GetListSize()>6)?"PRV":NULL;
				case 11: return "SCN";
				default: return ((-page-1)*6+bt<data->GetListSize())?"==>":NULL;
			}
		}
		break;
	}
	return NULL;
}

// Return button menus
int AscensionTower::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static MFDBUTTONMENU mnu[12];
	if (menu) *menu = mnu;
	int page=data->GetPage();
	switch (page)
	{
	case 0:
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		if (page<0)
		{
			int size=data->GetListSize();
			int k=size+(page+1)*6;

			for(int i=0;i<k;i++)
			{
				mnu[i].line1="Select base";
				mnu[i].line2="next to the button";
				mnu[i].selchar=0;
			}
			for(int i=k;i<6;i++)
			{
				mnu[i].line1=NULL;
				mnu[i].line2=NULL;
				mnu[i].selchar=0;
			}
			
			if (size>1)
			{
				mnu[6].line1="Select currently";
				mnu[6].line2="marked base";
				mnu[6].selchar='S';
				mnu[7].line1="Move marker up";
				mnu[7].line2=NULL;
				mnu[7].selchar='U';
				mnu[8].line1="Move marker down";
				mnu[8].line2=NULL;
				mnu[8].selchar='D';
			}
			else for(int i=6;i<9;i++)
			{
				mnu[i].line1=NULL;
				mnu[i].line2=NULL;
				mnu[i].selchar=0;
			}

			if (size>6)
			{
				mnu[9].line1="Switch to next";
				mnu[9].line2="page";
				mnu[9].selchar='N';
				mnu[10].line1="Switch to previous";
				mnu[10].line2="page";
				mnu[10].selchar='P';
			}
			else for(int i=9;i<11;i++)
			{
				mnu[i].line1=NULL;
				mnu[i].line2=NULL;
				mnu[i].selchar=0;
			}

			mnu[11].line1="Scan for changes";
			mnu[11].line2=NULL;
			mnu[11].selchar='C';
			
			return 12;			
		}
		break;
	}
	return 0;
}

bool AscensionTower::WriteMFD(char *text, int line, int column, bool halfLines, bool rightAligned)
{
	int l=strlen(text);			
	if (line<0) return TextOut(hDC, (1+(37-l)/2)*width, 13*height, text, l);
	int y=(line*height) >> (halfLines?1:0);
	if (column<0 && !rightAligned) return TextOut(hDC, (1+(37-l)/2)*width, y, text, l);
	if (rightAligned) return TextOut(hDC, (37-l)*width, y, text, l);
	TextOut(hDC, column*width, y, text, l);
}

// Repaint the MFD
void AscensionTower::Update (HDC hDC)
{
	static int atButton[6]={8, 16, 24, 33, 41, 50}; //Best choice for certain MFD size in half-height units

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
	AscensionUltra *ascension=data->GetAscension();
	int page=data->GetPage();	

	if (page<0)
	{
		if (data->StartList())
		{
			//Base selection screens
			//Descriptions (normal, light green)
			SelectDefaultFont (hDC, 0);			
			int i=data->GetListSize();
			int pages=(i+5)/6;
			if (-page>pages) data->SetPage(page=-pages);
			for (i=-1;i>page;i--) for (int j=0;j<6;j++) data->NextList();			
			i=0;
			do
			{
				sprintf(line, "[%d] %s", data->GetListIndex(), data->GetListName());				
				WriteMFD(line, atButton[i++], 1, true);
			}
			while (data->NextList() && i<6);
			if (pages>1)
			{
				sprintf(line, "p.%d/%d", -page, pages);
				WriteMFD(line, 27, NULL, false, true);
			}

			Title (hDC, "Ascension Tower: select...");
		}
		else
		{
			//No bases available
			//Descriptions (normal, light green)
			SelectDefaultFont (hDC, 0);
			WriteMFD("N O   B A S E S   A V A I L A B L E");
			Title (hDC, "Ascension Tower");
		}
	}
	else
	{
		//Base interaction screens
		//Descriptions (normal, light green)
		SelectDefaultFont (hDC, 0);

		sprintf(line, "Ascension Tower: %s", ascension->GetName());
		Title (hDC, line);
	}

	if (pageRE!=page) InvalidateButtons();
	pageRE=page;
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