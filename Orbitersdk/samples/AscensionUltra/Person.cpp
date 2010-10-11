// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Person.cpp
// Class implementation of person data class.
// ==============================================================

#include "Person.h"

Person::Person(Room *location, int slot)
{	
	UMMUCREWMANAGMENT *crew=location->GetCrew();
	if (crew!=NULL)
	{
		Name=crew->GetCrewNameBySlotNumber(slot);
		MiscId=crew->GetCrewMiscIdBySlotNumber(slot);
		Age=crew->GetCrewAgeBySlotNumber(slot);
		Puls=crew->GetCrewPulseBySlotNumber(slot);
		Weight=crew->GetCrewWeightBySlotNumber(slot);
		Location=location;
	}
	else
	{
		Name=MiscId="";
		Age=Puls=Weight=0;
		Location=NULL;
	}
}