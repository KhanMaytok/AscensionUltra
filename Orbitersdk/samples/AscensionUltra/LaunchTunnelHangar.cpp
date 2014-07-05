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

bool LaunchTunnel::RequestChecklist::List::SetEvent(int event)
{
	if (event!=Abort) return false;
	if (GetState()==Empty) return false;
	RecordEvent(event);
	hangar->SetIllumination("requesthold",false);
	SetState(Empty);
	subject=NULL;
	return true;
}

void LaunchTunnel::RequestChecklist::List::PostStep (double simt, double simdt, double mjd)
{	
	if (subject==NULL)
	{
		//fail-safe		
		SetState(Empty);
		return;
	}	
	VECTOR3 local=GetNosePoint();	
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYLFHOLD);
	Checklist *next=hangar->GetChecklist(1);
	BaseVessel::EventHandler::Arguments args={Step, BaseVessel::EventHandler::Checklist, this};
	switch(GetState())
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		hangar->SetIllumination("requesthold",true);
		if (vincinity)
		{
			next->SetSubject(subject);
			if (next->GetSubject()==subject)
			{
				hangar->SetIllumination("requesthold",false);
				SetState(Roll);
				return;
			}
			SetState(Wait);
			return;
		}
		SetState(LFHold);
		return;
	case LFHold:
		if (!vincinity) return;
		next->SetSubject(subject);
		if (next->GetSubject()==subject)
		{
			hangar->SetIllumination("requesthold",false);
			SetState(Roll);
			owner->SendEvent(args);
			return;
		}
		SetState(Wait);
		owner->SendEvent(args);
		return;
	case Wait:
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		hangar->SetIllumination("requesthold",false);
		SetState(Roll);
		owner->SendEvent(args);
		return;
	case Roll:
		if (vincinity) return;
		SetState(Empty);
		subject=NULL;
		return;
	}
}

bool LaunchTunnel::PreflightChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(2);
	switch(GetState())
	{
	case PFHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);			
			if (next->GetSubject()!=subject)
			{
				SetState(Wait);
				return true;
			}
			hangar->SetIllumination("preflighthold",false);
			SetState(Roll);
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
		hangar->SetIllumination("preflighthold",false);
		SetState(AbortOpen);
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
		SetState(Empty);
		if (entry->GetPosition()>0 && entry->GetMovement()>=0) entry->Close();
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYPFHOLD);
	bool area=hangar->CheckVincinity(&local, VINCINITYPFAREA);
	Checklist *next=hangar->GetChecklist(2);
	BaseVessel::EventHandler::Arguments args={Step, BaseVessel::EventHandler::Checklist, this};
	switch(GetState())
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		hangar->SetIllumination("preflighthold",true);
		SetState(OpenEntry);
		entry->Open();
		return;
	case OpenEntry:
		if (entry->GetPosition()<1) return;
		SetState(Entry);
		owner->SendEvent(args);
		return;
	case Entry:
		if (!vincinity) return;
		SetState(PFHold);
		entry->Close();
		owner->SendEvent(args);
		return;
	case PFHold:
		//Nothing to do here
		break;
	case Wait:
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		hangar->SetIllumination("preflighthold",false);
		SetState(Roll);
		owner->SendEvent(args);
		return;
	case Roll:
		if (vincinity) return;
		SetState(Empty);
		subject=NULL;
		return;
	case AbortOpen:
		if (area) return;
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		next->SetEvent(LaunchTunnel::BoardingChecklist::Abort);
		SetState(Empty);		
		subject=NULL;
		return;
	}
}

