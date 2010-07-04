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

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))
#define TA1MATRIXOFFSET _V(266,0,0)
#define TA1OFFSET _V(-2242,0,580)
#define OFFSET _V(2700,0,-950)
#define CTRLROOM1 _V(-88,22,0)
#define CTRLROOM2 _V(-88,22,0)
#define _V_(x,y,z) _V(-x,y,z)+OFFSET
#define LS1OFFSET _V(-855,0,480)
#define LS1MATRIXOFFSET _V(70,0,0)
#define MAXSTATICRUNWAYLINES 14
#define RUNWAYENDPOINTS 9

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
	int i, j;

	modelidx = (fmodel ? 1 : 0);
	visual            = NULL;
	campos            = CAM_GENERIC;

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

	//Generated subsection table by Excel
	taxiwaySubsection[0].Init(this, _V_(110,0,395), _V_(940,0,395), _V(0,1,0), 12);
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
	taxiwaySubsection[29].Init(this, _V_( 965,0,1095), _V_(965,0,1220), _V(0,1,0), 7);
	taxiwaySubsection[30].Init(this, _V_(968,0,1232), _V_( 978,0,1242), _V(0,1,0), 2);
	taxiwaySubsection[31].Init(this, _V_( 990,0,1245), _V_(1240,0,1245), _V(0,1,0), 13);

	for(int i=0;i<TAXIWAYPATHS;i++) taxiwayPath[i].Init(this, NULL, _V(0,0,0), 0, 0);

	//Generated path table by Excel
	taxiwayPath[0].Add(&taxiwaySubsection[0]);	taxiwayPath[0].Add(&taxiwaySubsection[1]);							
	taxiwayPath[1].Add(&taxiwaySubsection[0]);	taxiwayPath[1].Add(&taxiwaySubsection[1]);	taxiwayPath[1].Add(&taxiwaySubsection[3]);	taxiwayPath[1].Add(&taxiwaySubsection[20]);	taxiwayPath[1].Add(&taxiwaySubsection[8]);	taxiwayPath[1].Add(&taxiwaySubsection[10]);			
	taxiwayPath[2].Add(&taxiwaySubsection[0]);	taxiwayPath[2].Add(&taxiwaySubsection[1]);	taxiwayPath[2].Add(&taxiwaySubsection[19]);	taxiwayPath[2].Add(&taxiwaySubsection[26]);					
	taxiwayPath[3].Add(&taxiwaySubsection[0]);	taxiwayPath[3].Add(&taxiwaySubsection[26]);	taxiwayPath[3].Add(&taxiwaySubsection[27]);	taxiwayPath[3].Add(&taxiwaySubsection[28]);	taxiwayPath[3].Add(&taxiwaySubsection[29]);	taxiwayPath[3].Add(&taxiwaySubsection[31]);			
	taxiwayPath[4].Add(&taxiwaySubsection[1]);	taxiwayPath[4].Add(&taxiwaySubsection[3]);	taxiwayPath[4].Add(&taxiwaySubsection[19]);	taxiwayPath[4].Add(&taxiwaySubsection[20]);	taxiwayPath[4].Add(&taxiwaySubsection[8]);	taxiwayPath[4].Add(&taxiwaySubsection[10]);			
	taxiwayPath[5].Add(&taxiwaySubsection[1]);	taxiwayPath[5].Add(&taxiwaySubsection[19]);							
	taxiwayPath[6].Add(&taxiwaySubsection[4]);	taxiwayPath[6].Add(&taxiwaySubsection[3]);	taxiwayPath[6].Add(&taxiwaySubsection[1]);						
	taxiwayPath[7].Add(&taxiwaySubsection[5]);	taxiwayPath[7].Add(&taxiwaySubsection[4]);	taxiwayPath[7].Add(&taxiwaySubsection[3]);	taxiwayPath[7].Add(&taxiwaySubsection[1]);					
	taxiwayPath[8].Add(&taxiwaySubsection[6]);	taxiwayPath[8].Add(&taxiwaySubsection[7]);							
	taxiwayPath[9].Add(&taxiwaySubsection[6]);	taxiwayPath[9].Add(&taxiwaySubsection[7]);	taxiwayPath[9].Add(&taxiwaySubsection[8]);						
	taxiwayPath[10].Add(&taxiwaySubsection[7]);								
	taxiwayPath[11].Add(&taxiwaySubsection[7]);	taxiwayPath[11].Add(&taxiwaySubsection[8]);							
	taxiwayPath[12].Add(&taxiwaySubsection[13]);	taxiwayPath[12].Add(&taxiwaySubsection[12]);							
	taxiwayPath[13].Add(&taxiwaySubsection[13]);	taxiwayPath[13].Add(&taxiwaySubsection[15]);	taxiwayPath[13].Add(&taxiwaySubsection[22]);	taxiwayPath[13].Add(&taxiwaySubsection[21]);					
	taxiwayPath[14].Add(&taxiwaySubsection[13]);	taxiwayPath[14].Add(&taxiwaySubsection[15]);	taxiwayPath[14].Add(&taxiwaySubsection[22]);	taxiwayPath[14].Add(&taxiwaySubsection[21]);	taxiwayPath[14].Add(&taxiwaySubsection[20]);	taxiwayPath[14].Add(&taxiwaySubsection[19]);			
	taxiwayPath[15].Add(&taxiwaySubsection[13]);	taxiwayPath[15].Add(&taxiwaySubsection[15]);	taxiwayPath[15].Add(&taxiwaySubsection[22]);	taxiwayPath[15].Add(&taxiwaySubsection[21]);	taxiwayPath[15].Add(&taxiwaySubsection[20]);	taxiwayPath[15].Add(&taxiwaySubsection[19]);	taxiwayPath[15].Add(&taxiwaySubsection[26]);	taxiwayPath[15].Add(&taxiwaySubsection[0]);	
	taxiwayPath[16].Add(&taxiwaySubsection[14]);	taxiwayPath[16].Add(&taxiwaySubsection[13]);	taxiwayPath[16].Add(&taxiwaySubsection[12]);						
	taxiwayPath[17].Add(&taxiwaySubsection[14]);	taxiwayPath[17].Add(&taxiwaySubsection[13]);	taxiwayPath[17].Add(&taxiwaySubsection[15]);	taxiwayPath[17].Add(&taxiwaySubsection[22]);	taxiwayPath[17].Add(&taxiwaySubsection[21]);				
	taxiwayPath[18].Add(&taxiwaySubsection[14]);	taxiwayPath[18].Add(&taxiwaySubsection[13]);	taxiwayPath[18].Add(&taxiwaySubsection[15]);	taxiwayPath[18].Add(&taxiwaySubsection[22]);	taxiwayPath[18].Add(&taxiwaySubsection[21]);	taxiwayPath[18].Add(&taxiwaySubsection[20]);	taxiwayPath[18].Add(&taxiwaySubsection[19]);		
	taxiwayPath[19].Add(&taxiwaySubsection[14]);	taxiwayPath[19].Add(&taxiwaySubsection[13]);	taxiwayPath[19].Add(&taxiwaySubsection[15]);	taxiwayPath[19].Add(&taxiwaySubsection[22]);	taxiwayPath[19].Add(&taxiwaySubsection[21]);	taxiwayPath[19].Add(&taxiwaySubsection[20]);	taxiwayPath[19].Add(&taxiwaySubsection[19]);	taxiwayPath[19].Add(&taxiwaySubsection[26]);	taxiwayPath[19].Add(&taxiwaySubsection[0]);
	taxiwayPath[20].Add(&taxiwaySubsection[15]);	taxiwayPath[20].Add(&taxiwaySubsection[16]);	taxiwayPath[20].Add(&taxiwaySubsection[18]);						
	taxiwayPath[21].Add(&taxiwaySubsection[15]);	taxiwayPath[21].Add(&taxiwaySubsection[16]);	taxiwayPath[21].Add(&taxiwaySubsection[18]);	taxiwayPath[21].Add(&taxiwaySubsection[19]);					
	taxiwayPath[22].Add(&taxiwaySubsection[15]);	taxiwayPath[22].Add(&taxiwaySubsection[16]);	taxiwayPath[22].Add(&taxiwaySubsection[18]);	taxiwayPath[22].Add(&taxiwaySubsection[19]);	taxiwayPath[22].Add(&taxiwaySubsection[20]);	taxiwayPath[22].Add(&taxiwaySubsection[8]);	taxiwayPath[22].Add(&taxiwaySubsection[10]);		
	taxiwayPath[23].Add(&taxiwaySubsection[15]);	taxiwayPath[23].Add(&taxiwaySubsection[16]);	taxiwayPath[23].Add(&taxiwaySubsection[18]);	taxiwayPath[23].Add(&taxiwaySubsection[19]);	taxiwayPath[23].Add(&taxiwaySubsection[20]);	taxiwayPath[23].Add(&taxiwaySubsection[21]);			
	taxiwayPath[24].Add(&taxiwaySubsection[16]);	taxiwayPath[24].Add(&taxiwaySubsection[18]);							
	taxiwayPath[25].Add(&taxiwaySubsection[16]);	taxiwayPath[25].Add(&taxiwaySubsection[18]);	taxiwayPath[25].Add(&taxiwaySubsection[19]);						
	taxiwayPath[26].Add(&taxiwaySubsection[16]);	taxiwayPath[26].Add(&taxiwaySubsection[18]);	taxiwayPath[26].Add(&taxiwaySubsection[19]);	taxiwayPath[26].Add(&taxiwaySubsection[20]);	taxiwayPath[26].Add(&taxiwaySubsection[8]);	taxiwayPath[26].Add(&taxiwaySubsection[10]);			
	taxiwayPath[27].Add(&taxiwaySubsection[16]);	taxiwayPath[27].Add(&taxiwaySubsection[18]);	taxiwayPath[27].Add(&taxiwaySubsection[19]);	taxiwayPath[27].Add(&taxiwaySubsection[20]);	taxiwayPath[27].Add(&taxiwaySubsection[21]);				
	taxiwayPath[28].Add(&taxiwaySubsection[19]);	taxiwayPath[28].Add(&taxiwaySubsection[20]);	taxiwayPath[28].Add(&taxiwaySubsection[8]);	taxiwayPath[28].Add(&taxiwaySubsection[10]);					
	taxiwayPath[29].Add(&taxiwaySubsection[19]);	taxiwayPath[29].Add(&taxiwaySubsection[20]);	taxiwayPath[29].Add(&taxiwaySubsection[8]);	taxiwayPath[29].Add(&taxiwaySubsection[10]);	taxiwayPath[29].Add(&taxiwaySubsection[26]);	taxiwayPath[29].Add(&taxiwaySubsection[0]);			
	taxiwayPath[30].Add(&taxiwaySubsection[26]);	taxiwayPath[30].Add(&taxiwaySubsection[27]);	taxiwayPath[30].Add(&taxiwaySubsection[28]);	taxiwayPath[30].Add(&taxiwaySubsection[29]);	taxiwayPath[30].Add(&taxiwaySubsection[31]);				
	taxiwayPath[31].Add(&taxiwaySubsection[27]);	taxiwayPath[31].Add(&taxiwaySubsection[4]);	taxiwayPath[31].Add(&taxiwaySubsection[3]);						
	taxiwayPath[32].Add(&taxiwaySubsection[27]);	taxiwayPath[32].Add(&taxiwaySubsection[26]);	taxiwayPath[32].Add(&taxiwaySubsection[19]);	taxiwayPath[32].Add(&taxiwaySubsection[4]);	taxiwayPath[32].Add(&taxiwaySubsection[3]);				
	taxiwayPath[33].Add(&taxiwaySubsection[27]);	taxiwayPath[33].Add(&taxiwaySubsection[28]);	taxiwayPath[33].Add(&taxiwaySubsection[29]);	taxiwayPath[33].Add(&taxiwaySubsection[31]);					
	taxiwayPath[34].Add(&taxiwaySubsection[28]);	taxiwayPath[34].Add(&taxiwaySubsection[27]);	taxiwayPath[34].Add(&taxiwaySubsection[4]);	taxiwayPath[34].Add(&taxiwaySubsection[3]);					
	taxiwayPath[35].Add(&taxiwaySubsection[28]);	taxiwayPath[35].Add(&taxiwaySubsection[27]);	taxiwayPath[35].Add(&taxiwaySubsection[26]);	taxiwayPath[35].Add(&taxiwaySubsection[19]);	taxiwayPath[35].Add(&taxiwaySubsection[4]);	taxiwayPath[35].Add(&taxiwaySubsection[3]);

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

	runwayPath[0].Init(this, (VECTOR3 *)runwayLines, _V(1,1,1), runwayBeacons, MAXSTATICRUNWAYLINES);
	runwayPath[1].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=0;i<14;i++) runwayPath[1].Add(&runwaySubsection[i]);
	runwayPath[2].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=14;i<28;i++) runwayPath[2].Add(&runwaySubsection[i]);
	runwayPath[3].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=28;i<42;i++) runwayPath[3].Add(&runwaySubsection[i]);
	runwayPath[4].Init(this, NULL, _V(0,0,0), NULL, 0);
	for(int i=42;i<56;i++) runwayPath[4].Add(&runwaySubsection[i]);

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
	};

	//Generated subsection table by Excel
	taxiways.Add(&taxiwayPath[0], points[1], points[2], false);
	taxiways.Add(&taxiwayPath[1], points[1], points[3], false);
	taxiways.Add(&taxiwayPath[3], points[1], points[0], false);
	taxiways.Add(&taxiwayPath[2], points[2], points[1], false);
	taxiways.Add(&taxiwayPath[4], points[2], points[3], false);
	taxiways.Add(&taxiwayPath[5], points[0], points[2], false);
	taxiways.Add(&taxiwayPath[30], points[2], points[0], false);
	taxiways.Add(&taxiwayPath[28], points[3], points[2], false);
	taxiways.Add(&taxiwayPath[29], points[3], points[1], false);
	taxiways.Add(&taxiwayPath[28], points[0], points[3], false);
	taxiways.Add(&taxiwayPath[33], points[3], points[0], false);
	taxiways.Add(&taxiwayPath[10], points[4], points[2], false);
	taxiways.Add(&taxiwayPath[14], points[2], points[4], false);
	taxiways.Add(&taxiwayPath[10], points[4], points[1], false);
	taxiways.Add(&taxiwayPath[15], points[1], points[4], false);
	taxiways.Add(&taxiwayPath[11], points[4], points[3], false);
	taxiways.Add(&taxiwayPath[12], points[3], points[4], false);
	taxiways.Add(&taxiwayPath[10], points[4], points[8], false);
	taxiways.Add(&taxiwayPath[13], points[8], points[4], false);
	taxiways.Add(&taxiwayPath[8], points[5], points[2], false);
	taxiways.Add(&taxiwayPath[18], points[2], points[5], false);
	taxiways.Add(&taxiwayPath[8], points[5], points[1], false);
	taxiways.Add(&taxiwayPath[19], points[1], points[5], false);
	taxiways.Add(&taxiwayPath[9], points[5], points[3], false);
	taxiways.Add(&taxiwayPath[16], points[3], points[5], false);
	taxiways.Add(&taxiwayPath[8], points[5], points[8], false);
	taxiways.Add(&taxiwayPath[17], points[8], points[5], false);
	taxiways.Add(&taxiwayPath[21], points[6], points[2], false);
	taxiways.Add(&taxiwayPath[7], points[2], points[6], false);
	taxiways.Add(&taxiwayPath[20], points[6], points[1], false);
	taxiways.Add(&taxiwayPath[35], points[1], points[6], false);
	taxiways.Add(&taxiwayPath[22], points[6], points[3], false);
	taxiways.Add(&taxiwayPath[34], points[3], points[6], false);
	taxiways.Add(&taxiwayPath[23], points[6], points[8], false);
	taxiways.Add(&taxiwayPath[34], points[8], points[6], false);
	taxiways.Add(&taxiwayPath[25], points[7], points[2], false);
	taxiways.Add(&taxiwayPath[6], points[2], points[7], false);
	taxiways.Add(&taxiwayPath[24], points[7], points[1], false);
	taxiways.Add(&taxiwayPath[32], points[1], points[7], false);
	taxiways.Add(&taxiwayPath[26], points[7], points[3], false);
	taxiways.Add(&taxiwayPath[31], points[3], points[7], false);
	taxiways.Add(&taxiwayPath[27], points[7], points[8], false);
	taxiways.Add(&taxiwayPath[31], points[8], points[7], false);

	for(int i=0;i<RUNWAYENDPOINTS;i++) taxiways.SwitchAll(points[i], false, true);
	for(int i=0;i<RUNWAYPATHS;i++) runwayPath[i].Switch(true);

	DefineAnimations();

	cur_TurnAround=-1;

	//DEBUG
	disx=0.0;
	disy=0.0;
	disz=0.0;
	stp=10.0;
	mnr=0;
	dnr=0;
}

