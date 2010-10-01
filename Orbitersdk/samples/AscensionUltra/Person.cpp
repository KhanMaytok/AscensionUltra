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

Person::Person(UMMUCREWMANAGMENT *crew, int slot)
{
	if (crew!=NULL)
	{
		Name=crew->GetCrewNameBySlotNumber(slot);
		MiscId=crew->GetCrewMiscIdBySlotNumber(slot);
	}
	else
	{
		Name=MiscId="";
	}
}