bool LaunchTunnel::BoardingChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(3);
	switch(GetState())
	{
	case PAXHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);
			if (next->GetSubject()!=subject)
			{
				SetState(Wait);
				return true;
			}
			hangar->SetIllumination("paxhold",false);
			SetState(Roll);
			return true;
		}
		//fall-through
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
	case Taxi:
	case Wait:
	case Roll:
		if (event!=Abort) return false;
		RecordEvent(event);
		hangar->SetIllumination("paxhold",false);
		SetState(AbortWait);
		owner->DockVessel(hangar->GetRoom(0), NULL);
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
		SetState(Empty);
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYPAXHOLD);
	bool area=hangar->CheckVincinity(&local, VINCINITYPAXAREA);
	Checklist *next=hangar->GetChecklist(3);
	BaseVessel::EventHandler::Arguments args={Step, BaseVessel::EventHandler::Checklist, this};
	switch(GetState())
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		hangar->SetIllumination("paxhold",true);
		SetState(Taxi);
		return;
	case Taxi:
		if (!vincinity) return;
		SetState(PAXHold);
		owner->DockVessel(hangar->GetRoom(0), oapiGetVesselInterface(subject));
		owner->SendEvent(args);
		return;
	case PAXHold:
		//Nothing to do here
		break;
	case Wait:
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		hangar->SetIllumination("paxhold",false);
		SetState(Roll);
		owner->SendEvent(args);
		return;
	case Roll:
		if (vincinity) return;
		SetState(Empty);
		subject=NULL;
		owner->DockVessel(hangar->GetRoom(0), NULL);
		return;
	case AbortWait:
		if (area) return;
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		SetState(Empty);
		next->SetEvent(LaunchTunnel::FuelingChecklist::Abort);
		subject=NULL;
		return;
	}
}

bool LaunchTunnel::FuelingChecklist::List::SetEvent(int event)
{
	Checklist *next=hangar->GetChecklist(4);
	switch(GetState())
	{
	case FuelHold:
		if (event==Proceed)
		{
			RecordEvent(event);			
			next->SetSubject(subject);
			if (next->GetSubject()!=subject)
			{
				SetState(Wait);
				return true;
			}
			hangar->SetIllumination("fuelhold",false);
			SetState(Roll);
			return true;
		}
		//fall-through
	case Empty: //Abort allowed even in empty state due to coupling with launch checklist aborts
	case Taxi:
	case Wait:
	case Roll:
		if (event!=Abort) return false;
		RecordEvent(event);
		hangar->SetIllumination("fuelhold",false);
		SetState(AbortWait);
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
		SetState(Empty);
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYFUELHOLD);
	bool area=hangar->CheckVincinity(&local, VINCINITYFUELAREA);
	Checklist *next=hangar->GetChecklist(4);
	BaseVessel::EventHandler::Arguments args={Step, BaseVessel::EventHandler::Checklist, this};
	switch(GetState())
	{
	case Empty:
		//If the overall condition of a valid subject is met, the next state is activated immediately
		hangar->SetIllumination("fuelhold",true);
		SetState(Taxi);
		return;
	case Taxi:
		if (!vincinity) return;
		SetState(FuelHold);
		owner->SendEvent(args);
		return;
	case FuelHold:
		//Nothing to do here
		break;
	case Wait:
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		hangar->SetIllumination("fuelhold",false);
		SetState(Roll);
		owner->SendEvent(args);
		return;
	case Roll:
		if (vincinity) return;
		SetState(Empty);
		subject=NULL;
		return;
	case AbortWait:
		if (area) return;
		next->SetSubject(subject);
		if (next->GetSubject()!=subject) return;
		next->SetEvent(LaunchTunnel::LaunchChecklist::Abort);
		SetState(Empty);		
		subject=NULL;
		return;
	}
}