// --------------------------------------------------------------
// Destructor
// --------------------------------------------------------------
AscensionUltra::~AscensionUltra ()
{
}

double AscensionUltra::GetVersion(){return 1.0;}

// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void AscensionUltra::DefineAnimations ()
{
	for(int i=0;i<5;i++) turnAround[i].DefineAnimations();
	for(int i=0;i<12;i++) lightStorage[i].DefineAnimations();
}

void AscensionUltra::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
	// draw the default HUD
	VESSEL2::clbkDrawHUD (mode, hps, hDC);

	// TODO: draw vessel status	
}

void AscensionUltra::SetNavlight (bool on)
{
	beacon[0].active = beacon[1].active = beacon[2].active = on;
	UpdateCtrlDialog (this);
}

void AscensionUltra::SetBeacon (bool on)
{
	beacon[3].active = beacon[4].active = on;
	UpdateCtrlDialog (this);
}

void AscensionUltra::SetStrobe (bool on)
{
	beacon[5].active = beacon[6].active = on;
	UpdateCtrlDialog (this);
}

static UINT HatchGrp[2] = {12,88};

static UINT AileronGrp[8] = {29,51,30,52,35,55,36,54};

static float tv0[8] = {0,0,0.0469f,0.0469f,0,0,0.0469f,0.0469f};

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

	// ********************* beacon lights **********************
	static VECTOR3 beaconpos[8] = {{-8.6,0,-3.3}, {8.6,0,-3.3}, {0,0.5,-7.5}, {0,2.2,2}, {0,-1.4,2}, {-8.9,2.5,-5.4}, {8.9,2.5,-5.4}, {0,-1.8,2}};
	static VECTOR3 beaconcol[7] = {{1.0,0.5,0.5}, {0.5,1.0,0.5}, {1,1,1}, {1,0.6,0.6}, {1,0.6,0.6}, {1,1,1}, {1,1,1}};
	for (int i = 0; i < 7; i++) {
		beacon[i].shape = (i < 3 ? BEACONSHAPE_DIFFUSE : BEACONSHAPE_STAR);
		beacon[i].pos = beaconpos+i;
		beacon[i].col = beaconcol+i;
		beacon[i].size = (i < 3 ? 0.3 : 0.55);
		beacon[i].falloff = (i < 3 ? 0.4 : 0.6);
		beacon[i].period = (i < 3 ? 0 : i < 5 ? 2 : 1.13);
		beacon[i].duration = (i < 5 ? 0.1 : 0.05);
		beacon[i].tofs = (6-i)*0.2;
		beacon[i].active = false;
		AddBeacon (beacon+i);
	}

	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Island1"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Base_Signs"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Place_Holders"), &OFFSET), MESHVIS_ALWAYS);
	meshHangar = oapiLoadMeshGlobal ("AscensionUltra\\TA1-NW");
	meshWindow = oapiLoadMeshGlobal ("AscensionUltra\\TA1-WO");
	meshLightStorage = oapiLoadMeshGlobal ("AscensionUltra\\LS1-1");
	
	double curvoff[5]={-0.02,-0.05,-0.08,-0.12,-0.17};

	for(int i=0;i<5;i++) SetMeshVisibilityMode (AddMesh (meshHangar, &(OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0))), MESHVIS_ALWAYS);
	for(int i=0;i<12;i++) SetMeshVisibilityMode (AddMesh (meshLightStorage, &(OFFSET+LS1OFFSET+LS1MATRIXOFFSET*i)), MESHVIS_ALWAYS);
	for(int i=0;i<5;i++) SetMeshVisibilityMode (AddMesh (meshWindow, &(OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoff[i],0))), MESHVIS_ALWAYS);

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
		} else if (!strnicmp (line, "LIGHTS", 6)) {
			int lgt[3];
			sscanf (line+6, "%d%d%d", lgt+0, lgt+1, lgt+2);
			SetNavlight (lgt[0] != 0);
			SetBeacon (lgt[1] != 0);
			SetStrobe (lgt[2] != 0);
        } else if (!strnicmp (line, "LIGHTS", 6)) {
			int lgt[3];
			sscanf (line+6, "%d%d%d", lgt+0, lgt+1, lgt+2);
			SetNavlight (lgt[0] != 0);
			SetBeacon (lgt[1] != 0);
			SetStrobe (lgt[2] != 0);
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
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "HANGAR", cbuf);
	
	for (i = 0; i < 7; i++)
		if (beacon[i].active) {
			sprintf (cbuf, "%d %d %d", beacon[0].active, beacon[3].active, beacon[5].active);
			oapiWriteScenario_string (scn, "LIGHTS", cbuf);
			break;
		}
}

