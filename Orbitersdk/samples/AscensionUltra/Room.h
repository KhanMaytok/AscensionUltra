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
	virtual void PostStep (double simt, double simdt, double mjd);
	void SetDock(VESSEL *vessel); //Not virtual for a reason! Should not be used outside AU, because AU keeps central hook information
	virtual VESSEL *GetDock();
private:
	VESSEL *owner;
	Hangar *hangar;
	char *name;
	UMMUCREWMANAGMENT crew;
	VECTOR3 cameraPosition, viewDirection;	
	int capacity;
	VESSEL *docked;
protected:
	bool GetTransfered();
	void SetTransfered(bool transfer);
};
