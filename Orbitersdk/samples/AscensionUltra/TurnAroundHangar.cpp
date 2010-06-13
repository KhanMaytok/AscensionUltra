// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// TurnAroundHangar.cpp
// Class implementation of turn-around hangar building.
// ==============================================================
#include "TurnAroundHangar.h"

TurnAroundHangar::TurnAroundHangar(void):Hangar()
{
	crane1.SetSpeed(_V(10,10,10));
	crane1.SetCrawl(_V(1,1,1));	
}

HangarType TurnAroundHangar::GetType(){return HangarType::TurnAround;}

void TurnAroundHangar::DefineAnimations ()
{
	static UINT DoorGrp[8] = {0,1,2,3,4,5,6,7};
	char prefix[20]="";
	int i=0;

	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[0].Init(owner, "South Main Door", prefix, 1, new MGROUP_ROTATE(meshIndex, DoorGrp+4, 1,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, "North Main Door", prefix, 1, new MGROUP_ROTATE(meshIndex, DoorGrp+5, 1,	_V(0,0,0), _V(1,0,0), (float)(30*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[2].Init(owner, "East Maintenance Door", prefix, 1, new MGROUP_TRANSLATE(meshIndex, DoorGrp+6, 1, _V(0,6,0)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, "West Maintenance Door", prefix, 1, new MGROUP_TRANSLATE(meshIndex, DoorGrp+7, 1, _V(0,6,0)));
	Hangar::DefineAnimations();
	
	sprintf(prefix, "%sCRANE%d", event_prefix, 0);
	crane1.Init(owner, "Cargo Crane",
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+0, 1, _V(CRANEXOFFSET,0,0)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+1, 1, _V(0,0,CRANEYOFFSET)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,-CRANEREELLOWERPOINT,0)),
		new MGROUP_SCALE(meshIndex, DoorGrp+2, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1)),
		prefix);
	crane1.DefineAnimations();
}

void TurnAroundHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	Hangar::clbkPostStep(simt, simdt, mjd);
	crane1.PostStep(simt, simdt, mjd);
}

bool TurnAroundHangar::clbkLoadStateEx (char *line)
{
	if (Hangar::clbkLoadStateEx(line)) return true;
	else if (!strnicmp (line, "CRANE", 5)) sscanf (line+5, "%d", &cur_crane);
	else if (cur_crane>=0 && cur_crane<1) return crane1.clbkLoadStateEx(line);	
	else return false;	
}

void TurnAroundHangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;
	// Write custom parameters
	Hangar::clbkSaveState(scn);
	for(i=0;i<1;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tCRANE", cbuf);
		crane1.clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tCRANE", cbuf);
}

void TurnAroundHangar::clbkPostCreation ()
{	
	Hangar::clbkPostCreation();
	crane1.clbkPostCreation();
}

Crane *TurnAroundHangar::GetCrane(){return &crane1;}

int TurnAroundHangar::GetDoors(){return DOORS;}

Door *TurnAroundHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

bool TurnAroundHangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "CRANE", 5))
	{
		//Crane event
		int crane=(int)(event_type+5)[0]-0x30;
		if (crane>=0 && crane<1) return crane1.clbkPlaybackEvent(simt, event_t, event_type+6, event);
	}
	return Hangar::clbkPlaybackEvent(simt, event_t, event_type, event);
}