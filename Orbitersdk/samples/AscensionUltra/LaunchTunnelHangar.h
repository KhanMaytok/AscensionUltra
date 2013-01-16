// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LaunchTunnel.h
// Class definition of launch tunnel building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Door.h"
#include "Room.h"
#include "BeaconArray.h"
#include <vector>
#include "Checklist.h"
#include "LFMCChecklistStates.h"

#define DOORS	4
#define ROOMS	2

namespace LaunchTunnel
{
	namespace PrepareChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event)
			{
				if (event!=Event::Abort) return false;
				if (state++>State::Ready) state=State::Ready;
				if (state==State::Ready) GetHangar()->GetChecklist(1)->SetSubject(subject);
				sprintf(oapiDebugString(), "Prepare %d", state);
				return true;
			}
		};
	}

	namespace LaunchChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event)
			{
				if (event!=Event::Abort) return false;
				if (state++>State::TakeOff)
				{
					state=State::Empty;
					GetHangar()->GetChecklist(1)->SetSubject(NULL);
				}
				if (state==State::OpenExit) GetHangar()->GetChecklist(0)->Init(owner, hangar, "SOMETHING", 1); //TODO: this is unsafe and just for tests
				sprintf(oapiDebugString(), "Launch %d", state);
				return true;
			}
		};
	}
}

class LaunchTunnelHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);	
	int GetRooms();
	Room *GetRoom(int index);
	bool CheckVincinity(VECTOR3 *pos);
	void SetPosition(VECTOR3 position);
	int GetChecklists();
	Checklist *GetChecklist(int index);
private:
	Door doors[DOORS];	
	Room rooms[ROOMS];
	LaunchTunnel::PrepareChecklist::List prepare;
	LaunchTunnel::LaunchChecklist::List launch;
};
