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
#include "VirtualDockingTunnel.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))
#define TA1MATRIXOFFSET _V(266,0,0)
#define TA1OFFSET _V(-2242,0,580)
#define OFFSET _V(2700,0,-950)
#define _V_(x,y,z) _V(-x,y,z)+OFFSET
#define LS1OFFSET _V(-855,0,480)
#define LS1MATRIXOFFSET _V(70,0,0)
#define MAXSTATICRUNWAYLINES 14
#define STATICRUNWAYLINESSPLIT 7
#define RUNWAYENDPOINTS 14
#define LFMCOFFSET _V(-1463,0,1260)

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
	modelidx = (fmodel ? 1 : 0);
	visual            = NULL;
	campos            = CAM_GENERIC;

	cur_TurnAround=-1;
	cur_LightStorage=-1;

	cur_Path=0;
	cur_Section=0;

	coords=false;
}

// --------------------------------------------------------------
// Destructor
// --------------------------------------------------------------
AscensionUltra::~AscensionUltra ()
{
	OrbiterExtensions::Exit(this);
}

double AscensionUltra::GetVersion(){return 1.0;}

void AscensionUltra::InitSubObjects()
{
	int i;
	char prefix[14]="HANGARx";
	char name[40]=" x. Turn-around Hangar";
	for(i=0;i<5;i++)
	{
		prefix[6]=0x30+i;
		name[1]=0x31+i;
		turnAround[i].Init(this, name, i+3, prefix);
	}
	strcpy(prefix, "LIGHTSTORAGEx");
	strcpy(name, " x. Light Storage Hangar");
	for(i=0;i<12;i++)
	{
		prefix[12]=0x30+i;
		name[1]=0x30+((i+1) % 10);
		if (i>8) name[0]=0x31;
		lightStorage[i].Init(this, name, i+8, prefix);
	}
	strcpy(prefix, "LAUNCHTUNNEL");
	strcpy(name, "Launch Tunnel");
	launchTunnel.Init(this, name, 20, prefix);

	strcpy(prefix, "AIRPORT");
	strcpy(name, "Airport");
	airport.Init(this, name, -1, prefix); //TODO: mesh index currently not set
	crew=airport.GetEntrance()->GetCrew();

	controlRoom=turnAround[0].GetRoom(0);

	//Initialize Orbiter extensions
	orbiterExtensionsResult=OrbiterExtensions::Init(this);

	if ((orbiterExtensionsVersion=OrbiterExtensions::GetVersion())<0) orbiterExtensionsVersion=0.0;

	//Generated subsection table by Excel
	taxiwaySubsection[0].Init(this, _V_(110,0,395), _V_(940,0,395), _V(0,1,0), 42);
	taxiwaySubsection[1].Init(this, _V_(980,0,395), _V_(2430,0,395), _V(0,1,0), 73);
	taxiwaySubsection[2].Init(this, _V_(2442,0,398), _V_(2452,0,408), _V(0,1,0), 2);
	taxiwaySubsection[3].Init(this, _V_(2455,0,420), _V_(2455,0,740), _V(0,1,0), 16);
	taxiwaySubsection[4].Init(this, _V_(2455,0,790), _V_(2455,0,880), _V(0,1,0), 5);
	taxiwaySubsection[5].Init(this, _V_(2455,0,925), _V_(2455,0,1015), _V(0,1,0), 5);
	taxiwaySubsection[6].Init(this, _V_(4460,0,1015), _V_(4460,0,925), _V(0,1,0), 5);
	taxiwaySubsection[7].Init(this, _V_(4460,0,880), _V_(4460,0,790), _V(0,1,0), 5);
	taxiwaySubsection[8].Init(this, _V_(4460,0,740), _V_(4460,0,430), _V(0,1,0), 16);
	taxiwaySubsection[9].Init(this, _V_(4463,0,418), _V_(4473,0,408), _V(0,1,0), 2);
	taxiwaySubsection[10].Init(this, _V_(4485,0,405), _V_(5295,0,405), _V(0,1,0), 41);
	taxiwaySubsection[11].Init(this, _V_(5307,0,408), _V_(5317,0,418), _V(0,1,0), 2);
	taxiwaySubsection[12].Init(this, _V_(5320,0,430), _V_(5320,0,740), _V(0,1,0), 16);
	taxiwaySubsection[13].Init(this, _V_(5320,0,790), _V_(5320,0,880), _V(0,1,0), 5);
	taxiwaySubsection[14].Init(this, _V_(5320,0,925), _V_(5320,0,1015), _V(0,1,0), 5);
	taxiwaySubsection[15].Init(this, _V_( 763,0,1015), _V_(763,0,925), _V(0,1,0), 5);
	taxiwaySubsection[16].Init(this, _V_( 763,0,880), _V_(763,0,790), _V(0,1,0), 5);
	taxiwaySubsection[17].Init(this, _V_(766,0,778), _V_( 776,0,768), _V(0,1,0), 2);
	taxiwaySubsection[18].Init(this, _V_( 788,0,765), _V_(940,0,765), _V(0,1,0), 8);
	taxiwaySubsection[19].Init(this, _V_(990,0,765), _V_(2430,0,765), _V(0,1,0), 73);
	taxiwaySubsection[20].Init(this, _V_(2480,0,765), _V_(4435,0,765), _V(0,1,0), 98);
	taxiwaySubsection[21].Init(this, _V_(4485,0,765), _V_(5295,0,765), _V(0,1,0), 41);
	taxiwaySubsection[22].Init(this, _V_(5345,0,765), _V_(5688,0,765), _V(0,1,0), 18);
	taxiwaySubsection[23].Init(this, _V_(5700,0,768), _V_(5710,0,778), _V(0,1,0), 2);
	taxiwaySubsection[24].Init(this, _V_(5713,0,790), _V_(5713,0,880), _V(0,1,0), 5);
	taxiwaySubsection[25].Init(this, _V_(5713,0,925), _V_(5713,0,1015), _V(0,1,0), 5);
	taxiwaySubsection[26].Init(this, _V_( 965,0,420), _V_(965,0,740), _V(0,1,0), 16);
	taxiwaySubsection[27].Init(this, _V_(965,0,790), _V_(965,0,880), _V(0,1,0), 5);
	taxiwaySubsection[28].Init(this, _V_( 965,0,925), _V_(965,0,1015), _V(0,1,0), 5);
	taxiwaySubsection[29].Init(this, _V_(965,0,1095), _V_(965,0,1235), _V(0,1,0), 7);
	taxiwaySubsection[30].Init(this, _V_(968,0,1247), _V_(978,0,1257), _V(0,1,0), 2);
	taxiwaySubsection[31].Init(this, _V_(990,0,1260), _V_(1177,0,1260), _V(0,1,0), 10);

	for(int i=0;i<TAXIWAYPATHS;i++) taxiwayPath[i].Init(this, NULL, _V(0,0,0), 0, 0);

	//Generated path table by Excel
	taxiwayPath[0].Add(&taxiwaySubsection[0]);	taxiwayPath[0].Add(&taxiwaySubsection[1]);																				
	taxiwayPath[1].Add(&taxiwaySubsection[0]);	taxiwayPath[1].Add(&taxiwaySubsection[1]);	taxiwayPath[1].Add(&taxiwaySubsection[2]);	taxiwayPath[1].Add(&taxiwaySubsection[3]);	taxiwayPath[1].Add(&taxiwaySubsection[20]);	taxiwayPath[1].Add(&taxiwaySubsection[8]);	taxiwayPath[1].Add(&taxiwaySubsection[9]);	taxiwayPath[1].Add(&taxiwaySubsection[10]);														
	taxiwayPath[2].Add(&taxiwaySubsection[0]);	taxiwayPath[2].Add(&taxiwaySubsection[26]);	taxiwayPath[2].Add(&taxiwaySubsection[27]);	taxiwayPath[2].Add(&taxiwaySubsection[28]);	taxiwayPath[2].Add(&taxiwaySubsection[29]);	taxiwayPath[2].Add(&taxiwaySubsection[30]);	taxiwayPath[2].Add(&taxiwaySubsection[31]);															
	taxiwayPath[3].Add(&taxiwaySubsection[19], true);	taxiwayPath[3].Add(&taxiwaySubsection[26], true);	taxiwayPath[3].Add(&taxiwaySubsection[1], true);	taxiwayPath[3].Add(&taxiwaySubsection[0], true);																		
	taxiwayPath[4].Add(&taxiwaySubsection[1]);	taxiwayPath[4].Add(&taxiwaySubsection[2]);	taxiwayPath[4].Add(&taxiwaySubsection[3]);	taxiwayPath[4].Add(&taxiwaySubsection[19]);	taxiwayPath[4].Add(&taxiwaySubsection[20]);	taxiwayPath[4].Add(&taxiwaySubsection[8]);	taxiwayPath[4].Add(&taxiwaySubsection[9]);	taxiwayPath[4].Add(&taxiwaySubsection[10]);														
	taxiwayPath[5].Add(&taxiwaySubsection[19], true);	taxiwayPath[5].Add(&taxiwaySubsection[1], true);	taxiwayPath[5].Add(&taxiwaySubsection[26]);	taxiwayPath[5].Add(&taxiwaySubsection[27]);	taxiwayPath[5].Add(&taxiwaySubsection[28]);	taxiwayPath[5].Add(&taxiwaySubsection[29]);	taxiwayPath[5].Add(&taxiwaySubsection[30]);	taxiwayPath[5].Add(&taxiwaySubsection[31]);														
	taxiwayPath[6].Add(&taxiwaySubsection[10], true);	taxiwayPath[6].Add(&taxiwaySubsection[9], true);	taxiwayPath[6].Add(&taxiwaySubsection[8], true);	taxiwayPath[6].Add(&taxiwaySubsection[20], true);	taxiwayPath[6].Add(&taxiwaySubsection[19], true);																	
	taxiwayPath[7].Add(&taxiwaySubsection[10], true);	taxiwayPath[7].Add(&taxiwaySubsection[9], true);	taxiwayPath[7].Add(&taxiwaySubsection[8], true);	taxiwayPath[7].Add(&taxiwaySubsection[20], true);	taxiwayPath[7].Add(&taxiwaySubsection[19], true);	taxiwayPath[7].Add(&taxiwaySubsection[26], true);	taxiwayPath[7].Add(&taxiwaySubsection[0], true);															
	taxiwayPath[8].Add(&taxiwaySubsection[10], true);	taxiwayPath[8].Add(&taxiwaySubsection[9], true);	taxiwayPath[8].Add(&taxiwaySubsection[8], true);	taxiwayPath[8].Add(&taxiwaySubsection[20], true);	taxiwayPath[8].Add(&taxiwaySubsection[19], true);	taxiwayPath[8].Add(&taxiwaySubsection[27]);	taxiwayPath[8].Add(&taxiwaySubsection[28]);	taxiwayPath[8].Add(&taxiwaySubsection[29]);	taxiwayPath[8].Add(&taxiwaySubsection[30]);	taxiwayPath[8].Add(&taxiwaySubsection[31]);												
	taxiwayPath[9].Add(&taxiwaySubsection[16]);	taxiwayPath[9].Add(&taxiwaySubsection[17]);	taxiwayPath[9].Add(&taxiwaySubsection[18]);	taxiwayPath[9].Add(&taxiwaySubsection[27], true);	taxiwayPath[9].Add(&taxiwaySubsection[19]);	taxiwayPath[9].Add(&taxiwaySubsection[4], true);	taxiwayPath[9].Add(&taxiwaySubsection[3], true);	taxiwayPath[9].Add(&taxiwaySubsection[2], true);	taxiwayPath[9].Add(&taxiwaySubsection[1], true);	taxiwayPath[9].Add(&taxiwaySubsection[7]);	taxiwayPath[9].Add(&taxiwaySubsection[20], true);	taxiwayPath[9].Add(&taxiwaySubsection[13], true);	taxiwayPath[9].Add(&taxiwaySubsection[21], true);	taxiwayPath[9].Add(&taxiwaySubsection[24], true);	taxiwayPath[9].Add(&taxiwaySubsection[23], true);	taxiwayPath[9].Add(&taxiwaySubsection[22], true);						
	taxiwayPath[10].Add(&taxiwaySubsection[16]);	taxiwayPath[10].Add(&taxiwaySubsection[17]);	taxiwayPath[10].Add(&taxiwaySubsection[18]);	taxiwayPath[10].Add(&taxiwaySubsection[27], true);	taxiwayPath[10].Add(&taxiwaySubsection[26], true);	taxiwayPath[10].Add(&taxiwaySubsection[0], true);	taxiwayPath[10].Add(&taxiwaySubsection[4], true);	taxiwayPath[10].Add(&taxiwaySubsection[19], true);	taxiwayPath[10].Add(&taxiwaySubsection[7]);	taxiwayPath[10].Add(&taxiwaySubsection[20], true);	taxiwayPath[10].Add(&taxiwaySubsection[13], true);	taxiwayPath[10].Add(&taxiwaySubsection[21], true);	taxiwayPath[10].Add(&taxiwaySubsection[24], true);	taxiwayPath[10].Add(&taxiwaySubsection[23], true);	taxiwayPath[10].Add(&taxiwaySubsection[22], true);							
	taxiwayPath[11].Add(&taxiwaySubsection[16]);	taxiwayPath[11].Add(&taxiwaySubsection[17]);	taxiwayPath[11].Add(&taxiwaySubsection[18]);	taxiwayPath[11].Add(&taxiwaySubsection[27], true);	taxiwayPath[11].Add(&taxiwaySubsection[19]);	taxiwayPath[11].Add(&taxiwaySubsection[4], true);	taxiwayPath[11].Add(&taxiwaySubsection[20]);	taxiwayPath[11].Add(&taxiwaySubsection[7]);	taxiwayPath[11].Add(&taxiwaySubsection[8]);	taxiwayPath[11].Add(&taxiwaySubsection[9]);	taxiwayPath[11].Add(&taxiwaySubsection[10]);	taxiwayPath[11].Add(&taxiwaySubsection[13], true);	taxiwayPath[11].Add(&taxiwaySubsection[21], true);	taxiwayPath[11].Add(&taxiwaySubsection[24], true);	taxiwayPath[11].Add(&taxiwaySubsection[23], true);	taxiwayPath[11].Add(&taxiwaySubsection[22], true);						
	taxiwayPath[12].Add(&taxiwaySubsection[16]);	taxiwayPath[12].Add(&taxiwaySubsection[17]);	taxiwayPath[12].Add(&taxiwaySubsection[18]);	taxiwayPath[12].Add(&taxiwaySubsection[27], true);	taxiwayPath[12].Add(&taxiwaySubsection[19]);	taxiwayPath[12].Add(&taxiwaySubsection[4], true);	taxiwayPath[12].Add(&taxiwaySubsection[20]);	taxiwayPath[12].Add(&taxiwaySubsection[7]);	taxiwayPath[12].Add(&taxiwaySubsection[21]);	taxiwayPath[12].Add(&taxiwaySubsection[13], true);	taxiwayPath[12].Add(&taxiwaySubsection[24], true);	taxiwayPath[12].Add(&taxiwaySubsection[23], true);	taxiwayPath[12].Add(&taxiwaySubsection[22], true);									
	taxiwayPath[13].Add(&taxiwaySubsection[15]);	taxiwayPath[13].Add(&taxiwaySubsection[16]);	taxiwayPath[13].Add(&taxiwaySubsection[17]);	taxiwayPath[13].Add(&taxiwaySubsection[18]);	taxiwayPath[13].Add(&taxiwaySubsection[28], true);	taxiwayPath[13].Add(&taxiwaySubsection[27], true);	taxiwayPath[13].Add(&taxiwaySubsection[19]);	taxiwayPath[13].Add(&taxiwaySubsection[5], true);	taxiwayPath[13].Add(&taxiwaySubsection[4], true);	taxiwayPath[13].Add(&taxiwaySubsection[3], true);	taxiwayPath[13].Add(&taxiwaySubsection[2], true);	taxiwayPath[13].Add(&taxiwaySubsection[1], true);	taxiwayPath[13].Add(&taxiwaySubsection[6]);	taxiwayPath[13].Add(&taxiwaySubsection[7]);	taxiwayPath[13].Add(&taxiwaySubsection[20], true);	taxiwayPath[13].Add(&taxiwaySubsection[14], true);	taxiwayPath[13].Add(&taxiwaySubsection[13], true);	taxiwayPath[13].Add(&taxiwaySubsection[21], true);	taxiwayPath[13].Add(&taxiwaySubsection[25], true);	taxiwayPath[13].Add(&taxiwaySubsection[24], true);	taxiwayPath[13].Add(&taxiwaySubsection[23], true);	taxiwayPath[13].Add(&taxiwaySubsection[22], true);
	taxiwayPath[14].Add(&taxiwaySubsection[15]);	taxiwayPath[14].Add(&taxiwaySubsection[16]);	taxiwayPath[14].Add(&taxiwaySubsection[17]);	taxiwayPath[14].Add(&taxiwaySubsection[18]);	taxiwayPath[14].Add(&taxiwaySubsection[28], true);	taxiwayPath[14].Add(&taxiwaySubsection[27], true);	taxiwayPath[14].Add(&taxiwaySubsection[26], true);	taxiwayPath[14].Add(&taxiwaySubsection[0], true);	taxiwayPath[14].Add(&taxiwaySubsection[5], true);	taxiwayPath[14].Add(&taxiwaySubsection[4], true);	taxiwayPath[14].Add(&taxiwaySubsection[19], true);	taxiwayPath[14].Add(&taxiwaySubsection[6]);	taxiwayPath[14].Add(&taxiwaySubsection[7]);	taxiwayPath[14].Add(&taxiwaySubsection[20], true);	taxiwayPath[14].Add(&taxiwaySubsection[14], true);	taxiwayPath[14].Add(&taxiwaySubsection[13], true);	taxiwayPath[14].Add(&taxiwaySubsection[21], true);	taxiwayPath[14].Add(&taxiwaySubsection[25], true);	taxiwayPath[14].Add(&taxiwaySubsection[24], true);	taxiwayPath[14].Add(&taxiwaySubsection[23], true);	taxiwayPath[14].Add(&taxiwaySubsection[22], true);	
	taxiwayPath[15].Add(&taxiwaySubsection[15]);	taxiwayPath[15].Add(&taxiwaySubsection[16]);	taxiwayPath[15].Add(&taxiwaySubsection[17]);	taxiwayPath[15].Add(&taxiwaySubsection[18]);	taxiwayPath[15].Add(&taxiwaySubsection[28], true);	taxiwayPath[15].Add(&taxiwaySubsection[27], true);	taxiwayPath[15].Add(&taxiwaySubsection[19]);	taxiwayPath[15].Add(&taxiwaySubsection[5], true);	taxiwayPath[15].Add(&taxiwaySubsection[4], true);	taxiwayPath[15].Add(&taxiwaySubsection[20]);	taxiwayPath[15].Add(&taxiwaySubsection[6]);	taxiwayPath[15].Add(&taxiwaySubsection[7]);	taxiwayPath[15].Add(&taxiwaySubsection[8]);	taxiwayPath[15].Add(&taxiwaySubsection[9]);	taxiwayPath[15].Add(&taxiwaySubsection[10]);	taxiwayPath[15].Add(&taxiwaySubsection[14], true);	taxiwayPath[15].Add(&taxiwaySubsection[13], true);	taxiwayPath[15].Add(&taxiwaySubsection[21], true);	taxiwayPath[15].Add(&taxiwaySubsection[25], true);	taxiwayPath[15].Add(&taxiwaySubsection[24], true);	taxiwayPath[15].Add(&taxiwaySubsection[23], true);	taxiwayPath[15].Add(&taxiwaySubsection[22], true);
	taxiwayPath[16].Add(&taxiwaySubsection[15]);	taxiwayPath[16].Add(&taxiwaySubsection[16]);	taxiwayPath[16].Add(&taxiwaySubsection[17]);	taxiwayPath[16].Add(&taxiwaySubsection[18]);	taxiwayPath[16].Add(&taxiwaySubsection[28], true);	taxiwayPath[16].Add(&taxiwaySubsection[27], true);	taxiwayPath[16].Add(&taxiwaySubsection[19]);	taxiwayPath[16].Add(&taxiwaySubsection[5], true);	taxiwayPath[16].Add(&taxiwaySubsection[4], true);	taxiwayPath[16].Add(&taxiwaySubsection[20]);	taxiwayPath[16].Add(&taxiwaySubsection[6]);	taxiwayPath[16].Add(&taxiwaySubsection[7]);	taxiwayPath[16].Add(&taxiwaySubsection[21]);	taxiwayPath[16].Add(&taxiwaySubsection[14], true);	taxiwayPath[16].Add(&taxiwaySubsection[13], true);	taxiwayPath[16].Add(&taxiwaySubsection[25], true);	taxiwayPath[16].Add(&taxiwaySubsection[24], true);	taxiwayPath[16].Add(&taxiwaySubsection[23], true);	taxiwayPath[16].Add(&taxiwaySubsection[22], true);			
	taxiwayPath[17].Add(&taxiwaySubsection[21]);	taxiwayPath[17].Add(&taxiwaySubsection[12], true);	taxiwayPath[17].Add(&taxiwaySubsection[11], true);	taxiwayPath[17].Add(&taxiwaySubsection[10], true);																		
	taxiwayPath[18].Add(&taxiwaySubsection[21], true);	taxiwayPath[18].Add(&taxiwaySubsection[20], true);	taxiwayPath[18].Add(&taxiwaySubsection[19], true);	taxiwayPath[18].Add(&taxiwaySubsection[27]);																		
	taxiwayPath[19].Add(&taxiwaySubsection[21], true);	taxiwayPath[19].Add(&taxiwaySubsection[20], true);	taxiwayPath[19].Add(&taxiwaySubsection[19], true);	taxiwayPath[19].Add(&taxiwaySubsection[27]);	taxiwayPath[19].Add(&taxiwaySubsection[28]);																	
	taxiwayPath[20].Add(&taxiwaySubsection[21]);	taxiwayPath[20].Add(&taxiwaySubsection[13]);	taxiwayPath[20].Add(&taxiwaySubsection[14]);																			
	taxiwayPath[21].Add(&taxiwaySubsection[21]);	taxiwayPath[21].Add(&taxiwaySubsection[13]);																				

	VECTOR3 runwayLines[MAXSTATICRUNWAYLINES][2]=
	{
		//Generated lines by Excel - last comma removed
		{_V_(1010,0,880), _V_(2410,0,880)},
		{_V_(2500,0,880), _V_(4415,0,880)},
		{_V_(4505,0,880), _V_(5275,0,880)},
		{_V_(1010,0,925), _V_(2410,0,925)},
		{_V_(2500,0,925), _V_(4415,0,925)},
		{_V_(4505,0,925), _V_(5275,0,925)},
		{_V_(1010,0,902.5), _V_(5275,0,902.5)},
		{_V_(1010,0,1015), _V_(2410,0,1015)},
		{_V_(2500,0,1015), _V_(4415,0,1015)},
		{_V_(4505,0,1015), _V_(5275,0,1015)},
		{_V_(1010,0,1095), _V_(2410,0,1095)},
		{_V_(2500,0,1095), _V_(4415,0,1095)},
		{_V_(4505,0,1095), _V_(5275,0,1095)},
		{_V_(1010,0,1055), _V_(5275,0,1055)}
	};

	int runwayBeacons[MAXSTATICRUNWAYLINES]=
	{
		//Generated lines by Excel - last comma removed
		70,
		96,
		40,
		70,
		96,
		40,
		285,
		70,
		96,
		40,
		70,
		96,
		40,
		285
	};

	//Generated subsection table by Excel
	runwaySubsection[0].Init(this, _V_(4669,0,880), _V_(5265,0,880), _V(1,1,0), 31);
	runwaySubsection[1].Init(this, _V_(4669,0,925), _V_(5265,0,925), _V(1,1,0), 31);
	runwaySubsection[2].Init(this, _V_(4967.5,0,902.5), _V_(5267.5,0,902.5), _V(1,0,0), 20);
	runwaySubsection[3].Init(this, _V_(1011,0,880), _V_(1011,0,925), _V(0,0,1), 18);
	runwaySubsection[4].Init(this, _V_(5400,0,880), _V_(5400,0,925), _V(1,0,0), 9);
	runwaySubsection[5].Init(this, _V_(5775,0,880), _V_(5775,0,925), _V(1,0,0), 18);
	runwaySubsection[6].Init(this, _V_(738,0,880), _V_(1010,0,880), _V(1,1,0), 14);
	runwaySubsection[7].Init(this, _V_(738,0,925), _V_(1010,0,925), _V(1,1,0), 14);
	runwaySubsection[8].Init(this, _V_(700,0,902.5), _V_(1010,0,902.5), _V(1,1,0), 20);
	runwaySubsection[9].Init(this, _V_(700,0,895), _V_(1010,0,895), _V(1,0,0), 20);
	runwaySubsection[10].Init(this, _V_(700,0,898), _V_(1010,0,898), _V(1,0,0), 20);
	runwaySubsection[11].Init(this, _V_(700,0,907), _V_(1010,0,907), _V(1,0,0), 20);
	runwaySubsection[12].Init(this, _V_(700,0,910), _V_(1010,0,910), _V(1,0,0), 20);
	runwaySubsection[13].Init(this, _V_(80,0,902.5), _V_(700,0,902.5), _V(1,1,1), 25);
	runwaySubsection[14].Init(this, _V_(4669,0,1015), _V_(5265,0,1015), _V(1,1,0), 31);
	runwaySubsection[15].Init(this, _V_(4669,0,1095), _V_(5265,0,1095), _V(1,1,0), 31);
	runwaySubsection[16].Init(this, _V_(4967.5,0,1055), _V_(5267.5,0,1055), _V(1,0,0), 20);
	runwaySubsection[17].Init(this, _V_(1011,0,1015), _V_(1011,0,1095), _V(0,0,1), 18);
	runwaySubsection[18].Init(this, _V_(5400,0,1015), _V_(5400,0,1095), _V(1,0,0), 9);
	runwaySubsection[19].Init(this, _V_(5775,0,1015), _V_(5775,0,1095), _V(1,0,0), 18);
	runwaySubsection[20].Init(this, _V_(738,0,1015), _V_(1010,0,1015), _V(1,1,0), 14);
	runwaySubsection[21].Init(this, _V_(738,0,1015), _V_(1010,0,1095), _V(1,1,0), 14);
	runwaySubsection[22].Init(this, _V_(700,0,1055), _V_(1010,0,1055), _V(1,1,0), 20);
	runwaySubsection[23].Init(this, _V_(700,0,1035), _V_(1010,0,1035), _V(1,0,0), 20);
	runwaySubsection[24].Init(this, _V_(700,0,1040), _V_(1010,0,1040), _V(1,0,0), 20);
	runwaySubsection[25].Init(this, _V_(700,0,1070), _V_(1010,0,1070), _V(1,0,0), 20);
	runwaySubsection[26].Init(this, _V_(700,0,1075), _V_(1010,0,1075), _V(1,0,0), 20);
	runwaySubsection[27].Init(this, _V_(80,0,1055), _V_(700,0,1055), _V(1,1,1), 25);
	runwaySubsection[28].Init(this, _V_(1020,0,1015), _V_(1620,0,1015), _V(1,1,0), 30);
	runwaySubsection[29].Init(this, _V_(1020,0,1095), _V_(1620,0,1095), _V(1,1,0), 30);
	runwaySubsection[30].Init(this, _V_(4967.5,0,1055), _V_(5267.5,0,1055), _V(1,0,0), 19);
	runwaySubsection[31].Init(this, _V_(5274,0,1015), _V_(5274,0,1095), _V(0,0,1), 18);
	runwaySubsection[32].Init(this, _V_(1011,0,1015), _V_(1011,0,1095), _V(1,0,0), 9);
	runwaySubsection[33].Init(this, _V_(738,0,1015), _V_(738,0,1095), _V(1,0,0), 18);
	runwaySubsection[34].Init(this, _V_(5274,0,1015), _V_(5775,0,1015), _V(1,1,0), 25);
	runwaySubsection[35].Init(this, _V_(5274,0,1095), _V_(5775,0,1095), _V(1,1,0), 25);
	runwaySubsection[36].Init(this, _V_(5274,0,1055), _V_(5584,0,1055), _V(1,1,0), 20);
	runwaySubsection[37].Init(this, _V_(5274,0,1035), _V_(5584,0,1035), _V(1,0,0), 20);
	runwaySubsection[38].Init(this, _V_(5274,0,1040), _V_(5584,0,1040), _V(1,0,0), 20);
	runwaySubsection[39].Init(this, _V_(5274,0,1070), _V_(5584,0,1070), _V(1,0,0), 20);
	runwaySubsection[40].Init(this, _V_(5274,0,1075), _V_(5584,0,1075), _V(1,0,0), 20);
	runwaySubsection[41].Init(this, _V_(6205,0,1055), _V_(5429,0,1055), _V(1,1,1), 25);
	runwaySubsection[42].Init(this, _V_(1020,0,880), _V_(1620,0,880), _V(1,1,0), 30);
	runwaySubsection[43].Init(this, _V_(1020,0,925), _V_(1620,0,925), _V(1,1,0), 30);
	runwaySubsection[44].Init(this, _V_(1017.5,0,902.5), _V_(1302.5,0,902.5), _V(1,0,0), 19);
	runwaySubsection[45].Init(this, _V_(5274,0,880), _V_(5274,0,925), _V(0,0,1), 18);
	runwaySubsection[46].Init(this, _V_(1011,0,880), _V_(1011,0,925), _V(1,0,0), 9);
	runwaySubsection[47].Init(this, _V_(738,0,880), _V_(738,0,925), _V(1,0,0), 18);
	runwaySubsection[48].Init(this, _V_(5274,0,880), _V_(5775,0,880), _V(1,1,0), 25);
	runwaySubsection[49].Init(this, _V_(5274,0,925), _V_(5775,0,925), _V(1,1,0), 25);
	runwaySubsection[50].Init(this, _V_(5274,0,902.5), _V_(5584,0,902.5), _V(1,1,0), 20);
	runwaySubsection[51].Init(this, _V_(5274,0,895), _V_(5584,0,895), _V(1,0,0), 20);
	runwaySubsection[52].Init(this, _V_(5274,0,898), _V_(5584,0,898), _V(1,0,0), 20);
	runwaySubsection[53].Init(this, _V_(5274,0,907), _V_(5584,0,907), _V(1,0,0), 20);
	runwaySubsection[54].Init(this, _V_(5274,0,910), _V_(5584,0,910), _V(1,0,0), 20);
	runwaySubsection[55].Init(this, _V_(6205,0,902.5), _V_(5429,0,902.5), _V(1,1,1), 25);

	runwayPath[0].Init(this, (VECTOR3 *)runwayLines, _V(1,1,1), runwayBeacons, STATICRUNWAYLINESSPLIT);

	runwayPath[1].Init(this, (VECTOR3 *)(runwayLines+STATICRUNWAYLINESSPLIT), _V(1,1,1), runwayBeacons+STATICRUNWAYLINESSPLIT, MAXSTATICRUNWAYLINES-STATICRUNWAYLINESSPLIT);

	runwayPath[2].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=0;i<13;i++) runwayPath[2].Add(&runwaySubsection[i]);

	runwayPath[3].Init(this, NULL, _V(0,0,0), NULL, 0);
	runwayPath[3].Add(&runwaySubsection[13]);

	runwayPath[4].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=14;i<27;i++) runwayPath[4].Add(&runwaySubsection[i]);

	runwayPath[5].Init(this, NULL, _V(0,0,0), NULL, 0);
	runwayPath[5].Add(&runwaySubsection[27]);

	runwayPath[6].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=28;i<41;i++) runwayPath[6].Add(&runwaySubsection[i]);

	runwayPath[7].Init(this, NULL, _V(0,0,0), NULL, 0);
	runwayPath[7].Add(&runwaySubsection[41]);

	runwayPath[8].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=42;i<55;i++) runwayPath[8].Add(&runwaySubsection[i]);

	runwayPath[9].Init(this, NULL, _V(0,0,0), NULL, 0);
	runwayPath[9].Add(&runwaySubsection[55]);

	static char *points[RUNWAYENDPOINTS]=
	{
		"Launch",
		"Storage Hangars",
		"TA Hangars",
		"Aux Hangars",
		"Runway 13L",
		"Runway 13R",
		"Runway 31L",
		"Runway 31R",
		"Airport",
		"Runway 13L/31R",
		"Runway 31L/13R",
		"Static",
		"Non-Static",
		"Lead-In"
	};

	//Generated subsection table by Excel
	taxiways.Init(1.4, 0.8, 2, 0.3, -0.2);
	taxiways.Add(&taxiwayPath[0], points[1], points[2], true);
	taxiways.Add(&taxiwayPath[1], points[1], points[3], true);
	taxiways.Add(&taxiwayPath[2], points[1], points[0], true);
	taxiways.Add(&taxiwayPath[3], points[2], points[1], true);
	taxiways.Add(&taxiwayPath[4], points[2], points[3], true);
	taxiways.Add(&taxiwayPath[5], points[2], points[0], true);
	taxiways.Add(&taxiwayPath[6], points[3], points[2], true);
	taxiways.Add(&taxiwayPath[7], points[3], points[1], true);
	taxiways.Add(&taxiwayPath[8], points[3], points[0], true);
	taxiways.Add(&taxiwayPath[9], points[9], points[2], true);
	taxiways.Add(&taxiwayPath[10], points[9], points[1], true);
	taxiways.Add(&taxiwayPath[11], points[9], points[3], true);
	taxiways.Add(&taxiwayPath[12], points[9], points[8], true);
	taxiways.Add(&taxiwayPath[13], points[10], points[2], true);
	taxiways.Add(&taxiwayPath[14], points[10], points[1], true);
	taxiways.Add(&taxiwayPath[15], points[10], points[3], true);
	taxiways.Add(&taxiwayPath[16], points[10], points[8], true);
	taxiways.Add(&taxiwayPath[17], points[8], points[3], true);
	taxiways.Add(&taxiwayPath[18], points[8], points[4], true);
	taxiways.Add(&taxiwayPath[19], points[8], points[5], true);
	taxiways.Add(&taxiwayPath[20], points[8], points[6], true);
	taxiways.Add(&taxiwayPath[21], points[8], points[7], true);

	taxiways.Switch(true);

	runways.Init(1.4, 0.8, 2, 0.03, 0);
	runways.Add(&runwayPath[0], points[4], points[11], false);
	runways.Add(&runwayPath[2], points[4], points[12], false);
	runways.Add(&runwayPath[3], points[4], points[13], true);
	runways.Add(&runwayPath[1], points[5], points[11], false);
	runways.Add(&runwayPath[4], points[5], points[12], false);
	runways.Add(&runwayPath[5], points[5], points[13], true);
	runways.Add(&runwayPath[1], points[6], points[11], false);
	runways.Add(&runwayPath[6], points[6], points[12], false);
	runways.Add(&runwayPath[7], points[6], points[13], true);
	runways.Add(&runwayPath[0], points[7], points[11], false);
	runways.Add(&runwayPath[8], points[7], points[12], false);
	runways.Add(&runwayPath[9], points[7], points[13], true);
	runways.Switch(true);
}
// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void AscensionUltra::DefineAnimations ()
{
	for(int i=0;i<5;i++) turnAround[i].DefineAnimations();
	for(int i=0;i<12;i++) lightStorage[i].DefineAnimations();
	launchTunnel.DefineAnimations();
	airport.DefineAnimations();
}

