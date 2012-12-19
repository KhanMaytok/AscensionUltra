// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AirportHangar.cpp
// Class implementation of airport building.
// ==============================================================
#include "AirportHangar.h"

int AirportHangar::GetType(){return HANGARTYPEPORT;}

void AirportHangar::DefineAnimations ()
{
	//Setup entrance room - this is the dummy place for putting new persons to
	entrance.Init(owner, this, "Entrance", _V(0,0,0), _V(0,0,1), _V(0,0,0), 1);
	UMMUCREWMANAGMENT *crew=entrance.GetCrew();
	crew->SetAirlockDoorState(FALSE);
	crew->SetMaxSeatAvailableInShip(1);
	crew->AddCrewMember("John Doe", 20, 60, 75, "Crew");

	hall.Init(owner, this, "Terminal", _V(0,89,20), _V(0,-0.090444119,0.995901532), _V(0,0,20), 100);
	Hangar::DefineAnimations();
}

int AirportHangar::GetRooms(){return 1;} //Do not return entrance as a room

Room *AirportHangar::GetRoom(int index){return index==0?&hall:NULL;} //Skip entrance

Room *AirportHangar::GetEntrance(){return &entrance;}

void AirportHangar::SetPosition(VECTOR3 position){this->position=position;}