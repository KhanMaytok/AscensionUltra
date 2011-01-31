#pragma once
#include "orbitersdk.h"
#include "UMmuSDK.h"

//Struct holding transfer UMMU information
//  This is a workaround for UMMU lack of multi-crew support
class Room;
struct VesselRoomTransfer
{
	char *Name;
	char *MiscId;
	int Age;
	int Puls;
	int Weight;
	Room *Room;
};

class Hangar;
class Room
{
public:
	Room(void);
	~Room(void);
	void Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition=_V(0,0,0), int capacity=0);
	virtual char *GetName();
	virtual Hangar *GetHangar();
	UMMUCREWMANAGMENT *GetCrew();
	VECTOR3 GetCameraPosition();
	VECTOR3 GetViewDirection();
	int GetMaxPersons();
	void PostStep (double simt, double simdt, double mjd);
	virtual void PostPostStep();
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
	VesselRoomTransfer *GetTransfered();
	void ResetTransfered();
};
