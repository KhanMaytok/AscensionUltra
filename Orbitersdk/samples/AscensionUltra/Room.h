#pragma once
#include "orbitersdk.h"

class Hangar;
class Room
{
public:
	Room(void);
	~Room(void);
	void Init(Hangar *owner, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection);
	virtual char *GetName();
	virtual Hangar *GetHangar();
	VECTOR3 GetCameraPosition();
	VECTOR3 GetViewDirection();
private:
	Hangar *owner;
	char *name;
	VECTOR3 cameraPosition, viewDirection;
};
