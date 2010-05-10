// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LightStorageHangar.cpp
// Class implementation of light storage hangar building.
// ==============================================================
#include "LightStorageHangar.h"

LightStorageHangar::LightStorageHangar(void)
{
	for(int i=0;i<4;i++) doors[i].SetSpeed(0.1);

	event_prefix=NULL;

	cur_door=-1;
}

LightStorageHangar::~LightStorageHangar(void)
{
	delete [] event_prefix;
}

void LightStorageHangar::DefineAnimations ()
{
	static UINT DoorGrp[4] = {0,1,2,3};
	char prefix[20]="";
	int i=0;

	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[0].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp, 1,	_V(0,0,0), _V(-1,0,0), (float)(45*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp+1, 1,	_V(0,0,0), _V(1,0,0), (float)(45*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[2].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+2, 1, _V(0,3.62,0)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,1.81,0)), prefix);
	for (i=0;i<4;i++) doors[i].DefineAnimations();
}

void LightStorageHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	for(int i=0;i<4;i++) doors[i].PostStep(simt, simdt, mjd);	
}

bool LightStorageHangar::clbkLoadStateEx (char *line)
{
    if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d", &cur_door);
	else if (cur_door>=0 && cur_door<4) return doors[cur_door].clbkLoadStateEx(line);
	else return false;
	return true;
}

void LightStorageHangar::clbkSaveState (FILEHANDLE scn)
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
}

void LightStorageHangar::clbkPostCreation ()
{	
	for(int i=0;i<4;i++) doors[i].clbkPostCreation();
}

void LightStorageHangar::Init(VESSEL* owner, UINT meshIndex, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	strcpy(this->event_prefix=new char[strlen(event_prefix)], event_prefix);
}

Door *LightStorageHangar::GetDoor(int index){return (index>=0 && index<4)?doors+index:NULL;}

bool LightStorageHangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "DOOR", 4))
	{
		//Door event
		int door=(int)(event_type+4)[0]-0x30;
		if (door>=0 && door<4) return doors[door].clbkPlaybackEvent(simt, event_t, event_type+5, event);
	}
	return false;
}