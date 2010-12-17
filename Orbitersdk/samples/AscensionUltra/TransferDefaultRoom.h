#pragma once
#include "Room.h"

class TransferDefaultRoom:public Room
{
public:
	virtual void PostPostStep();
};
