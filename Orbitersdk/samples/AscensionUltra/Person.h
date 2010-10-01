#pragma once
#include "orbitersdk.h"
#include "UMmuSDK.h"

class Person
{
public:
	Person(UMMUCREWMANAGMENT *crew, int slot);
	char *Name;
	char *MiscId;
};
