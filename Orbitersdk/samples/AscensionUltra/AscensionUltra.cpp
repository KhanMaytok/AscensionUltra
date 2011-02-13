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
#define TA1OFFSET _V(-1710,0,580)
#define OFFSET _V(2700,0,-950)
#define _V_(x,y,z) _V(-x,y,z)+OFFSET
#define LL1OFFSET _V(-2330,0,670)
#define LL1MATRIXOFFSET _V(70,0,0)
#define LH1OFFSET _V(-2315,0,577.5)
#define LH1MATRIXOFFSET _V(160,0,0)
#define MAXSTATICRUNWAYLINES 14
#define STATICRUNWAYLINESSPLIT 7
#define RUNWAYENDPOINTS 15
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
	cur_Lease=-1;
	cur_Airport=-1;
	cur_LaunchTunnel=-1;

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
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		prefix[6]=0x30+i;
		name[1]=0x31+i;
		turnAround[i].Init(this, name, i+3, prefix);
	}
	strcpy(prefix, "LEASEx");
	strcpy(name, " x. Lease Hangar");
	for(i=0;i<LEASELIGHTHANGARS;i++)
	{
		prefix[10]=0x30+i;
		name[1]=0x30+((i+1) % 10);
		if (i>8) name[0]=0x31;
		leaseLight[i].Init(this, name, i+3+TURNAROUNDHANGARS, prefix);
	}	
	for(;i<LEASELIGHTHANGARS+LEASEHEAVYHANGARS;i++)
	{
		prefix[10]=0x30+i;
		name[1]=0x30+((i+1) % 10);
		if (i>8) name[0]=0x31;
		leaseHeavy[i-LEASELIGHTHANGARS].Init(this, name, i+3+TURNAROUNDHANGARS, prefix);
	}
	strcpy(prefix, "LAUNCHTUNNEL");
	strcpy(name, "Launch Facility");
	launchTunnel.Init(this, name, 20, prefix);

	strcpy(prefix, "AIRPORT");
	strcpy(name, "Airport");
	airport.Init(this, name, -1, prefix); //TODO: mesh index currently not set
	crew=airport.GetEntrance()->GetCrew();

	controlRoom=launchTunnel.GetRoom(1); //Tower

	//Initialize Orbiter extensions
	orbiterExtensionsResult=OrbiterExtensions::Init(this);

	if ((orbiterExtensionsVersion=OrbiterExtensions::GetVersion())<0) orbiterExtensionsVersion=0.0;

	//Generated subsection table by Excel
	taxiwaySubsection[0].Init(this, _V_(254,0,395), _V_(940,0,395), _V(0,1,0), 34);
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
	taxiwaySubsection[15].Init(this, _V_( 765,0,1015), _V_(765,0,925), _V(0,1,0), 5);
	taxiwaySubsection[16].Init(this, _V_( 765,0,880), _V_(765,0,790), _V(0,1,0), 5);
	taxiwaySubsection[17].Init(this, _V_(768,0,778), _V_( 778,0,768), _V(0,1,0), 2);
	taxiwaySubsection[18].Init(this, _V_( 788,0,765), _V_(940,0,765), _V(0,1,0), 8);
	taxiwaySubsection[19].Init(this, _V_(990,0,765), _V_(2430,0,765), _V(0,1,0), 73);
	taxiwaySubsection[20].Init(this, _V_(2480,0,765), _V_(4435,0,765), _V(0,1,0), 98);
	taxiwaySubsection[21].Init(this, _V_(4485,0,765), _V_(5295,0,765), _V(0,1,0), 41);
	taxiwaySubsection[22].Init(this, _V_(5345,0,765), _V_(5495,0,765), _V(0,1,0), 8);
	taxiwaySubsection[23].Init(this, _V_(5507,0,768), _V_(5517,0,778), _V(0,1,0), 2);
	taxiwaySubsection[24].Init(this, _V_(5520,0,790), _V_(5520,0,880), _V(0,1,0), 5);
	taxiwaySubsection[25].Init(this, _V_(5520,0,925), _V_(5520,0,1015), _V(0,1,0), 5);
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

	//Generated subsection table by Excel
	runwaySubsection[0].Init(this, _V_(1010,0,880), _V_(2410,0,880), _V(1,1,1), 70);
	runwaySubsection[1].Init(this, _V_(2500,0,880), _V_(4415,0,880), _V(1,1,1), 96);
	runwaySubsection[2].Init(this, _V_(4505,0,880), _V_(5275,0,880), _V(1,1,1), 40);
	runwaySubsection[3].Init(this, _V_(1010,0,925), _V_(2410,0,925), _V(1,1,1), 70);
	runwaySubsection[4].Init(this, _V_(2500,0,925), _V_(4415,0,925), _V(1,1,1), 96);
	runwaySubsection[5].Init(this, _V_(4505,0,925), _V_(5275,0,925), _V(1,1,1), 40);
	runwaySubsection[6].Init(this, _V_(1010,0,902.5), _V_(5275,0,902.5), _V(1,1,1), 300);
	runwaySubsection[7].Init(this, _V_(1010,0,880), _V_(1010,0,925), _V(0,0,1), 8);
	runwaySubsection[8].Init(this, _V_(5365,0,880), _V_(5365,0,925), _V(1,0,0), 8);
	runwaySubsection[9].Init(this, _V_(1010,0,1015), _V_(1010,0,1095), _V(0,0,1), 15);
	runwaySubsection[10].Init(this, _V_(5365,0,1015), _V_(5365,0,1095), _V(1,0,0), 15);
	runwaySubsection[11].Init(this, _V_(1010,0,1015), _V_(2410,0,1015), _V(1,1,1), 70);
	runwaySubsection[12].Init(this, _V_(2500,0,1015), _V_(4415,0,1015), _V(1,1,1), 96);
	runwaySubsection[13].Init(this, _V_(4505,0,1015), _V_(5275,0,1015), _V(1,1,1), 40);
	runwaySubsection[14].Init(this, _V_(1010,0,1095), _V_(2410,0,1095), _V(1,1,1), 70);
	runwaySubsection[15].Init(this, _V_(2500,0,1095), _V_(4415,0,1095), _V(1,1,1), 96);
	runwaySubsection[16].Init(this, _V_(4505,0,1095), _V_(5275,0,1095), _V(1,1,1), 40);
	runwaySubsection[17].Init(this, _V_(1010,0,1055), _V_(5275,0,1055), _V(1,1,1), 300);
	runwaySubsection[18].Init(this, _V_(5275,0,1015), _V_(5275,0,1095), _V(0,0,1), 15);
	runwaySubsection[19].Init(this, _V_(920,0,1015), _V_(920,0,1095), _V(1,0,0), 15);
	runwaySubsection[20].Init(this, _V_(5275,0,880), _V_(5275,0,925), _V(0,0,1), 8);
	runwaySubsection[21].Init(this, _V_(920,0,880), _V_(920,0,925), _V(1,0,0), 8);
	runwaySubsection[22].Init(this, _V_(5060,0,885), _V_(5365,0,885), _V(1,1,0), 16);
	runwaySubsection[23].Init(this, _V_(5060,0,920), _V_(5365,0,920), _V(1,1,0), 16);
	runwaySubsection[24].Init(this, _V_(5545,0,880), _V_(5545,0,925), _V(1,0,0), 13);
	runwaySubsection[25].Init(this, _V_(740,0,880), _V_(1010,0,880), _V(1,1,0), 18);
	runwaySubsection[26].Init(this, _V_(740,0,925), _V_(1010,0,925), _V(1,1,0), 18);
	runwaySubsection[27].Init(this, _V_(740,0,902.5), _V_(1010,0,902.5), _V(1,1,0), 18);
	runwaySubsection[28].Init(this, _V_(740,0,895), _V_(1010,0,895), _V(1,0,0), 18);
	runwaySubsection[29].Init(this, _V_(740,0,898), _V_(1010,0,898), _V(1,0,0), 18);
	runwaySubsection[30].Init(this, _V_(740,0,907), _V_(1010,0,907), _V(1,0,0), 18);
	runwaySubsection[31].Init(this, _V_(740,0,910), _V_(1010,0,910), _V(1,0,0), 18);
	runwaySubsection[32].Init(this, _V_(0,0,902.5), _V_(740,0,902.5), _V(1,1,1), 25);
	runwaySubsection[33].Init(this, _V_(4970,0,1030), _V_(5275,0,1030), _V(1,1,0), 16);
	runwaySubsection[34].Init(this, _V_(4970,0,1080), _V_(5275,0,1080), _V(1,1,0), 16);
	runwaySubsection[35].Init(this, _V_(5545,0,1015), _V_(5545,0,1095), _V(1,0,0), 25);
	runwaySubsection[36].Init(this, _V_(740,0,1015), _V_(1010,0,1015), _V(1,1,0), 18);
	runwaySubsection[37].Init(this, _V_(740,0,1095), _V_(1010,0,1095), _V(1,1,0), 18);
	runwaySubsection[38].Init(this, _V_(740,0,1055), _V_(1010,0,1055), _V(1,1,0), 18);
	runwaySubsection[39].Init(this, _V_(740,0,1035), _V_(1010,0,1035), _V(1,0,0), 18);
	runwaySubsection[40].Init(this, _V_(740,0,1040), _V_(1010,0,1040), _V(1,0,0), 18);
	runwaySubsection[41].Init(this, _V_(740,0,1070), _V_(1010,0,1070), _V(1,0,0), 18);
	runwaySubsection[42].Init(this, _V_(740,0,1075), _V_(1010,0,1075), _V(1,0,0), 18);
	runwaySubsection[43].Init(this, _V_(0,0,1055), _V_(740,0,1055), _V(1,1,1), 25);
	runwaySubsection[44].Init(this, _V_(920,0,1030), _V_(1225,0,1030), _V(1,1,0), 16);
	runwaySubsection[45].Init(this, _V_(920,0,1080), _V_(1225,0,1080), _V(1,1,0), 16);
	runwaySubsection[46].Init(this, _V_(740,0,1015), _V_(740,0,1095), _V(1,0,0), 25);
	runwaySubsection[47].Init(this, _V_(5275,0,1015), _V_(5545,0,1015), _V(1,1,0), 18);
	runwaySubsection[48].Init(this, _V_(5275,0,1095), _V_(5545,0,1095), _V(1,1,0), 18);
	runwaySubsection[49].Init(this, _V_(5275,0,1055), _V_(5545,0,1055), _V(1,1,0), 18);
	runwaySubsection[50].Init(this, _V_(5275,0,1035), _V_(5545,0,1035), _V(1,0,0), 18);
	runwaySubsection[51].Init(this, _V_(5275,0,1040), _V_(5545,0,1040), _V(1,0,0), 18);
	runwaySubsection[52].Init(this, _V_(5275,0,1070), _V_(5545,0,1070), _V(1,0,0), 18);
	runwaySubsection[53].Init(this, _V_(5275,0,1075), _V_(5545,0,1075), _V(1,0,0), 18);
	runwaySubsection[54].Init(this, _V_(5275,0,1055), _V_(6105,0,1055), _V(1,1,1), 25);
	runwaySubsection[55].Init(this, _V_(1010,0,890), _V_(1315,0,890), _V(1,1,0), 16);
	runwaySubsection[56].Init(this, _V_(1010,0,915), _V_(1315,0,915), _V(1,1,0), 16);
	runwaySubsection[57].Init(this, _V_(740,0,880), _V_(740,0,925), _V(1,0,0), 13);
	runwaySubsection[58].Init(this, _V_(5275,0,880), _V_(5545,0,880), _V(1,1,0), 18);
	runwaySubsection[59].Init(this, _V_(5275,0,925), _V_(5545,0,925), _V(1,1,0), 18);
	runwaySubsection[60].Init(this, _V_(5275,0,902.5), _V_(5545,0,902.5), _V(1,1,0), 18);
	runwaySubsection[61].Init(this, _V_(5275,0,895), _V_(5545,0,895), _V(1,0,0), 18);
	runwaySubsection[62].Init(this, _V_(5275,0,898), _V_(5545,0,898), _V(1,0,0), 18);
	runwaySubsection[63].Init(this, _V_(5275,0,907), _V_(5545,0,907), _V(1,0,0), 18);
	runwaySubsection[64].Init(this, _V_(5275,0,910), _V_(5545,0,910), _V(1,0,0), 18);
	runwaySubsection[65].Init(this, _V_(5545,0,902.5), _V_(6105,0,902.5), _V(1,1,1), 25);
	runwaySubsection[66].Init(this, _V_(1145,0,1235), _V_(5733,0,1235), _V(1,1,1), 230);
	runwaySubsection[67].Init(this, _V_(1145,0,1285), _V_(5733,0,1285), _V(1,1,1), 230);
	runwaySubsection[68].Init(this, _V_(1145,0,1260), _V_(1735,0,1260), _V(1,1,1), 30);
	runwaySubsection[69].Init(this, _V_(1145,0,1235), _V_(1145,0,1285), _V(1,0,0), 7);
	runwaySubsection[70].Init(this, _V_(1282,0,1235), _V_(1282,0,1285), _V(1,0,0), 7);
	runwaySubsection[71].Init(this, _V_(1389,0,1235), _V_(1389,0,1285), _V(1,0,0), 7);
	runwaySubsection[72].Init(this, _V_(1502,0,1235), _V_(1502,0,1285), _V(1,0,0), 7);
	runwaySubsection[73].Init(this, _V_(1738,0,1235), _V_(1738,0,1285), _V(1,0,0), 10);
	runwaySubsection[74].Init(this, _V_(5738,0,1235), _V_(5738,0,1285), _V(1,0,0), 7);
	runwaySubsection[75].Init(this, _V_(5744,0,1235), _V_(5744,0,1285), _V(1,0,0), 10);
	runwaySubsection[76].Init(this, _V_(5430,0,1245), _V_(5738,0,1245), _V(1,1,0), 16);
	runwaySubsection[77].Init(this, _V_(5430,0,1275), _V_(5738,0,1275), _V(1,1,0), 16);
	runwaySubsection[78].Init(this, _V_(1145,0,1260), _V_(1502,0,1260), _V(0,1,0), 18);
	runwaySubsection[79].Init(this, _V_(1502,0,1260), _V_(1738,0,1260), _V(0,1,0), 12);
	runwaySubsection[80].Init(this, _V_(1738,0,1235), _V_(1738,0,1285), _V(1,0,0), 20);
	runwaySubsection[81].Init(this, _V_(1738,0,1215), _V_(1738,30,1215), _V(1,0,0), 15);
	runwaySubsection[82].Init(this, _V_(1738,0,1305), _V_(1738,30,1305), _V(1,0,0), 15);
	runwaySubsection[83].Init(this, _V_(1738,0,1235), _V_(1738,0,1285), _V(1,0.549019607843137,0), 20);
	runwaySubsection[84].Init(this, _V_(1738,0,1215), _V_(1738,30,1215), _V(1,0.549019607843137,0), 15);
	runwaySubsection[85].Init(this, _V_(1738,0,1305), _V_(1738,30,1305), _V(1,0.549019607843137,0), 15);
	runwaySubsection[86].Init(this, _V_(1511,0,1235), _V_(1511,0,1285), _V(1,0.549019607843137,0), 10);
	runwaySubsection[87].Init(this, _V_(1738,0,1235), _V_(1738,0,1285), _V(0,1,0), 20);
	runwaySubsection[88].Init(this, _V_(1738,0,1215), _V_(1738,30,1215), _V(0,1,0), 15);
	runwaySubsection[89].Init(this, _V_(1738,0,1305), _V_(1738,30,1305), _V(0,1,0), 15);
	runwaySubsection[90].Init(this, _V_(1738,0,1260), _V_(5738,0,1260), _V(0,1,0), 200);
	runwaySubsection[91].Init(this, _V_(1189,0,1235), _V_(1189,0,1285), _V(1,0.549019607843137,0), 10);

	int offsets[RUNWAYPATHS]=
	{
		11,					//0		13L/31R Static
		22,					//1		13R/31L Static
		25,					//2		13L Take-Off
		33,					//3		13L Landing
		36,					//4		13R Take-Off
		44,					//5		13R Landing
		47,					//6		31L Take-Off
		55,					//7		31L Landing
		58,					//8		31R Take-Off
		66,					//9		31R Landing
		78,					//10	Launch Static
		79,					//11	Launch S2
		83,					//12	Launch S5
		87,					//13	Launch S3
		91,					//14	Launch S4
		RUNWAYSUBSECTIONS	//15	Launch S1
	};

	int j=0;
	for(int i=0;i<RUNWAYPATHS;i++)
	{
		runwayPath[i].Init(this, NULL, _V(0,0,0), NULL, 0);
		for(;j<offsets[i];j++) runwayPath[i].Add(&runwaySubsection[j]);
	}	

	static char *points[RUNWAYENDPOINTS]=
	{
		"Launch",			//0
		"Storage Hangars",	//1
		"TA Hangars",		//2
		"Aux Hangars",		//3
		"Runway 13L",		//4
		"Runway 13R",		//5
		"Runway 31L",		//6
		"Runway 31R",		//7
		"Airport",			//8
		"Runway 13L/31R",	//9
		"Runway 31L/13R",	//10
		"Static",			//11
		"Take-Off",			//12
		"Landing",			//13
		"Lease Hangars"		//14
	};

	taxiways.Init(1.4, 0.4, 2, 0.3, -0.2);
	//Generated subsection table by Excel
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
	taxiways.Add(&taxiwayPath[0], points[1], points[14], true);
	taxiways.Add(&taxiwayPath[3], points[14], points[1], true);
	taxiways.Add(&taxiwayPath[4], points[14], points[3], true);
	taxiways.Add(&taxiwayPath[5], points[14], points[0], true);
	taxiways.Add(&taxiwayPath[6], points[3], points[14], true);
	taxiways.Add(&taxiwayPath[9], points[9], points[14], true);
	taxiways.Add(&taxiwayPath[13], points[10], points[14], true);

	taxiways.Switch(true);

	runways.Init(1.4, 0.4, 2, 0.12, -0.07);
	runways.Add(&runwayPath[0], points[4], points[11], false);
	runways.Add(&runwayPath[2], points[4], points[12], false);
	runways.Add(&runwayPath[3], points[4], points[13], false);
	runways.Add(&runwayPath[1], points[5], points[11], false);
	runways.Add(&runwayPath[4], points[5], points[12], false);
	runways.Add(&runwayPath[5], points[5], points[13], false);
	runways.Add(&runwayPath[1], points[6], points[11], false);
	runways.Add(&runwayPath[6], points[6], points[12], false);
	runways.Add(&runwayPath[7], points[6], points[13], false);
	runways.Add(&runwayPath[0], points[7], points[11], false);
	runways.Add(&runwayPath[8], points[7], points[12], false);
	runways.Add(&runwayPath[9], points[7], points[13], false);
	runways.Add(&runwayPath[10], points[0], points[11], false);
	runways.Add(&runwayPath[11], points[0], points[2], false);
	runways.Add(&runwayPath[12], points[0], points[5], false);
	runways.Add(&runwayPath[13], points[0], points[3], false);
	runways.Add(&runwayPath[14], points[0], points[4], false);
	runways.Add(&runwayPath[15], points[0], points[1], false);
	runways.Switch(false);
	runways.Switch(points[4],points[11],true);
	runways.Switch(points[5],points[11],true);
	runways.Switch(points[6],points[11],true);
	runways.Switch(points[7],points[11],true);
	runways.Switch(points[0],points[11],true);

	double landingStrobes[7][2]={{32,-2},{43,-2},{54,2},{65,2},{78,2},{79,2},{90,2}};	
	for(int i=0;i<4;i++)
	{
		runwaySubsection[(int)landingStrobes[i][0]].SetPeriod(landingStrobes[i][1]);
		runwaySubsection[(int)landingStrobes[i][0]].SetDuration(0.12);
		runwaySubsection[(int)landingStrobes[i][0]].SetPropagate(-0.07);
	}

	//DEBUG
	disx=0.0;
	disy=0.0;
	disz=0.0;
	stp=10.0;
	mnr=0;
}
// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void AscensionUltra::DefineAnimations ()
{
	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].DefineAnimations();
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].DefineAnimations();
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].DefineAnimations();
	launchTunnel.DefineAnimations();
	airport.DefineAnimations();
}

