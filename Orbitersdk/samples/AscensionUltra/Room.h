#pragma once
#include "orbitersdk.h"
#include "UMmuSDK.h"

class Hangar;
class Room
{
public:
	Room(void);
	~Room(void);
	void Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition);
	virtual char *GetName();
	virtual Hangar *GetHangar();
	VECTOR3 GetCameraPosition();
	VECTOR3 GetViewDirection();
	void PostStep (double simt, double simdt, double mjd);
private:
	Hangar *hangar;
	char *name;
	VECTOR3 cameraPosition, viewDirection;
	UMMUCREWMANAGMENT crew;
};
