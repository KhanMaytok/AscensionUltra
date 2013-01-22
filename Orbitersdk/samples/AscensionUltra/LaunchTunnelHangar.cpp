// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LaunchTunnel.cpp
// Class implementation of launch tunnel building.
// ==============================================================
#pragma once
#include "LaunchTunnelHangar.h"
#include "Module.h"
#include "AscensionUltra.h"

//Redefinition, because AscensionUltra.h unfortunately overwrites them
#define DOORS	4
#define ROOMS	2

bool LaunchTunnel::PrepareChecklist::List::SetEvent(int event)
{
	switch(state)
	{
	case Occupied:
		if (event==Event::Revert) return false;
		if (event==Event::Proceed)
		{
			RecordEvent(event);
			((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), NULL);
			state=Ready;
			return true;
		}
		//intentional fall-through
	case Ready:
		if (event==Event::Proceed) return false;
		if (event==Event::Revert)
		{
			RecordEvent(event);
			((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), oapiGetVesselInterface(subject));
			state=Occupied;
			return true;
		}
		//intentional fall-through
	case OpenEntry:
	case Entry:
	case CloseEntry:				
		if (event!=Event::Abort) return false;
		
		//Undock and open entry
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), NULL);
		hangar->GetDoor(0)->Open();

		RecordEvent(event);
		state=AbortOpen;
		return true;
	default:
		return false;
	}
}

void LaunchTunnel::PrepareChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	if (subject==NULL) return;
	
	//Calculate local coordinates of subject w.r.t. hangar
	VECTOR3 global, local;
	oapiGetGlobalPos(subject, &global);
	owner->Global2Local(global, local);
	global=local-hangar->GetPosition();

	bool vincinity=hangar->CheckVincinity(&local);
	switch(state)
	{
	case AbortOpen:
		//TODO: check area clearance
		return;
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		hangar->GetDoor(0)->Open();
		state=OpenEntry;
		return;
	case OpenEntry:
		if (hangar->GetDoor(0)->GetPosition()>=1) state=Entry;
		//intentional fall-through
	case Entry:
		if (!vincinity) return;
		hangar->GetDoor(0)->Close(); //Door 0 is entry
		state=CloseEntry;
		return;
	case CloseEntry:
		if (hangar->GetDoor(0)->GetPosition()>0) return; //Door 0 is entry
		state=Occupied;
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), oapiGetVesselInterface(subject));
		return;
	case Ready:
		if (vincinity) return;
		state=Empty;
		subject=NULL;
		return;
	case Occupied:
		//Nothing to do here
		break;
	}
}		
	
bool LaunchTunnel::LaunchChecklist::List::SetEvent(int event)
{
	switch(state)
	{
	case OpenExit:
	case Exit:
	case CloseExit:
	case DeployShield:
	case Launch:
	case Beacons:
	case Speeding:
	case TakeOff:
		if (event!=Event::Abort) return false;
		RecordEvent(event);
		state=AbortOpen;
		GetHangar()->GetChecklist(0)->SetEvent(PrepareChecklist::AbortOpen);
		return true;
	default:
		return false;
	}				
}
	
int LaunchTunnelHangar::GetType(){return HANGARTYPELFMC;}

void LaunchTunnelHangar::DefineAnimations ()
{
	static UINT DoorGrp[4] = {0,1,2,3};
	char prefix[20]="";
	int i=0;
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[0].Init(owner, "Main Door", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp, 1,	_V(223,0,0), _V(0,0,1), (float)(45*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[1].Init(owner, "Exit Door", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp+1, 1,	_V(0,0,0), _V(0,0,1), (float)(-45*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[2].Init(owner, "Blast Shield", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp+2, 1,	_V(-189.3,0,47.5), _V(0,1,0), (float)(-90*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, "Tunnel Door", prefix, 1,
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1,	_V(-47,0,0)));

	rooms[0].Init(owner, this, "Control Room", _V(111,20,47), _V(0,0,-1), _V(115,0,50), 20);
	rooms[1].Init(owner, this, "Control Tower", _V(111,218,70), _V(0,0,-1));

	sprintf(prefix, "%sPREPARE", event_prefix);
	prepare.Init(owner, this, prefix, LaunchTunnel::PrepareChecklist::Empty);
	sprintf(prefix, "%sLAUNCH", event_prefix);
	launch.Init(owner, this, prefix, LaunchTunnel::LaunchChecklist::Empty);

	Hangar::DefineAnimations();
}

void LaunchTunnelHangar::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	//Close tunnel door
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=meshIndex;
	mt.ngrp=3;
	mt.transform=mt.TRANSLATE;
	mt.P.transparam.shift=_V(47,0,0);	
	owner->MeshgroupTransform(vis, mt);
}

int LaunchTunnelHangar::GetDoors(){return DOORS;}

Door *LaunchTunnelHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

int LaunchTunnelHangar::GetRooms(){return ROOMS;}

Room *LaunchTunnelHangar::GetRoom(int index){return (index>=0 && index<ROOMS)?rooms+index:NULL;}

int LaunchTunnelHangar::GetChecklists(){return 2;}
Checklist *LaunchTunnelHangar::GetChecklist(int index){return index==0?(Checklist *)&prepare:(index==1?(Checklist *)&launch:NULL);}

bool LaunchTunnelHangar::CheckVincinity(VECTOR3 *pos){return pos->x>position.x+85 && pos->x<position.x+145 && pos->z<position.z+30 && pos->z>position.z-30;}

void LaunchTunnelHangar::SetPosition(VECTOR3 position){this->position=position;}