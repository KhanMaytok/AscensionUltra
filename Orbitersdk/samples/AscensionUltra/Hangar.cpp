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
	name=NULL;

	cur_door=-1;
	cur_room=-1;

	position=_V(0,0,0);
}

Hangar::~Hangar(void)
{
	delete [] event_prefix;
	delete [] name;
}

int Hangar::GetType(){throw "GetType() not allowed on abstract hangar class!";}

void Hangar::DefineAnimations ()
{	
	int k=GetDoors();
	for (int i=0;i<k;i++) GetDoor(i)->DefineAnimations();
}

void Hangar::clbkPostStep (double simt, double simdt, double mjd)
{
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->PostStep(simt, simdt, mjd);	
	k=GetRooms();
	for(int i=0;i<k;i++) GetRoom(i)->PostStep(simt, simdt, mjd);	
}

bool Hangar::clbkLoadStateEx (char *line)
{
	int k=GetDoors();
	int l=GetRooms();
    if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d", &cur_door);
	else if (cur_door>=0 && cur_door<k) return GetDoor(cur_door)->clbkLoadStateEx(line);
	else if (!strnicmp (line, "ROOM", 4)) sscanf (line+4, "%d", &cur_room);
	else if (cur_room>=0 && cur_room<l) return GetRoom(cur_room)->GetCrew()->LoadAllMembersFromOrbiterScenario(line);	
	else return false;	
}

void Hangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int k=GetDoors();
	int l=GetRooms();
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
	for(i=0;i<l;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tROOM", cbuf);		
		GetRoom(i)->GetCrew()->SaveAllMembersInOrbiterScenarios(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tROOM", cbuf);
}

void Hangar::clbkPostCreation ()
{	
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->clbkPostCreation();
}

void Hangar::Init(VESSEL* owner, const char *name, UINT meshIndex, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	strcpy(this->event_prefix=new char[strlen(event_prefix)+1], event_prefix);
	strcpy(this->name=new char[strlen(name)+1], name);
}

int Hangar::GetDoors(){return 0;}
Door *Hangar::GetDoor(int index){throw "GetDoor(1) not allowed on abstract hangar class!";}

int Hangar::GetRooms(){return 0;}
Room *Hangar::GetRoom(int index){throw "GetRoom(1) not allowed on abstract hangar class!";}

VECTOR3 Hangar::GetPosition(){return position;}

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

char *Hangar::GetName(){return name;}

int Hangar::InitActionAreas(UMMUCREWMANAGMENT *crew, int index){return index;}
bool Hangar::ActionAreaActivated(int action){return false;}

bool Hangar::CheckVincinity(VECTOR3 *pos){return false;}