void LaunchTunnel::LaunchChecklist::List::SuppressStreams()
{
	oldThrusters.clear();
	newThrusters.clear();
	exhausts.clear();
	VESSEL *vessel=oapiGetVesselInterface(subject);
	THGROUP_HANDLE group=vessel->GetThrusterGroupHandle(THGROUP_MAIN);
	double level=vessel->GetThrusterGroupLevel(group);
	THRUSTER_HANDLE thruster;
	for(int i=0;(thruster=vessel->GetGroupThruster(group, i))!=NULL;i++)
	{
		VECTOR3 pos,dir;
		vessel->GetThrusterRef(thruster, pos);
		vessel->GetThrusterDir(thruster, dir);
		THRUSTER_HANDLE newThruster =
			vessel->CreateThruster(	pos, dir,
									vessel->GetThrusterMax0(thruster),
									vessel->GetThrusterResource(thruster),
									vessel->GetThrusterIsp0(thruster));
		oldThrusters.push_back(thruster);
		newThrusters.push_back(newThruster);
		EXHAUSTSPEC exhaust, *newExhaust;
		for(int j=0;vessel->GetExhaustSpec(j, &exhaust);j++)
			if (exhaust.th==thruster)
			{
				newExhaust=new EXHAUSTSPEC(exhaust);
				newExhaust->th=newThruster;
				newExhaust->level=NULL;
				exhausts[vessel->AddExhaust(newExhaust)]=newExhaust;
				break;
			}
	}
	int k=newThrusters.size();
	if (k==0) return;
	vessel->SetThrusterGroupLevel(group, 0);
	THRUSTER_HANDLE *thrusters=new THRUSTER_HANDLE[k];
	for(int i=0;i<k;i++) thrusters[i]=newThrusters[i];
	group=vessel->CreateThrusterGroup(thrusters, k, THGROUP_MAIN);
	vessel->SetThrusterGroupLevel(group, level);
	delete [] thrusters;
}

void LaunchTunnel::LaunchChecklist::List::ResetStreams()
{
	VESSEL *vessel=oapiGetVesselInterface(subject);
	THGROUP_HANDLE group=vessel->GetThrusterGroupHandle(THGROUP_MAIN);
	double level=vessel->GetThrusterGroupLevel(group);
	int k=oldThrusters.size();
	if (k==0) return;
	vessel->SetThrusterGroupLevel(group, 0);
	THRUSTER_HANDLE *thrusters=new THRUSTER_HANDLE[k];
	for(int i=0;i<k;i++)
	{
		thrusters[i]=oldThrusters[i];
		vessel->DelThruster(newThrusters[i]);
	}
	for(std::map<UINT, EXHAUSTSPEC *>::iterator i=exhausts.begin();i!=exhausts.end();i++)
	{
		vessel->DelExhaust(i->first);
		delete i->second;
	}
	group=vessel->CreateThrusterGroup(thrusters, k, THGROUP_MAIN);
	vessel->SetThrusterGroupLevel(group, level);
	delete [] thrusters;	
}
	
bool LaunchTunnel::LaunchChecklist::List::SetEvent(int event)
{
	switch(GetState())
	{
	case TakeOff:
		return false; //No abort after take-off	
	case LaunchHold:
		if (event==Proceed)
		{
			hangar->SetIllumination("launchhold",false);
			hangar->SetIllumination("launch",true);
			SetState(Beacons);
			SuppressStreams();			
			return true;
		}
		goto blast; //Skipping exhaust toggle, not on yet.
	case Beacons:
	case Speeding:	
		if (event!=Abort) return false;		
		ResetStreams();
		//fall-through
	case Blast:
 blast: if (event!=Abort) return false;
		hangar->GetDoor(1)->Open(); //Open exit door
		hangar->GetDoor(2)->Open(); //"Open" (get out of the way) blast shield
		hangar->GetDoor(3)->Open(); //Close tunnel door
		//fall-through
	case Exit:
	case OpenExit:
	case Empty:
		if (event!=Abort) return false;
		RecordEvent(event);
		hangar->SetIllumination("tolaunch",false);
		hangar->SetIllumination("launchhold",false);
		hangar->SetIllumination("launch",false);
		hangar->SetIllumination("exitline",true);
		SetState(AbortOpen);
		hangar->GetDoor(1)->Open(); //Open exit door
		hangar->GetDoor(4)->Open(); //Open escape door
		return true;	
	default:
		return false;
	}				
}

