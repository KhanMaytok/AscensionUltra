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

TurnAroundHangar::TurnAroundHangar(void)
{
	crane1.SetSpeed(_V(10,10,10));
	crane1.SetCrawl(_V(1,1,1));
	for(int i=0;i<4;i++) doors[i].SetSpeed(0.1);

	event_prefix=NULL;

	cur_crane=cur_door=-1;
}

TurnAroundHangar::~TurnAroundHangar(void)
{
	delete [] event_prefix;
}

void TurnAroundHangar::DefineAnimations ()
{
	static UINT DoorGrp[8] = {0,1,2,3,4,5,6,7};
	char prefix[20]="";
	int i=0;

	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[0].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp+4, 1,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp+5, 1,	_V(0,0,0), _V(1,0,0), (float)(30*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[2].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+6, 1, _V(0,6,0)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+7, 1, _V(0,6,0)), prefix);
	for (i=0;i<4;i++) doors[i].DefineAnimations();

	sprintf(prefix, "%sCRANE%d", event_prefix, 0);
	crane1.Init(owner,
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+0, 1, _V(CRANEXOFFSET,0,0)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+1, 1, _V(0,0,CRANEYOFFSET)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,-CRANEREELLOWERPOINT,0)),
		new MGROUP_SCALE(meshIndex, DoorGrp+2, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1)),
		prefix);
	crane1.DefineAnimations();

}

void TurnAroundHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	for(int i=0;i<4;i++) doors[i].PostStep(simt, simdt, mjd);
	crane1.PostStep(simt, simdt, mjd);
}

bool TurnAroundHangar::clbkLoadStateEx (char *line)
{
    if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d", &cur_door);
	else if (cur_door>=0 && cur_door<4) return doors[cur_door].clbkLoadStateEx(line);
	else if (!strnicmp (line, "CRANE", 5)) sscanf (line+5, "%d", &cur_crane);
	else if (cur_crane>=0 && cur_crane<1) return crane1.clbkLoadStateEx(line);	
	else return false;
	return true;
}

void TurnAroundHangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;
	// Write custom parameters
	for(i=0;i<4;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tDOOR", cbuf);
		doors[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tDOOR", cbuf);
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
	for(int i=0;i<4;i++) doors[i].clbkPostCreation();
	crane1.clbkPostCreation();
}

void TurnAroundHangar::Init(VESSEL* owner, UINT meshIndex, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	strcpy(this->event_prefix=new char[strlen(event_prefix)], event_prefix);
}

Crane *TurnAroundHangar::GetCrane(){return &crane1;}

Door *TurnAroundHangar::GetDoor(int index){return (index>=0 && index<4)?doors+index:NULL;}

bool TurnAroundHangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "CRANE", 5))
	{
		//Crane event
		int crane=(int)(event_type+5)[0]-0x30;
		if (crane>=0 && crane<1) return crane1.clbkPlaybackEvent(simt, event_t, event_type+6, event);
	}
	if (!strnicmp (event_type, "DOOR", 4))
	{
		//Door event
		int door=(int)(event_type+4)[0]-0x30;
		if (door>=0 && door<4) return doors[door].clbkPlaybackEvent(simt, event_t, event_type+5, event);
	}
	return false;
}