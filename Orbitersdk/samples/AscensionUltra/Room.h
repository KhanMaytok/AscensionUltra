#pragma once
#include "orbitersdk.h"
#include "UMmuSDK.h"

class Hangar;
class Room
{
public:
	Room(void);
	~Room(void);
	void Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition, int capacity);
	virtual char *GetName();
	virtual Hangar *GetHangar();
	UMMUCREWMANAGMENT *GetCrew();
	VECTOR3 GetCameraPosition();
	VECTOR3 GetViewDirection();
	int GetMaxPersons();
	void PostStep (double simt, double simdt, double mjd);
private:
	Hangar *hangar;
	char *name;
	VECTOR3 cameraPosition, viewDirection;
	UMMUCREWMANAGMENT crew;
	int capacity;
};