void LaunchTunnel::LaunchChecklist::List::PostStep (double simt, double simdt, double mjd)
{
	Door *exit=hangar->GetDoor(1);
	Door *shield=hangar->GetDoor(2); //Note that blast shield opening is "getting out of the way", closing is "putting into place for function"
	Door *door=hangar->GetDoor(3);   //Note that tunnel door operation is reversed: open is closing, closing is opening
	Door *escape=hangar->GetDoor(4);

	if (subject==NULL)
	{
		SetState(Empty);
		//Fail-safe reset of animation		
		if (exit->GetPosition()>0 && exit->GetMovement()>=0) exit->Close();
		if (shield->GetPosition()<1 && shield->GetMovement()<=0) shield->Open();
		if (door->GetPosition()<1 && door->GetMovement()<=0) door->Open();
		if (escape->GetPosition()>0 && escape->GetMovement()>=0) escape->Close();
		return;
	}
	
	VECTOR3 local=GetNosePoint();
	bool vincinity=hangar->CheckVincinity(&local, VINCINITYLAUNCHHOLD);
	bool inExhaustArea=hangar->CheckVincinity(&local, VINCINITYEXHAUST);
	bool inTakeoffArea=hangar->CheckVincinity(&local, VINCINITYTAKEOFF);
	bool inAbortHold=hangar->CheckVincinity(&local, VINCINITYABORT);
	BaseVessel::EventHandler::Arguments args={Step, BaseVessel::EventHandler::Checklist, this};
	switch(GetState())
	{
	case AbortOpen:
		if (inTakeoffArea && !inAbortHold) return;
		hangar->SetIllumination("exitline",false);
		args.Event=Aborted;
		//The order before clearing the subject here is important! The event handler needs the subject in order to determine if it is a valid checklist event.
		owner->SendEvent(args);
		SetState(Empty);
		subject=NULL;
		return;
	case Empty:
		if (shield->GetPosition()<1) return;
		exit->Open();
		SetState(OpenExit);
		owner->SendEvent(args);
		return;
	case OpenExit:
		if (exit->GetPosition()<1) return;
		hangar->SetIllumination("tolaunch",true);
		SetState(Exit);
		owner->SendEvent(args);
		return;
	case Exit:
		if (!vincinity) return;
		hangar->SetIllumination("tolaunch",false);
		exit->Close();
		shield->Close();
		door->Close();
		SetState(Blast);
		owner->SendEvent(args);
		return;
	case Blast:
		if (shield->GetPosition()>0) return;
		hangar->SetIllumination("launchhold",true); //Note: small inefficiency here: while shield is closed but rest is still in transit, beacons will repeatedly lit up
		if (exit->GetPosition()>0) return;		
		if (door->GetPosition()>0) return;
		SetState(LaunchHold);
		owner->SendEvent(args);
		return;
	case LaunchHold:
		//Nothing to do here
		break;
	case Beacons:
		if (inExhaustArea) return;
		ResetStreams();
		SetState(Speeding);
		owner->SendEvent(args);
		return;
	case Speeding:
		if (oapiGetVesselInterface(subject)->GroundContact()) return;
		shield->Open();
		door->Open();
		SetState(TakeOff);
		owner->SendEvent(args);
		return;
	case TakeOff:
		if (inTakeoffArea) return;
		hangar->SetIllumination("launch",false);
		args.Event=Left;
		//The order before clearing the subject here is important! The event handler needs the subject in order to determine if it is a valid checklist event.
		owner->SendEvent(args);
		SetState(Empty);
		subject=NULL;
		return;
	}
}

void LaunchTunnelHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	Hangar::clbkPostStep(simt,simdt,mjd);
	//Check and update guidance beacon strobing/color according to checklist states
	int p=preflight.GetState();
	int b=boarding.GetState();
	int f=fueling.GetState();
	int l=launch.GetState();
	bool abort =
		p==LaunchTunnel::PreflightChecklist::AbortOpen ||
		b==LaunchTunnel::BoardingChecklist::AbortWait ||
		f==LaunchTunnel::FuelingChecklist::AbortWait;
	bool tofueling =
		p!=LaunchTunnel::PreflightChecklist::Empty ||
		b!=LaunchTunnel::BoardingChecklist::Empty ||
		f!=LaunchTunnel::FuelingChecklist::Empty;
	Group *beacons = (Group *)beaconGroup["tofueling"];
	if (beacons && (*beacons)[0]->GetType()==TypeBeaconArray)
	{
		BeaconArray *beacon=(BeaconArray *)((*beacons)[0]);
		if (beacon->On())
		{
			if (!tofueling) beacons->Switch(false);
		}
		else if (tofueling) beacons->Switch(true);
		VECTOR3 color;
		beacon->GetColor(color);
		if (color.y!=0)
		{
			if (abort) beacon->SetColor(_V(1,0,0));
		}
		else if (!abort) beacon->SetColor(_V(0,1,0));
	}
	abort = l==LaunchTunnel::LaunchChecklist::AbortOpen;
	beacons = (Group *)beaconGroup["exitline"];
	if (beacons && (*beacons)[0]->GetType()==TypeBeaconArray)
	{
		BeaconArray *beacon=(BeaconArray *)((*beacons)[0]);
		VECTOR3 color;
		beacon->GetColor(color);
		if (color.y!=0)
		{
			if (abort) for(int i=0;(beacon=(BeaconArray *)((*beacons)[i]))!=NULL;i++) beacon->SetColor(_V(1,0,0));
		}
		else if (!abort) for(int i=0;(beacon=(BeaconArray *)((*beacons)[i]))!=NULL;i++) beacon->SetColor(_V(0,1,0));
	}
}
	
int LaunchTunnelHangar::GetType(){return HANGARTYPELFMC;}

void LaunchTunnelHangar::DefineAnimations ()
{
	static UINT DoorGrp[8] = {0,1,2,3,4,5,6,7};
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
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1,	_V(47,0,0)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	foldingDoors[0].Init(owner, "Escape Door", prefix, 4,
		new MGROUP_ROTATE(meshIndex, DoorGrp+4, 1,	_V( -943.6,0,-59.52), _V(0,1,0), (float)( -90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+5, 1,	_V( -967.6,0,-57.52), _V(0,1,0), (float)( 180*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+6, 1,	_V(-1039.6,0,-59.52), _V(0,1,0), (float)(  90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+7, 1,	_V(-1015.6,0,-57.52), _V(0,1,0), (float)(-180*RAD)));

	rooms[0].Init(owner, this, "Control Room", _V(111,20,47), _V(0,0,-1), _V(112.96,0,49.28), 20);
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

	//Switch some beacons off by default
	SetIllumination("requesthold",false);
	SetIllumination("preflighthold",false);
	SetIllumination("paxhold",false);
	SetIllumination("fuelhold",false);
	SetIllumination("launchhold",false);
	SetIllumination("tolaunch",false);
	SetIllumination("launch",false);
}

int LaunchTunnelHangar::GetDoors(){return LFMCDOORS+LFMCFDOORS;}

Door *LaunchTunnelHangar::GetDoor(int index){return index<0?NULL:(index<LFMCDOORS?doors+index:(index<LFMCDOORS+LFMCFDOORS?foldingDoors+index-LFMCDOORS:NULL));}

int LaunchTunnelHangar::GetRooms(){return LFMCROOMS;}

Room *LaunchTunnelHangar::GetRoom(int index){return (index>=0 && index<LFMCROOMS)?rooms+index:NULL;}

int LaunchTunnelHangar::GetChecklists(){return LFMCLISTS;}
Checklist *LaunchTunnelHangar::GetChecklist(int index){return (index>=0 && index<LFMCLISTS)?lists[index]:NULL;}

bool LaunchTunnelHangar::CheckVincinity(VECTOR3 *pos, int index)
{
	VECTOR3 range[11][2]=
	{ PAXHOLDRANGE , PFHOLDRANGE , LAUNCHHOLDRANGE , 
	  EXHAUSTRANGE , TAKEOFFRANGE , FUELHOLDRANGE, 
	  LFHOLDRANGE, PFAREARANGE, PAXAREARANGE,
	  FUELAREARANGE , ABORTRANGE };
	range[index][0]=position+range[index][0];
	range[index][1]=position+range[index][1];
	return	pos->x>range[index][0].x && pos->x<range[index][1].x &&
			pos->y>range[index][0].y && pos->y<range[index][1].y &&
			pos->z>range[index][0].z && pos->z<range[index][1].z;
}

void LaunchTunnelHangar::SetPosition(VECTOR3 position){this->position=position;}