void AscensionUltra::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
	// draw the default HUD
	VESSEL2::clbkDrawHUD (mode, hps, hDC);

	// TODO: draw vessel status	
}

// Set vessel class parameters
void AscensionUltra::clbkSetClassCaps (FILEHANDLE cfg)
{
	// *************** physical parameters **********************

	VESSEL2::SetEmptyMass (EMPTY_MASS);
	VECTOR3 r[2] = {{0,0,6}, {0,0,-4}};
	SetSize (5000);
	SetVisibilityLimit (7.5e-4, 1.5e-3);
	SetAlbedoRGB (_V(0.77,0.20,0.13));
	SetTouchdownPoints (_V(0,0,10), _V(-3.5,0,-3), _V(3.5,0,-3));
	SetCOG_elev(0.001);
	EnableTransponder (true);
	bool render_cockpit = true;

	// ******************** NAV radios **************************

	InitNavRadios (4);

	// ************************* mesh ***************************
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Island1"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Base_Signs"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Place_Holders"), &OFFSET), MESHVIS_ALWAYS);
	meshHangar = oapiLoadMeshGlobal ("AscensionUltra\\TA1-NW");
	meshWindow = oapiLoadMeshGlobal ("AscensionUltra\\TA1-WO");
	meshLightStorage = oapiLoadMeshGlobal ("AscensionUltra\\LS1-1");
	
	double curvoff[5]={-0.02,-0.05,-0.08,-0.12,-0.17};

	InitSubObjects();

	for(int i=0;i<5;i++)
	{
		VECTOR3 off=OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0);
		SetMeshVisibilityMode (AddMesh (meshHangar, &off), MESHVIS_ALWAYS);
		turnAround[i].SetPosition(off);
	}
	for(int i=0;i<12;i++)
	{
		VECTOR3 off=OFFSET+LS1OFFSET+LS1MATRIXOFFSET*i;
		SetMeshVisibilityMode (AddMesh (meshLightStorage, &off), MESHVIS_ALWAYS);
		lightStorage[i].SetPosition(off);
	}
	SetMeshVisibilityMode (AddMesh (meshLaunch = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_NW"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);
	for(int i=0;i<5;i++) SetMeshVisibilityMode (AddMesh (meshWindow, &(OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0))), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshLaunchWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_WO"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);

	DefineAnimations();

	int index=0;

	for(int i=0;i<5;i++) index=turnAround[i].InitActionAreas(crew, index);
	for(int i=0;i<12;i++) index=lightStorage[i].InitActionAreas(crew, index);
}

// Read status from scenario file
void AscensionUltra::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {		
		if (!strnicmp (line, "HANGAR", 6)) {
			sscanf (line+6, "%d", &cur_TurnAround);
		} else if (cur_TurnAround>=0 && cur_TurnAround<5) {
			if (!turnAround[cur_TurnAround].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!strnicmp (line, "LIGHTSTORAGE", 12)) {
			sscanf (line+12, "%X", &cur_LightStorage);
		} else if (cur_LightStorage>=0 && cur_LightStorage<12) {
			if (!lightStorage[cur_LightStorage].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!strnicmp (line, "LAUNCHTUNNEL", 12)) {
			sscanf (line+12, "%X", &cur_LaunchTunnel);
		} else if (cur_LaunchTunnel>=0 && cur_LaunchTunnel<1) {
			if (!launchTunnel.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!strnicmp (line, "AIRPORT", 7)) {
			sscanf (line+7, "%X", &cur_Airport);
		} else if (cur_Airport>=0 && cur_Airport<1) {
			if (!airport.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
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
		turnAround[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "HANGAR", cbuf);

	for(i=0;i<12;i++)
	{
		sprintf (cbuf, "%X", i);
		oapiWriteScenario_string (scn, "LIGHTSTORAGE", cbuf);
		lightStorage[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%X", i);
	oapiWriteScenario_string (scn, "LIGHTSTORAGE", cbuf);

	oapiWriteScenario_string (scn, "LAUNCHTUNNEL", "0");
	launchTunnel.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "LAUNCHTUNNEL", "1");

	oapiWriteScenario_string (scn, "AIRPORT", "0");
	airport.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "AIRPORT", "1");
	
}

// Finalise vessel creation
void AscensionUltra::clbkPostCreation ()
{
	SetEmptyMass (EMPTY_MASS);

	for(int i=0;i<5;i++) turnAround[i].clbkPostCreation();
	for(int i=0;i<12;i++) lightStorage[i].clbkPostCreation();
	launchTunnel.clbkPostCreation();
	airport.clbkPostCreation();
}

// Respond to playback event
bool AscensionUltra::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "HANGAR", 6))
	{
		//Hangar event
		int hangar=(int)(event_type+6)[0]-0x30;
		if (hangar>=0 && hangar<5) return turnAround[hangar].clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	if (!strnicmp (event_type, "LIGHTSTORAGE", 12))
	{
		//Hangar event
		int hangar=(int)(event_type+12)[0]-0x30;
		if (hangar>=0 && hangar<12) return lightStorage[hangar].clbkPlaybackEvent(simt, event_t, event_type+13, event);
	}
	if (!strnicmp (event_type, "LAUNCHTUNNEL", 12))
	{
		//Tunnel event
		return launchTunnel.clbkPlaybackEvent(simt, event_t, event_type+12, event);
	}
	if (!strnicmp (event_type, "AIRPORT", 7))
	{
		//Airport event
		return airport.clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	return false;
}

// Create DG visual
void AscensionUltra::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	//Rotate light storage hangars
	for(int i=0;i<12;i++) RotateGroup(i+5+3, PI, _V(0,1,0), _V(0,0,0));
	//Close tunnel door
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=20;
	mt.ngrp=3;
	mt.transform=mt.TRANSLATE;
	mt.P.transparam.shift=_V(47,0,0);	
	MeshgroupTransform(vis, mt);
}

// Destroy DG visual
void AscensionUltra::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	visual = NULL;
}

// --------------------------------------------------------------
// Frame update
// --------------------------------------------------------------
void AscensionUltra::clbkPostStep (double simt, double simdt, double mjd)
{
	//Call post steps of all sub-elements
	for(int i=0;i<5;i++) turnAround[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<12;i++) lightStorage[i].clbkPostStep(simt, simdt, mjd);
	launchTunnel.clbkPostStep(simt, simdt, mjd);
	airport.clbkPostStep(simt, simdt, mjd);

	//Check all virtual docks
	VECTOR3 global, local;
	std::map<Room*, VESSEL*> toBeDeleted;
	for(std::map<Room*, VESSEL*>::iterator i=roomVessel.begin();i!=roomVessel.end();i++)
	{	
		//Check ground contact
		if (!i->second->GroundContact())
		{
			toBeDeleted[i->first]=i->second;
			continue;
		}

		//Check vincinity
		i->second->GetGlobalPos(global);
		Global2Local(global, local);
		local-=OFFSET;
		if (!i->first->GetHangar()->CheckVincinity(&local)) toBeDeleted[i->first]=i->second;
	}
	for(std::map<Room*, VESSEL*>::iterator i=toBeDeleted.begin();i!=toBeDeleted.end();i++)
	{
		roomVessel.erase(i->first);
		vesselRoom.erase(i->second);
		OrbiterExtensions::SetDockState(i->second, NULL);
		i->first->SetDock(NULL);
	}

	//Detect activated action area, iterate through sub-items for processing, break on first processor
	int action=crew->DetectActionAreaActivated();
	if (action>-1) for(int i=0;i<5;i++) if (turnAround[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) for(int i=0;i<12;i++) if (lightStorage[i].ActionAreaActivated(action)) break;

	//DEBUG relative position
	if (coords)
	{
		VECTOR3 global, local;
		oapiGetFocusInterface()->GetGlobalPos(global);
		Global2Local(global, local);
		local-=OFFSET+LFMCOFFSET;
		sprintf(oapiDebugString(),"MAP coordinates: %f , %f , %f", -local.x, local.y, local.z);
	}
	
	if (orbiterExtensionsResult<0) sprintf(oapiDebugString(), "WARNING! Orbiter extensions not loaded. Error %d", orbiterExtensionsResult);
	else crew->WarnUserUMMUNotInstalled("Ascension Ultra");
}

bool AscensionUltra::clbkLoadGenericCockpit ()
{
	SwitchView(controlRoom);	
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
		}
	} else {
		switch (key) {
		case OAPI_KEY_SPACE: // dummy key
			taxiwayPath[cur_Path].Switch(!taxiwayPath[cur_Path].On());
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_D:
			if (++cur_Path>=TAXIWAYPATHS) cur_Path=0;
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_S:
			taxiwayPath[cur_Path].SetPeriod(-taxiwayPath[cur_Path].GetPeriod());
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_W: // dummy key
			taxiwaySubsection[cur_Section].Switch(!taxiwaySubsection[cur_Section].On());
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_E:
			if (++cur_Section>=TAXIWAYSUBSECTIONS) cur_Section=0;
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_Q:
			if (--cur_Section<0) cur_Section=TAXIWAYSUBSECTIONS-1;
			sprintf(oapiDebugString(), "[%d]%s [%d]%s", cur_Path, taxiwayPath[cur_Path].On()?"ON":"off", cur_Section, taxiwaySubsection[cur_Section].On()?"ON":"off");
			return 1;
		case OAPI_KEY_C:
			coords=!coords;
			return 1;
		}
	}
	return 0;
}

void AscensionUltra::MoveGroup(int mesh, VECTOR3 v)
{
	//Transfer mesh groups	
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=mesh;
	mt.transform=mt.TRANSLATE;
	mt.P.transparam.shift=v;
	int k=oapiMeshGroupCount(GetMesh(visual, mesh));
	for(mt.ngrp=0;mt.ngrp<k;mt.ngrp++) MeshgroupTransform(visual, mt);	
}

void AscensionUltra::RotateGroup(int mesh, float angle, VECTOR3 v, VECTOR3 ref)
{
	//Transfer mesh groups	
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=mesh;
	mt.transform=mt.ROTATE;
	mt.P.rotparam.angle=angle;
	mt.P.rotparam.axis=v;
	mt.P.rotparam.ref=ref;
	int k=oapiMeshGroupCount(GetMesh(visual, mesh));
	for(mt.ngrp=0;mt.ngrp<k;mt.ngrp++) MeshgroupTransform(visual, mt);	
}

int AscensionUltra::GetHangars(int type)
{
	int count=0;
	if ((type & HANGARTYPETA)>0) count+=5;
	if ((type & HANGARTYPELS)>0) count+=12;
	if ((type & HANGARTYPELFMC)>0) count+=1;	
	return count;
}

Hangar *AscensionUltra::GetHangar(int type, int index)
{
	if (index<0) return NULL;
	if ((type & HANGARTYPETA)>0)
	{
		if (index<TURNAROUNDHANGARS) return turnAround+index;
		else index-=TURNAROUNDHANGARS;
	}
	if ((type & HANGARTYPELS)>0)
	{
		if (index<LIGHTSTORAGEHANGARS) return lightStorage+index;
		else index-=LIGHTSTORAGEHANGARS;
	}
	if ((type & HANGARTYPELFMC)>0)
	{
		if (index<1) return &launchTunnel;
		else index-=1;
	}
	if ((type & HANGARTYPEPORT)>0)
	{
		if (index<1) return &airport;
		else index-=1;
	}
	return NULL;
}

Routes *AscensionUltra::GetTaxiways(){return &taxiways;}

Routes *AscensionUltra::GetRunways(){return &runways;}

Room *AscensionUltra::GetControlRoom(){return controlRoom;}
void AscensionUltra::SwitchView(Room *room)
{
	SetCameraOffset (room->GetHangar()->GetPosition()+room->GetCameraPosition());
	SetCameraDefaultDirection(room->GetViewDirection());
	if (oapiGetFocusObject()==GetHandle()) oapiCameraSetCockpitDir(0,0);
	controlRoom=room;
}

int AscensionUltra::GetPersons()
{
	int i, rooms, j, persons=0;
		
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		rooms=turnAround[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=turnAround[i].GetRoom(j)->GetCrew()->GetCrewTotalNumber();
	}

	for(i=0;i<LIGHTSTORAGEHANGARS;i++)
	{
		rooms=lightStorage[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=lightStorage[i].GetRoom(j)->GetCrew()->GetCrewTotalNumber();
	}

	rooms=launchTunnel.GetRooms();
	for(j=0;j<rooms;j++) persons+=launchTunnel.GetRoom(j)->GetCrew()->GetCrewTotalNumber();	

	rooms=airport.GetRooms();
	for(j=0;j<rooms;j++) persons+=airport.GetRoom(j)->GetCrew()->GetCrewTotalNumber();	

	return ++persons;	//First entry is always the ADD PERSON entry
}

Room* AscensionUltra::GetPersonLocationFromHangar(int &index, Hangar *hangar)
{	
	UMMUCREWMANAGMENT *crew;
	int rooms=hangar->GetRooms();
	for(int j=0;j<rooms;j++)
	{
		Room *room=hangar->GetRoom(j);
		crew=room->GetCrew();
		if (crew->GetCrewTotalNumber()>index) return room;
		index-=crew->GetCrewTotalNumber();
	}
	return NULL;
}

Room *AscensionUltra::GetPersonLocation(int &index)
{
	if (index==0) return airport.GetEntrance();
	index--;

	Room *room;
			
	for(int i=0;i<TURNAROUNDHANGARS;i++) if ((room = GetPersonLocationFromHangar(index, &turnAround[i]))!=NULL) return room;
	//for(int i=0;i<LIGHTSTORAGEHANGARS;i++) if ((room = GetPersonLocationFromHangar(index, &lightStorage[i]))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &launchTunnel))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &airport))!=NULL) return room;
	
	index=0;
	return NULL;	
}

Person AscensionUltra::GetPerson(int index)
{
	Room *room=GetPersonLocation(index);
	return Person(room, index);
}

// Changes person properties according to flags (see header-file for PERSON_xxx macros)
// Returns -2 on internal failure, -1 if no more slots available on location change,
// 0 on success of EVA or remove, new index otherwise
int AscensionUltra::ChangePerson(int index, int flags, ...)
{	
	Person person=GetPerson(index);
	UMMUCREWMANAGMENT *crew=person.Location->GetCrew();
	int result=0;

	switch (flags)
	{
	case PERSON_EVA:
		crew->EvaCrewMember(person.Name);
		break;
	case PERSON_DELETE:
		crew->RemoveCrewMember(person.Name);		
		break;
	default:
		va_list args;
		va_start(args, flags);

		//Set new location if necessary
		if ((flags & PERSON_LOCATION)>0)
		{
			person.Location=va_arg(args, Room*);
			if (person.Location!=NULL)
				if (person.Location->GetCrew()->GetCrewTotalNumber()>=person.Location->GetMaxPersons())
					person.Location=NULL;			
			if (person.Location==NULL)
			{
				va_end(args);
				return -1;
			}			
		}

		//Prepare removal of crew member - heap variables to preserve strings
		char *name=new char[strlen(person.Name)+1];
		char *miscId=new char[strlen(person.MiscId)+1];
		strcpy(name, person.Name);
		strcpy(miscId, person.MiscId);
		person.Name=name;
		person.MiscId=miscId;

		//Remove crew member prior to adding new crew member with changes properties - important to
		//stay withing max. crew member amount
		crew->RemoveCrewMember(person.Name);

		//Set optionally changed crew and recreate add person if necessary
		if (crew!=person.Location->GetCrew())
		{
			if (crew==this->crew) crew->AddCrewMember("John Doe", 20, 60, 75, "Crew");
			crew=person.Location->GetCrew();
		}
		
		if ((flags & PERSON_NAME)>0) person.Name=va_arg(args, char*);
		if ((flags & PERSON_MISCID)>0) person.MiscId=va_arg(args, char*);
		if ((flags & PERSON_AGE)>0) person.Age=atoi(va_arg(args, char*));
		if ((flags & PERSON_PULS)>0) person.Puls=atoi(va_arg(args, char*));
		if ((flags & PERSON_WEIGHT)>0) person.Weight=atoi(va_arg(args, char*));
		va_end(args);

		crew->AddCrewMember(person.Name, person.Age, person.Puls, person.Weight, person.MiscId);
		
		//Get index of newly added person
		index=GetPersons();
		for(result=0;result<index;result++)
		{
			Person p=GetPerson(result);
			if (p.Location->GetCrew()!=crew) continue;
			if (strcmp(p.Name, person.Name)==0) break;
		}
		if (result==index) result=-2;
		
		delete [] name;
		delete [] miscId;
		break;
	}
	return result;
}

Hangar *AscensionUltra::GetNearestHangar(int type, VESSEL *vessel, double radius)
{
	//Check Orbiter extensions version
	if (orbiterExtensionsVersion<0.1) return NULL;

	//Check vessel landed
	if (!vessel->GroundContact()) return NULL;

	VECTOR3 global, local;
	vessel->GetGlobalPos(global);
	Global2Local(global, local);
	local-=OFFSET;
	
	if (local.x<-6000 || local.x>0 || local.z<0 || local.z>1300) return NULL; //Check base vincinity
	
	if (launchTunnel.CheckVincinity(&local)) return &launchTunnel;	
	if (airport.CheckVincinity(&local)) return &airport;
	for(int i=0;i<TURNAROUNDHANGARS;i++) if (turnAround[i].CheckVincinity(&local)) return &turnAround[i];
	for(int i=0;i<LIGHTSTORAGEHANGARS;i++) if (lightStorage[i].CheckVincinity(&local)) return &lightStorage[i];
	
	return NULL;
}

void AscensionUltra::DockVessel(Room *room, VESSEL *vessel)
{
	//Check Orbiter extensions version
	if (orbiterExtensionsVersion<0.1) return;

	VESSEL *oldVessel;
	Room *oldRoom;
	if (roomVessel.find(room)==roomVessel.end()) oldVessel=NULL;
	else oldVessel=roomVessel[room];
	if (vesselRoom.find(vessel)==vesselRoom.end()) oldRoom=NULL;
	else oldRoom=vesselRoom[vessel];

	if (room==NULL && oldRoom!=NULL)
	{
		//Undock vessel
		oldRoom->SetDock(NULL);
		roomVessel.erase(oldRoom);
		vesselRoom.erase(vessel);
		OrbiterExtensions::SetDockState(vessel, NULL);
	}

	if (vessel==NULL && oldVessel!=NULL)
	{
		//Undock room
		room->SetDock(NULL);
		roomVessel.erase(room);
		vesselRoom.erase(oldVessel);
		OrbiterExtensions::SetDockState(oldVessel, NULL);
	}

	if (vessel!=NULL && room!=NULL)
	{
		if (oldRoom!=NULL)
		{
			//Undock old room occupation
			oldRoom->SetDock(NULL);
			vesselRoom.erase(vessel);
			roomVessel.erase(oldRoom);			
		}
		if (oldVessel!=NULL)
		{
			//Undock old vessel occupation
			room->SetDock(NULL);
			roomVessel.erase(room);
			vesselRoom.erase(oldVessel);
			OrbiterExtensions::SetDockState(oldVessel, NULL);
		}
		//Dock vessel to room
		room->SetDock(vessel);
		roomVessel[room]=vessel;
		vesselRoom[vessel]=room;
		OrbiterExtensions::SetDockState(vessel, GetHandle());
	}
}

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