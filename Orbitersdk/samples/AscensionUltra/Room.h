// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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
	void Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition=_V(0,0,0), int capacity=0, VECTOR3 EVADirection=_V(DBL_MAX, DBL_MAX, DBL_MAX));
	virtual char *GetName();
	virtual Hangar *GetHangar();
	UMMUCREWMANAGMENT *GetCrew();
	VECTOR3 GetCameraPosition();
	VECTOR3 GetViewDirection();
	int GetMaxPersons();
	virtual int GetPersons();
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
