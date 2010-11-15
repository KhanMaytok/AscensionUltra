#pragma once
#include "orbitersdk.h"
#include "Room.h"

class Person
{
public:
	Person(Room *location, int slot);
	char *Name;
	char *MiscId;
	int Age;
	int Puls;
	int Weight;
	Room *Location;
};
