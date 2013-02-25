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

bool LaunchTunnel::RequestChecklist::List::SetEvent(int event)
{
	if (event!=Abort) return false;
	if (state==Empty) return false;
	RecordEvent(event);
	state=Empty;
	subject=NULL;
	return true;
}

void LaunchTunnel::RequestChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	if (subject==NULL)
	{
		//fail-safe
		state=Empty;
		return;
	}
	
	VECTOR3 local=GetNosePoint();	
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYLFHOLD);
	Checklist *next=hangar->GetChecklist(1);
	switch(state)
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		state=LFHold;
		//fall-through
	case LFHold:
		if (!vincinity) return;
		next->SetSubject(subject);
		state=Wait;
		//fall-through
	case Wait:
		if (next->GetSubject()!=subject) return;
		state=Roll;		
		return;
	case Roll:
		if (vincinity) return;
		state=Empty;
		subject=NULL;
		return;
	}
}

bool LaunchTunnel::PreflightChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(2);
	switch(state)
	{
	case PFHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);
			state=Wait;
			if (next->GetSubject()!=subject) return true;
			state=Roll;
			return true;
		}
		//fall-through
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
	case OpenEntry:
	case Entry:
	case Wait:
	case Roll:
		if (event!=Abort) return false;
		RecordEvent(event);
		state=AbortOpen;
		next->SetSubject(subject);
		hangar->GetDoor(0)->Open();
		return true;
	default:
		return false;
	}
}

void LaunchTunnel::PreflightChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	Door *entry=hangar->GetDoor(0);

	if (subject==NULL)
	{
		//fail-safe
		state=Empty;
		if (entry->GetPosition()>0 && entry->GetMovement()>=0) entry->Close();
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYPFHOLD);
	Checklist *next=hangar->GetChecklist(2);
	switch(state)
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		state=OpenEntry;
		entry->Open();
		return;
	case OpenEntry:
		if (entry->GetPosition()>=1) state=Entry;
		return;
	case Entry:
		if (!vincinity) return;
		state=PFHold;
		entry->Close();
		return;
	case PFHold:
		//Nothing to do here
		break;
	case Wait:
		if (next->GetSubject()!=subject) return;
		state=Roll;		
		return;
	case Roll:
		if (vincinity) return;
		state=Empty;
		subject=NULL;
		return;
	case AbortOpen:
		if (vincinity) return;
		if (next->GetSubject()!=subject) return;
		state=Empty;
		next->SetEvent(LaunchTunnel::BoardingChecklist::Abort);
		subject==NULL;
		return;
	}
}

bool LaunchTunnel::BoardingChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(3);
	switch(state)
	{
	case PAXHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);
			state=Wait;
			if (next->GetSubject()!=subject) return true;
			state=Roll;
			return true;
		}
		//fall-through
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
	case Taxi:
	case Wait:
	case Roll:
		if (event!=Abort) return false;
		RecordEvent(event);
		state=AbortWait;
		next->SetSubject(subject);
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), NULL);
		return true;
	default:
		return false;
	}
}

void LaunchTunnel::BoardingChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	if (subject==NULL)
	{
		//fail-safe
		state=Empty;
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYPAXHOLD);
	Checklist *next=hangar->GetChecklist(3);
	switch(state)
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		state=Taxi;
		return;
	case Taxi:
		if (!vincinity) return;
		state=PAXHold;
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), oapiGetVesselInterface(subject));
		return;
	case PAXHold:
		//Nothing to do here
		break;
	case Wait:
		if (next->GetSubject()!=subject) return;
		state=Roll;		
		return;
	case Roll:
		if (vincinity) return;
		state=Empty;
		subject=NULL;
		((AscensionUltra *)owner)->DockVessel(hangar->GetRoom(0), NULL);
		return;
	case AbortWait:
		if (vincinity) return;
		if (next->GetSubject()!=subject) return;
		state=Empty;
		next->SetEvent(LaunchTunnel::FuelingChecklist::Abort);
		subject==NULL;
		return;
	}
}

bool LaunchTunnel::FuelingChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(4);
	switch(state)
	{
	case FuelHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);
			state=Wait;
			if (next->GetSubject()!=subject) return true;
			state=Roll;
			return true;
		}
		//fall-through
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
	case Taxi:
	case Wait:
	case Roll:
		if (event!=Abort) return false;
		RecordEvent(event);
		state=AbortWait;
		next->SetSubject(subject);
		return true;
	default:
		return false;
	}
}

void LaunchTunnel::FuelingChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	if (subject==NULL)
	{
		//fail-safe
		state=Empty;
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYLAUNCHHOLD);
	Checklist *next=hangar->GetChecklist(4);
	switch(state)
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		state=Taxi;
		return;
	case Taxi:
		if (!vincinity) return;
		state=FuelHold;
		return;
	case FuelHold:
		//Nothing to do here
		break;
	case Wait:
		if (next->GetSubject()!=subject) return;
		state=Roll;		
		return;
	case Roll:
		if (vincinity) return;
		state=Empty;
		subject=NULL;
		return;
	case AbortWait:
		if (vincinity) return;
		if (next->GetSubject()!=subject) return;
		state=Empty;
		next->SetEvent(LaunchTunnel::LaunchChecklist::Abort);
		subject==NULL;
		return;
	}
}
	
