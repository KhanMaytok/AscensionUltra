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
#define LISTS	5

#define PAXHOLDRANGE	{ _V(     0, -1000, -25 ) , _V( 180, 1000, 25 ) }
#define PFHOLDRANGE		{ _V(   180, -1000, -78 ) , _V( 300, 1000, 78 ) }
#define LAUNCHHOLDRANGE	{ _V(  -275, -1000, -25 ) , _V(-200, 1000, 25 ) }
#define EXHAUSTRANGE	{ _V(  -350, -1000, -50 ) , _V(-200, 1000, 50 ) }
#define TAKEOFFRANGE	{ _V(-10000, -1000, -200) , _V(-200, 2000, 200) }
#define FUELHOLDRANGE	{ _V(  -100, -1000, -70 ) , _V(   0, 1000, 70 ) }
#define LFHOLDRANGE		{ _V(   300, -1000, -78 ) , _V( 330, 1000, 78 ) }

#define VINCINITYPAXHOLD	0
#define VINCINITYPFHOLD		1
#define VINCINITYLAUNCHHOLD	2
#define VINCINITYEXHAUST	3
#define VINCINITYTAKEOFF	4
#define VINCINITYFUELHOLD	5
#define VINCINITYLFHOLD		6

namespace LaunchTunnel
{
	namespace RequestChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace PreflightChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace BoardingChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace FuelingChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace LaunchChecklist
	{
		class List:public Checklist
		{
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
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
	bool CheckVincinity(VECTOR3 *pos, int index=0);
	void SetPosition(VECTOR3 position);
	int GetChecklists();
	Checklist *GetChecklist(int index);
private:
	Door doors[DOORS];	
	Room rooms[ROOMS];
	LaunchTunnel::RequestChecklist::List request;
	LaunchTunnel::PreflightChecklist::List preflight;
	LaunchTunnel::BoardingChecklist::List boarding;
	LaunchTunnel::FuelingChecklist::List fueling;
	LaunchTunnel::LaunchChecklist::List launch;
	Checklist *lists[LISTS];
};