void AscensionUltra::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
	// draw the default HUD
	VESSEL2::clbkDrawHUD (mode, hps, hDC);

	// TODO: draw vessel status	
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

void OnLeaseHeavyLoad(MESHHANDLE hMesh, bool firstload)
{
	if (firstload) RotateMesh(hMesh, PI, _V(0,1,0), _V(0,0,0));
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
	meshHangar = oapiLoadMeshGlobal ("AscensionUltra\\AU_TA-NW");
	meshWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_TA-WO");
	meshLeaseLight = oapiLoadMeshGlobal ("AscensionUltra\\AU_LH-NW");
	meshLeaseLightWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_LH-WO");
	meshLeaseHeavy = oapiLoadMeshGlobal ("AscensionUltra\\AU_HH-NW", OnLeaseHeavyLoad);
	meshLeaseHeavyWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_HH-WO", OnLeaseHeavyLoad);
	
	double curvoff[TURNAROUNDHANGARS]={-0.08,-0.12,-0.17};

	InitSubObjects();

	for(int i=0;i<TURNAROUNDHANGARS;i++)
	{
		VECTOR3 off=OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0);
		SetMeshVisibilityMode (AddMesh (meshHangar, &off), MESHVIS_ALWAYS);
		turnAround[i].SetPosition(off);
	}
	for(int i=0;i<LEASELIGHTHANGARS;i++)
	{
		VECTOR3 off=OFFSET+LL1OFFSET+LL1MATRIXOFFSET*i;
		SetMeshVisibilityMode (AddMesh (meshLeaseLight, &off), MESHVIS_ALWAYS);
		leaseLight[i].SetPosition(off);
	}
	for(int i=0;i<LEASEHEAVYHANGARS;i++)
	{
		VECTOR3 off=OFFSET+LH1OFFSET+LH1MATRIXOFFSET*i;
		SetMeshVisibilityMode (AddMesh (meshLeaseHeavy, &off), MESHVIS_ALWAYS);
		leaseHeavy[i].SetPosition(off);
	}
	SetMeshVisibilityMode (AddMesh (meshLaunch = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_NW"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);
	for(int i=0;i<TURNAROUNDHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshWindow, &(OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0))), MESHVIS_ALWAYS);
	for(int i=0;i<LEASELIGHTHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshLeaseLightWindow, &(OFFSET+LL1OFFSET+LL1MATRIXOFFSET*i)), MESHVIS_ALWAYS);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshLeaseHeavyWindow, &(OFFSET+LH1OFFSET+LH1MATRIXOFFSET*i)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshLaunchWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_WO"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);
	launchTunnel.SetPosition(OFFSET+LFMCOFFSET);

	DefineAnimations();

	int index=0;

	for(int i=0;i<TURNAROUNDHANGARS;i++) index=turnAround[i].InitActionAreas(crew, index);
	for(int i=0;i<LEASELIGHTHANGARS;i++) index=leaseLight[i].InitActionAreas(crew, index);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) index=leaseHeavy[i].InitActionAreas(crew, index);
}

