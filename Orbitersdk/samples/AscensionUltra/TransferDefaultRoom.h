#pragma once
#include "Room.h"

class TransferDefaultRoom:public Room
{
public:
	virtual void PostStep (double simt, double simdt, double mjd);
};
