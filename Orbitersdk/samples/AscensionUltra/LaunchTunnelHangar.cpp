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
		if (event==Revert) return false;
		if (event==Proceed)
		{
			RecordEvent(event);
			((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), NULL);
			state=Ready;
			return true;
		}
		//intentional fall-through
	case Ready:
		if (event==Proceed) return false;
		if (event==Revert)
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
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
		if (event!=Abort) return false;
		
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
	Door *entry=hangar->GetDoor(0);

	if (subject==NULL)
	{
		state=Empty;
		//Fail-safe reset		
		if (entry->GetPosition()>0 && entry->GetMovement()>=0) entry->Close();
		return;
	}
	
	//Calculate local coordinates of subject w.r.t. hangar
	VECTOR3 global, local;
	oapiGetGlobalPos(subject, &global);
	owner->Global2Local(global, local);
	
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYDOCK);
	bool inPrepareArea=hangar->CheckVincinity(&local, VINCINITYPREPARE);
	BaseVessel::EventHandler::Arguments args={Aborted, BaseVessel::EventHandler::Checklist, this};
	switch(state)
	{
	case AbortOpen:
		if (inPrepareArea) return;
		((AscensionUltra *)owner)->SendEvent(args);
		state=Empty;
		subject=NULL;
		return;
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		entry->Open();
		state=OpenEntry;
		return;
	case OpenEntry:
		if (entry->GetPosition()>=1) state=Entry;
		//intentional fall-through
	case Entry:
		if (!vincinity) return;
		entry->Close();
		state=CloseEntry;
		return;
	case CloseEntry:
		if (entry->GetPosition()>0) return;
		state=Occupied;
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), oapiGetVesselInterface(subject));
		return;
	case Ready:
		if (vincinity) return;
		{
			//Hand-over to launch checklist
			Checklist *list=hangar->GetChecklist(1);
			list->SetSubject(subject);
			if (list->GetSubject()!=subject) return;
		}
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
		if (event==Proceed)
		{
			state=Beacons;
			//TODO: Beacons and exhaust simulation on
			break;
		}
		//intentional fall-through
	case Beacons:
	case Speeding:
	case TakeOff:
		if (event!=Abort) return false;
		RecordEvent(event);
		hangar->GetDoor(1)->Open(); //Open exit door
		hangar->GetDoor(2)->Open(); //"Open" (get out of the way) shield door
		//No need to open or close tunnel door, because it should be in a proper place already
		//TODO: exhaust and beacons off
		state=AbortOpen;
		return true;
	default:
		return false;
	}				
}

void LaunchTunnel::LaunchChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	Door *exit=hangar->GetDoor(1);
	Door *shield=hangar->GetDoor(2); //Note that shield door opening is "getting out of the way", closing is "putting into place for function"
	Door *door=hangar->GetDoor(3); //Note that tunnel door operation is reversed: open is closing, closing is opening

	if (subject==NULL)
	{
		state=Empty;
		//Fail-safe reset of animation		
		if (exit->GetPosition()>0 && exit->GetMovement()>=0) exit->Close();
		if (shield->GetPosition()<1 && shield->GetMovement()<=0) shield->Open();
		if (door->GetPosition()<1 && door->GetMovement()<=0) door->Open();
		return;
	}
	
	//Calculate local coordinates of subject w.r.t. hangar
	VECTOR3 global, local;
	oapiGetGlobalPos(subject, &global);
	owner->Global2Local(global, local);
	
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYHOLD);
	bool inExhaustArea=hangar->CheckVincinity(&local, VINCINITYEXHAUST);
	bool inTakeoffArea=hangar->CheckVincinity(&local, VINCINITYTAKEOFF);
	bool inLaunchArea=hangar->CheckVincinity(&local, VINCINITYLAUNCH);
	BaseVessel::EventHandler::Arguments args={Aborted, BaseVessel::EventHandler::Checklist, this};
	switch(state)
	{
	case AbortOpen:
		if (inLaunchArea) return;
		((AscensionUltra *)owner)->SendEvent(args);
		state=Empty;
		subject=NULL;
		return;
	case Empty:
		if (shield->GetPosition()<1) return;
		exit->Open();
		state=OpenExit;
		return;
	case OpenExit:
		if (exit->GetPosition()>=1) state=Exit;
		//intentional fall-through
	case Exit:
		if (!vincinity) return;
		exit->Close();
		state=CloseExit;
		return;
	case CloseExit:
		if (exit->GetPosition()>0) return;
		state=DeployShield;
		shield->Close();
		return;
	case DeployShield:
		if (shield->GetPosition()>0) return;
		state=Launch;
		return;
	case Launch:
		//Nothing to do here
		break;
	case Beacons:
		if (inExhaustArea) return;
		//TODO: Exhaust simulation off
		state=Speeding;
		return;
	case Speeding:
		if (oapiGetVesselInterface(subject)->GroundContact()) return;
		shield->Open();
		state=TakeOff;
		return;
	case TakeOff:
		if (inTakeoffArea) return;
		//TODO: Beacons off
		args.Event=Left;
		((AscensionUltra *)owner)->SendEvent(args);
		state=Empty;
		subject=NULL;		
		return;
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

int LaunchTunnelHangar::GetDoors(){return DOORS;}

Door *LaunchTunnelHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

int LaunchTunnelHangar::GetRooms(){return ROOMS;}

Room *LaunchTunnelHangar::GetRoom(int index){return (index>=0 && index<ROOMS)?rooms+index:NULL;}

int LaunchTunnelHangar::GetChecklists(){return 2;}
Checklist *LaunchTunnelHangar::GetChecklist(int index){return index==0?(Checklist *)&prepare:(index==1?(Checklist *)&launch:NULL);}

bool LaunchTunnelHangar::CheckVincinity(VECTOR3 *pos, int index)
{
	VECTOR3 range[6][2]={ DOCKRANGE , PREPARERANGE , HOLDRANGE , EXHAUSTRANGE , TAKEOFFRANGE , LAUNCHRANGE };
	range[index][0]=position+range[index][0];
	range[index][1]=position+range[index][1];
	return	pos->x>range[index][0].x && pos->x<range[index][1].x &&
			pos->y>range[index][0].y && pos->y<range[index][1].y &&
			pos->z>range[index][0].z && pos->z<range[index][1].z;
}

void LaunchTunnelHangar::SetPosition(VECTOR3 position){this->position=position;}