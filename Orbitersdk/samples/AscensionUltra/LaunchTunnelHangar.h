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



#define LFHOLDRANGE		{ _V(   315, -10, -25 ) , _V( 325, 20, 25 ) }
#define PFHOLDRANGE		{ _V(   175, -10, -25 ) , _V( 185, 20, 25 ) }
#define PAXHOLDRANGE	{ _V(    70, -10, -25 ) , _V(  80, 20, 25 ) }
#define FUELHOLDRANGE	{ _V(   -40, -10, -25 ) , _V( -30, 20, 25 ) }
#define LAUNCHHOLDRANGE	{ _V(  -280, -10, -25 ) , _V(-270, 20, 25 ) }
#define EXHAUSTRANGE	{ _V(  -350, -10, -50 ) , _V(-270, 20, 50 ) }

#define PFAREARANGE		{ _V(   175, -10, -80 ) , _V( 325, 20, 80 ) }
#define PAXAREARANGE	{ _V(    70, -10, -80 ) , _V( 325, 20, 80 ) }
#define FUELAREARANGE	{ _V(   -30, -10, -80 ) , _V( 325, 20, 80 ) }
#define TAKEOFFRANGE	{ _V(-14000, -10, -200) , _V( 325, 2000, 200) }

#define VINCINITYPAXHOLD	0
#define VINCINITYPFHOLD		1
#define VINCINITYLAUNCHHOLD	2
#define VINCINITYEXHAUST	3
#define VINCINITYTAKEOFF	4
#define VINCINITYFUELHOLD	5
#define VINCINITYLFHOLD		6
#define VINCINITYPFAREA		7
#define VINCINITYPAXAREA	8
#define VINCINITYFUELAREA	9

namespace LaunchTunnel
{
	namespace RequestChecklist
	{
		class List:public Checklist
		{
			int GetType(void){return Request;}
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace PreflightChecklist
	{
		class List:public Checklist
		{
			int GetType(void){return Preflight;}
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace BoardingChecklist
	{
		class List:public Checklist
		{
			int GetType(void){return Boarding;}
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace FuelingChecklist
	{
		class List:public Checklist
		{
			int GetType(void){return Fueling;}
			bool SetEvent(int event);
			void PostStep (double simt, double simdt, double mjd);
		};
	}

	namespace LaunchChecklist
	{
		class List:public Checklist
		{
			int GetType(void){return Launch;}
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