// Finalise vessel creation
void AscensionUltra::clbkPostCreation ()
{
	SetEmptyMass (EMPTY_MASS);

	for(int i=0;i<5;i++) turnAround[i].clbkPostCreation();
	for(int i=0;i<12;i++) lightStorage[i].clbkPostCreation();
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
	return false;
}

// Create DG visual
void AscensionUltra::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	for(int i=0;i<12;i++) RotateGroup(i+5+3, PI, _V(0,1,0), _V(0,0,0));
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
	for(int i=0;i<5;i++) turnAround[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<12;i++) lightStorage[i].clbkPostStep(simt, simdt, mjd);
}

bool AscensionUltra::clbkLoadGenericCockpit ()
{
	SetCameraOffset (OFFSET+TA1OFFSET+CTRLROOM1);
	SetCameraDefaultDirection(_V(1,0,0));
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
	bp->SetSize(value1);
	bp->SetFallOff(value2);	
	return true;
}

bool clbkBeaconFallOffInput (void *id, char *str, void *usrdata)
{
	double value1, value2, value3, value4;
	sscanf(str, "%lf %lf %lf %lf", &value1, &value2, &value3, &value4);
	BeaconPath *bp=(BeaconPath *)usrdata;
	bp->SetPeriod(value1);
	bp->SetDuration(value2);
	bp->SetPropagate(value3);
	bp->SetOffset(value4);
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
		case OAPI_KEY_SPACE: // open control dialog
			oapiOpenDialogEx (g_Param.hDLL, IDD_CTRL, Ctrl_DlgProc, DLG_CAPTIONCLOSE, this);
			return 1;
		}
	} else {
		int res;
		//switch (key) {}
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

int AscensionUltra::GetHangars(HangarType type)
{
	switch(type)
	{
	case HangarType::TurnAround: return 5;
	case HangarType::LightStorage: return 12;	
	}
	return 0;
}

Hangar *AscensionUltra::GetHangar(HangarType type, int index)
{
	if (index<0) return NULL;
	switch(type)
	{
	case HangarType::TurnAround: return index<5?turnAround+index:NULL;
	case HangarType::LightStorage: return index<12?lightStorage+index:NULL;	
	}
	return NULL;
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
			GetDG(hTab)->GetHangar(HangarType::TurnAround, 0)->GetDoor(0)->Close();
			return TRUE;
		case IDC_OLOCK_OPEN:
			GetDG(hTab)->GetHangar(HangarType::TurnAround, 0)->GetDoor(0)->Open();
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
			dg->GetHangar(HangarType::TurnAround, 0)->GetDoor(0)->Close();
			return 0;
		case IDC_OLOCK_OPEN:
			dg->GetHangar(HangarType::TurnAround, 0)->GetDoor(0)->Open();
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

	op = dg->GetHangar(HangarType::TurnAround, 0)->GetDoor(0)->GetPosition()==0.0?0:1;
	SendDlgItemMessage (hWnd, IDC_OLOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_OLOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->beacon[0].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_NAVLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[3].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_BEACONLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[5].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_STROBELIGHT, BM_SETCHECK, bstatus[op], 0);
}