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
#include "Hangar.h"
#include "Door.h"
#include "FoldingDoor.h"
#include "Room.h"
#include "BeaconArray.h"
#include <vector>
#include <map>
#include "Checklist.h"
#include "LFMCChecklistStates.h"

#define LFMCDOORS	4
#define LFMCFDOORS	1
#define LFMCROOMS	2
#define LFMCLISTS	5



#define LFHOLDRANGE		{ _V(   315, -10, -25 ) , _V( 325, 20, 25 ) }
#define PFHOLDRANGE		{ _V(   175, -10, -25 ) , _V( 185, 20, 25 ) }
#define PAXHOLDRANGE	{ _V(    70, -10, -25 ) , _V(  80, 20, 25 ) }
#define FUELHOLDRANGE	{ _V(   -40, -10, -25 ) , _V( -30, 20, 25 ) }
#define LAUNCHHOLDRANGE	{ _V(  -280, -10, -25 ) , _V(-270, 20, 25 ) }
#define EXHAUSTRANGE	{ _V(  -350, -10, -50 ) , _V(-270, 20, 50 ) }
#define ABORTRANGE      { _V( -1015, -10, -148) , _V(-965, 20,-138) }

#define PFAREARANGE		{ _V(   175, -10, -80 ) , _V( 325, 20, 80 ) }
#define PAXAREARANGE	{ _V(    70, -10, -80 ) , _V( 325, 20, 80 ) }
#define FUELAREARANGE	{ _V(   -30, -10, -80 ) , _V( 325, 20, 80 ) }
#define TAKEOFFRANGE	{ _V(-14000, -10, -200) , _V( 325,2000,200) }

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
#define VINCINITYABORT		10

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
		private:
			std::vector<THRUSTER_HANDLE> oldThrusters,newThrusters;
			std::map<UINT, EXHAUSTSPEC *> exhausts;
			void SuppressStreams();
			void ResetStreams();
		};
	}
}

class LaunchTunnelHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	void clbkPostStep (double simt, double simdt, double mjd);
	int GetDoors();
	Door *GetDoor(int index);	
	int GetRooms();
	Room *GetRoom(int index);
	bool CheckVincinity(VECTOR3 *pos, int index=0);
	void SetPosition(VECTOR3 position);
	int GetChecklists();
	Checklist *GetChecklist(int index);
private:
	Door doors[LFMCDOORS];
	FoldingDoor foldingDoors[LFMCFDOORS];
	Room rooms[LFMCROOMS];
	LaunchTunnel::RequestChecklist::List request;
	LaunchTunnel::PreflightChecklist::List preflight;
	LaunchTunnel::BoardingChecklist::List boarding;
	LaunchTunnel::FuelingChecklist::List fueling;
	LaunchTunnel::LaunchChecklist::List launch;
	Checklist *lists[LFMCLISTS];
};