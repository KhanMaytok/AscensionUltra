// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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

	hall.Init(owner, this, "Control Tower", _V(0,89,20), _V(0,0,1), _V(0,0,-122.5), 100, _V(0,PI,0));
	Hangar::DefineAnimations();
}

int AirportHangar::GetRooms(){return 1;} //Do not return entrance as a room

Room *AirportHangar::GetRoom(int index){return index==0?&hall:NULL;} //Skip entrance

Room *AirportHangar::GetEntrance(){return &entrance;}

void AirportHangar::SetPosition(VECTOR3 position){this->position=position;}