// Read status from scenario file
void AscensionUltra::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {		
		if (!strnicmp (line, "HANGAR", 6)) {
			sscanf (line+6, "%d", &cur_TurnAround);
		} else if (cur_TurnAround>=0 && cur_TurnAround<TURNAROUNDHANGARS) {
			if (!turnAround[cur_TurnAround].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!strnicmp (line, "LEASE", 5)) {
			sscanf (line+5, "%X", &cur_Lease);
		} else if (cur_Lease>=0 && cur_Lease<LEASELIGHTHANGARS) {
			if (!leaseLight[cur_Lease].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (cur_Lease>=LEASELIGHTHANGARS && cur_Lease<LEASELIGHTHANGARS+LEASEHEAVYHANGARS) {
			if (!leaseHeavy[cur_Lease-LEASELIGHTHANGARS].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
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
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "HANGAR", cbuf);
		turnAround[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "HANGAR", cbuf);

	for(i=0;i<LEASEHEAVYHANGARS+LEASELIGHTHANGARS;i++)
	{
		sprintf (cbuf, "%X", i);
		oapiWriteScenario_string (scn, "LEASE", cbuf);
		if (i<LEASELIGHTHANGARS) leaseLight[i].clbkSaveState(scn);
		else leaseHeavy[i-LEASELIGHTHANGARS].clbkSaveState(scn);
	}	
	sprintf (cbuf, "%X", i);
	oapiWriteScenario_string (scn, "LEASE", cbuf);

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

	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].clbkPostCreation();
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].clbkPostCreation();
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].clbkPostCreation();
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
		if (hangar>=0 && hangar<TURNAROUNDHANGARS) return turnAround[hangar].clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	if (!strnicmp (event_type, "LEASE", 5))
	{
		//Hangar event
		int hangar=(int)(event_type+5)[0]-0x30;
		if (hangar>=0 && hangar<LEASELIGHTHANGARS) return leaseLight[hangar].clbkPlaybackEvent(simt, event_t, event_type+6, event);
		if (hangar>=LEASELIGHTHANGARS && hangar<LEASEHEAVYHANGARS) return leaseHeavy[hangar-LEASELIGHTHANGARS].clbkPlaybackEvent(simt, event_t, event_type+6, event);
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
	//Close tunnel door
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=3+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS;
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
	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].clbkPostStep(simt, simdt, mjd);
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
	if (action>-1) for(int i=0;i<TURNAROUNDHANGARS;i++) if (turnAround[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) for(int i=0;i<LEASELIGHTHANGARS;i++) if (leaseLight[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) for(int i=0;i<LEASEHEAVYHANGARS;i++) if (leaseHeavy[i].ActionAreaActivated(action)) break;

	//DEBUG relative position
	if (coords)
	{
		VECTOR3 global, local;
		oapiGetFocusInterface()->GetGlobalPos(global);
		Global2Local(global, local);
		local-=OFFSET+LH1OFFSET;
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

bool clbkBeaconSizeInput (void *id, char *str, void *usrdata)
{
	double value1, value2;
	sscanf(str, "%lf %lf", &value1, &value2);	
	if (value1<=0.0 || value2<0) return false;
	BeaconPath *bp=(BeaconPath *)usrdata;
	for(int i=0;i<TAXIWAYPATHS;i++)
	{
		bp[i].SetSize(value1);
		bp[i].SetFallOff(value2);	
	}
	return true;
}

bool clbkBeaconFallOffInput (void *id, char *str, void *usrdata)
{
	double value1, value2, value3, value4;
	sscanf(str, "%lf %lf %lf %lf", &value1, &value2, &value3, &value4);
	BeaconPath *bp=(BeaconPath *)usrdata;
	for(int i=0;i<RUNWAYPATHS;i++)
	{
		bp[i].SetPeriod(value1);
		bp[i].SetDuration(value2);
		bp[i].SetPropagate(value3);
		bp[i].SetOffset(value4);
	}
	return true;
}

// Process buffered key events
int AscensionUltra::clbkConsumeBufferedKey (DWORD key, bool down, char *kstate)
{
	char inputBoxBuffer[81], inputBoxTitle[81];
	
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (KEYMOD_SHIFT (kstate)) {
	} else if (KEYMOD_CONTROL (kstate)) {
		switch (key) {		
		}
	} else {
		switch (key) {
		//DEBUG		
		case OAPI_KEY_1:
			MoveGroup(mnr, _V(stp, 0, 0));
			disx+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_2:
			MoveGroup(mnr, _V(-stp, 0, 0));
			disx-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_3:
			MoveGroup(mnr, _V(0, stp, 0));
			disy+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_4:
			MoveGroup(mnr, _V(0, -stp, 0));
			disy-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_5:
			MoveGroup(mnr, _V(0, 0, stp));
			disz+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_6:
			MoveGroup(mnr, _V(0, 0, -stp));
			disz-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_7:
			stp/=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_8:
			stp*=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_9:
			if (mnr>0) mnr--;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_0:
			mnr++;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;	
		case OAPI_KEY_U:
			sprintf(inputBoxTitle, "Taxiway beacon (size,falloff):");
			sprintf(inputBoxBuffer, "%f  %f", taxiwayPath[0].GetSize(),taxiwayPath[0].GetFallOff());
			oapiOpenInputBox(inputBoxTitle, clbkBeaconSizeInput, inputBoxBuffer, 80, taxiwayPath);
			return 1;
		case OAPI_KEY_I:
			sprintf(inputBoxTitle, "Runway beacon (period, duration, propagate, offset):");
			sprintf(inputBoxBuffer, "%f  %f  %f  %f", runwayPath[9].GetPeriod(), runwayPath[9].GetDuration(), runwayPath[9].GetPropagate(), runwayPath[9].GetOffset());
			oapiOpenInputBox(inputBoxTitle, clbkBeaconFallOffInput, inputBoxBuffer, 80, runwayPath);
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

int AscensionUltra::GetHangars(int type)
{
	int count=0;
	if ((type & HANGARTYPETA)>0) count+=TURNAROUNDHANGARS;
	if ((type & HANGARTYPELL)>0) count+=LEASELIGHTHANGARS;	
	if ((type & HANGARTYPELH)>0) count+=LEASEHEAVYHANGARS;
	if ((type & HANGARTYPELFMC)>0) count+=1;
	if ((type & HANGARTYPEPORT)>0) count+=1;
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
	if ((type & HANGARTYPELL)>0)
	{
		if (index<LEASELIGHTHANGARS) return leaseLight+index;
		else index-=LEASELIGHTHANGARS;
	}	
	if ((type & HANGARTYPELH)>0)
	{
		if (index<LEASEHEAVYHANGARS) return leaseHeavy+index;
		else index-=LEASEHEAVYHANGARS;
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

	for(i=0;i<LEASELIGHTHANGARS;i++)
	{
		rooms=leaseLight[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=leaseLight[i].GetRoom(j)->GetCrew()->GetCrewTotalNumber();
	}

	for(i=0;i<LEASEHEAVYHANGARS;i++)
	{
		rooms=leaseHeavy[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=leaseHeavy[i].GetRoom(j)->GetCrew()->GetCrewTotalNumber();
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
// Returns values:
// -7 .. internal failure
// -3 .. no more slots available on location change (or ERROR_DOCKED_SHIP_IS_FULL on transfer)
// <0 .. negative results from UMmuSDK.h
//  0 .. success of EVA or remove
// >0 .. new index of modified person
int AscensionUltra::ChangePerson(int index, int flags, ...)
{	
	Person person=GetPerson(index);
	UMMUCREWMANAGMENT *crew=person.Location->GetCrew();
	int result=0;

	switch (flags)
	{
	case PERSON_EVA:
		{
			VESSEL *vessel=person.Location->GetDock();
			if (vessel!=NULL)
			{
				CreateDock(_V(0,0,0), _V(0,1,0), _V(0,1,0));
				OrbiterExtensions::SetDockState(this, vessel->GetHandle());
			}
			result=crew->EvaCrewMember(person.Name);
			if (result==ERROR_NO_ONE_ON_BOARD ||
				(result==EVA_OK && vessel!=NULL) ||
				(result==TRANSFER_TO_DOCKED_SHIP_OK && vessel==NULL)) result=ERROR_CHANGE_FAIL;
			if (result>EVA_OK) result=EVA_OK;
			if (vessel!=NULL)
			{
				OrbiterExtensions::SetDockState(this, NULL);
				ClearDockDefinitions();
			}
			break;
		}
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
				return ERROR_DOCKED_SHIP_IS_FULL;
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
		if (result==index) result=ERROR_CHANGE_FAIL;
		
		delete [] name;
		delete [] miscId;
		break;
	}
	return result;
}

Hangar *AscensionUltra::GetNearestHangar(int type, VESSEL *vessel)
{
	//Check Orbiter extensions version
	if (orbiterExtensionsVersion<0.1) return NULL;

	//Check vessel landed
	if (!vessel->GroundContact()) return NULL;

	VECTOR3 global, local;
	vessel->GetGlobalPos(global);
	Global2Local(global, local);
	global=local-OFFSET;
	
	if (global.x<-6000 || global.x>0 || global.z<0 || global.z>1300) return NULL; //Check base vincinity
	
	if (launchTunnel.CheckVincinity(&local)) return &launchTunnel;	
	if (airport.CheckVincinity(&local)) return &airport;
	for(int i=0;i<TURNAROUNDHANGARS;i++) if (turnAround[i].CheckVincinity(&local)) return &turnAround[i];
	for(int i=0;i<LEASELIGHTHANGARS;i++) if (leaseLight[i].CheckVincinity(&local)) return &leaseLight[i];
	for(int i=0;i<LEASEHEAVYHANGARS;i++) if (leaseHeavy[i].CheckVincinity(&local)) return &leaseHeavy[i];
	
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