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
#include "Module.h"

Hangar::Hangar(void)
{
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->SetSpeed(0.1);

	classname=NULL;
	event_prefix=NULL;
	name=NULL;

	cur_door=-1;
	cur_room=-1;
	cur_checklist=-1;

	position=_V(0,0,0);
}

Hangar::~Hangar(void)
{
	for(std::vector<BeaconArray *>::iterator i=beacons.begin();i!=beacons.end();i++) delete *i;
	delete [] classname;
	delete [] event_prefix;
	delete [] name;
}

int Hangar::GetType(){throw "GetType() not allowed on abstract hangar class!";}

void Hangar::DefineAnimations ()
{	
	int k=GetDoors();
	for (int i=0;i<k;i++) GetDoor(i)->DefineAnimations();
	ReadBeaconDefinition(beacons, ini, classname, position, owner);
	ReadBeaconGroups(beaconGroup, beacons, ini, classname);
	if (instance>=0)
	{
		char *line=new char[strlen(classname)+40];
		sprintf(line, "%s%d", classname, instance);
		ReadBeaconDefinition(beacons, ini, line, position, owner);
		ReadBeaconGroups(beaconGroup, beacons, ini, line);
		delete [] line;
	}
}

void Hangar::clbkPostStep (double simt, double simdt, double mjd)
{
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->PostStep(simt, simdt, mjd);	
	k=GetRooms();
	for(int i=0;i<k;i++) GetRoom(i)->PostStep(simt, simdt, mjd);	
	k=GetChecklists();
	for(int i=0;i<k;i++) GetChecklist(i)->PostStep(simt, simdt, mjd);	
}

bool Hangar::clbkLoadStateEx (char *line)
{
	int k=GetDoors();
	int l=GetRooms();
	int m=GetChecklists();
    if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d", &cur_door);
	else if (cur_door>=0 && cur_door<k) return GetDoor(cur_door)->clbkLoadStateEx(line);
	else if (!strnicmp (line, "ROOM", 4)) sscanf (line+4, "%d", &cur_room);
	else if (cur_room>=0 && cur_room<l) return GetRoom(cur_room)->GetCrew()->LoadAllMembersFromOrbiterScenario(line);	
	else if (!strnicmp (line, "CHECKLIST", 9)) sscanf (line+9, "%d", &cur_checklist);
	else if (cur_checklist>=0 && cur_checklist<m) return GetChecklist(cur_checklist)->clbkLoadStateEx(line);
	else return false;
}

void Hangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int k=GetDoors();
	int l=GetRooms();
	int m=GetChecklists();
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
	for(i=0;i<m;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tCHECKLIST", cbuf);		
		GetChecklist(i)->clbkSaveState(scn);;
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tCHECKLIST", cbuf);
}

void Hangar::clbkPostCreation ()
{	
	int k=GetDoors();
	for(int i=0;i<k;i++) GetDoor(i)->clbkPostCreation();
}

void Hangar::clbkVisualCreated (VISHANDLE vis, int refcount){}

void Hangar::Init(VESSEL* owner, const char *ini, const char *name, UINT meshIndex, const char *classname, int instance, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	this->instance=instance;
	this->ini=ini;
	int i=strlen(classname);
	strcpy(this->classname=new char[i+1], classname);
	if (event_prefix==NULL) event_prefix=classname;
	this->event_prefix=new char[strlen(event_prefix)+40]; // make some room for digits of instance
	if (instance<0) strcpy(this->event_prefix, event_prefix);
	else sprintf(this->event_prefix, "%s%d", event_prefix, instance);
	strcpy(this->name=new char[strlen(name)+1], name);
}

int Hangar::GetDoors(){return 0;}
Door *Hangar::GetDoor(int index){throw "GetDoor(1) not allowed on abstract hangar class!";}

int Hangar::GetRooms(){return 0;}
Room *Hangar::GetRoom(int index){throw "GetRoom(1) not allowed on abstract hangar class!";}

int Hangar::GetChecklists(){return 0;}
Checklist *Hangar::GetChecklist(int index){throw "GetChecklist(1) not allowed on abstract hangar class!";}

VECTOR3 Hangar::GetPosition(){return position;}

bool Hangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "DOOR", 4))
	{
		//Door event
		int k=GetDoors();
		int door=(int)(event_type+4)[0]-0x30;
		if (door>=0 && door<k) return GetDoor(door)->clbkPlaybackEvent(simt, event_t, event_type+5, event);
	}
	if (!strnicmp (event_type, "CHECKLIST", 9))
	{
		//Checklist event
		int k=GetChecklists();
		int checklist=(int)(event_type+9)[0]-0x30;
		if (checklist>=0 && checklist<k) return GetChecklist(checklist)->clbkPlaybackEvent(simt, event_t, event_type+10, event);
	}
	return false;
}

char *Hangar::GetName(){return name;}

int Hangar::InitActionAreas(UMMUCREWMANAGMENT *crew, int index){return index;}
bool Hangar::ActionAreaActivated(int action){return false;}

bool Hangar::CheckVincinity(VECTOR3 *pos, int index){return false;}