bool LaunchTunnel::LaunchChecklist::List::SetEvent(int event)
{
	switch(state)
	{
	case TakeOff:
		return false; //No abort after take-off	
	case LaunchHold:
		if (event==Proceed)
		{
			state=Beacons;
			//TODO: Beacons on
			//TODO: Exhaust on
			return true;
		}
		goto blast; //Skipping exhaust toggle, not on yet.
	case Beacons:
	case Speeding:	
		if (event!=Abort) return false;		
		//TODO: Exhaust off
		//fall-through
	case Blast:
 blast: if (event!=Abort) return false;
		hangar->GetDoor(1)->Open(); //Open exit door
		hangar->GetDoor(2)->Open(); //"Open" (get out of the way) blast shield
		hangar->GetDoor(3)->Open(); //Close tunnel door
		//fall-through
	case Exit:
	case OpenExit:
		if (event!=Abort) return false;
		RecordEvent(event);
		state=AbortOpen;
		//TODO: Beacons error
		return true;	
	default:
		return false;
	}				
}

void LaunchTunnel::LaunchChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	Door *exit=hangar->GetDoor(1);
	Door *shield=hangar->GetDoor(2); //Note that blast shield opening is "getting out of the way", closing is "putting into place for function"
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
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYLAUNCHHOLD);
	bool inExhaustArea=hangar->CheckVincinity(&local, VINCINITYEXHAUST);
	bool inTakeoffArea=hangar->CheckVincinity(&local, VINCINITYTAKEOFF);
	BaseVessel::EventHandler::Arguments args={Aborted, BaseVessel::EventHandler::Checklist, this};
	switch(state)
	{
	case AbortOpen:
		if (inTakeoffArea) return;
		((AscensionUltra *)owner)->SendEvent(args);
		//TODO: Beacons off
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
		return;
	case Exit:
		if (!vincinity) return;
		exit->Close();
		shield->Close();
		door->Close();
		state=Blast;
		return;
	case Blast:
		if (exit->GetPosition()>0) return;
		if (shield->GetPosition()>0) return;
		if (door->GetPosition()>0) return;
		state=LaunchHold;
		return;
	case LaunchHold:
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
		door->Open();
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
	char prefix[40]="";
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
	
	i=0;
	sprintf(prefix, "%sREQUEST", event_prefix);
	request.Init(owner, this, prefix, LaunchTunnel::RequestChecklist::Empty);
	lists[i++]=&request;
	sprintf(prefix, "%sPREFLIGHT", event_prefix);
	preflight.Init(owner, this, prefix, LaunchTunnel::PreflightChecklist::Empty);
	lists[i++]=&preflight;
	sprintf(prefix, "%sBOARDING", event_prefix);
	boarding.Init(owner, this, prefix, LaunchTunnel::BoardingChecklist::Empty);
	lists[i++]=&boarding;
	sprintf(prefix, "%sFUELING", event_prefix);
	fueling.Init(owner, this, prefix, LaunchTunnel::FuelingChecklist::Empty);
	lists[i++]=&fueling;
	sprintf(prefix, "%sLAUNCH", event_prefix);
	launch.Init(owner, this, prefix, LaunchTunnel::LaunchChecklist::Empty);
	lists[i++]=&launch;

	Hangar::DefineAnimations();
}

int LaunchTunnelHangar::GetDoors(){return DOORS;}

Door *LaunchTunnelHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

int LaunchTunnelHangar::GetRooms(){return ROOMS;}

Room *LaunchTunnelHangar::GetRoom(int index){return (index>=0 && index<ROOMS)?rooms+index:NULL;}

int LaunchTunnelHangar::GetChecklists(){return LISTS;}
Checklist *LaunchTunnelHangar::GetChecklist(int index){return (index>=0 && index<LISTS)?lists[index]:NULL;}

bool LaunchTunnelHangar::CheckVincinity(VECTOR3 *pos, int index)
{
	VECTOR3 range[7][2]={ PAXHOLDRANGE , PFHOLDRANGE , LAUNCHHOLDRANGE , EXHAUSTRANGE , TAKEOFFRANGE , FUELHOLDRANGE, LFHOLDRANGE };
	range[index][0]=position+range[index][0];
	range[index][1]=position+range[index][1];
	return	pos->x>range[index][0].x && pos->x<range[index][1].x &&
			pos->y>range[index][0].y && pos->y<range[index][1].y &&
			pos->z>range[index][0].z && pos->z<range[index][1].z;
}

void LaunchTunnelHangar::SetPosition(VECTOR3 position){this->position=position;}