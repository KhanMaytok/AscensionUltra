// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Hangar.cpp
// Class implementation of abstract hangar building.
// ==============================================================
#include "Hangar.h"

Hangar::Hangar(void)
{
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->SetSpeed(0.1);

	event_prefix=NULL;

	cur_door=-1;
}

Hangar::~Hangar(void)
{
	delete [] event_prefix;
}

HangarType Hangar::GetType(){throw "GetType() not allowed on abstract hangar class!";}

void Hangar::DefineAnimations ()
{	
	int k=GetDoors();
	for (int i=0;i<k;i++) GetDoor(i)->DefineAnimations();
}

void Hangar::clbkPostStep (double simt, double simdt, double mjd)
{
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->PostStep(simt, simdt, mjd);	
}

bool Hangar::clbkLoadStateEx (char *line)
{
	int k=GetDoors();
    if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d", &cur_door);
	else if (cur_door>=0 && cur_door<k) return GetDoor(cur_door)->clbkLoadStateEx(line);
	else return false;	
}

void Hangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int k=GetDoors();
	int i;
	// Write custom parameters
	for(i=0;i<k;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tDOOR", cbuf);
		GetDoor(i)->clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tDOOR", cbuf);	
}

void Hangar::clbkPostCreation ()
{	
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->clbkPostCreation();
}

void Hangar::Init(VESSEL* owner, UINT meshIndex, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	strcpy(this->event_prefix=new char[strlen(event_prefix)+1], event_prefix);
}

int Hangar::GetDoors(){return 0;}
Door *Hangar::GetDoor(int index){throw "GetDoor(1) not allowed on abstract hangar class!";}

bool Hangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	int k=GetDoors();
	if (!strnicmp (event_type, "DOOR", 4))
	{
		//Door event
		int door=(int)(event_type+4)[0]-0x30;
		if (door>=0 && door<k) return GetDoor(door)->clbkPlaybackEvent(simt, event_t, event_type+5, event);
	}
	return false;
}