#include "TurnAroundHangar.h"

TurnAroundHangar::TurnAroundHangar(void)
{
	crane1.SetSpeed(_V(10,10,10));
	crane1.SetCrawl(_V(1,1,1));

	doors_status      = DOOR_CLOSED;
	doors_proc        = 0.0;

	event_prefix=NULL;

	cur_crane=-1;
}

TurnAroundHangar::~TurnAroundHangar(void)
{
	delete [] event_prefix;
}

void TurnAroundHangar::DefineAnimations ()
{
	static UINT DoorGrp[8] = {2,3,0,1,6,4,5,7};
	door1=new MGROUP_ROTATE(meshIndex, DoorGrp, 1,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD));
	door2=new MGROUP_ROTATE(meshIndex, DoorGrp+1, 1,	_V(0,0,0), _V(1,0,0), (float)(30*RAD));
	door3=new MGROUP_TRANSLATE(meshIndex, DoorGrp+2, 1, _V(0,6,0));
	door4=new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,6,0));
	craneX=new MGROUP_TRANSLATE(meshIndex, DoorGrp+4, 1, _V(CRANEXOFFSET,0,0));
	craneY=new MGROUP_TRANSLATE(meshIndex, DoorGrp+5, 1, _V(0,0,CRANEYOFFSET));
	craneZ=new MGROUP_TRANSLATE(meshIndex, DoorGrp+7, 1, _V(0,-CRANEREELLOWERPOINT,0));
	craneReel=new MGROUP_SCALE(meshIndex, DoorGrp+6, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1));

	anim_doors = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_doors, 0, 1, door1);
	owner->AddAnimationComponent (anim_doors, 0, 1, door2);
	owner->AddAnimationComponent (anim_doors, 0, 1, door3);
	owner->AddAnimationComponent (anim_doors, 0, 1, door4);

	char prefix[20]="";
	sprintf(prefix, "%sCRANE%d", event_prefix, 0);
	crane1.Init(owner, craneX, craneY, craneZ, craneReel, prefix);
	crane1.DefineAnimations();

}

void TurnAroundHangar::ActivateOuterAirlock (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	doors_status = action;
	if (action <= DOOR_OPEN) {
		doors_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		owner->SetAnimation (anim_doors, doors_proc);
	}
	char event_type[20]="";
	sprintf(event_type, "%sDOOR%d", event_prefix, 0);	
	owner->RecordEvent (event_type, close ? "CLOSE" : "OPEN");
}

void TurnAroundHangar::RevertOuterAirlock ()
{
	ActivateOuterAirlock (doors_status == DOOR_CLOSED || doors_status == DOOR_CLOSING ?
		                  DOOR_OPENING : DOOR_CLOSING);
}

void TurnAroundHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	if (doors_status >= DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (doors_status == TurnAroundHangar::DOOR_CLOSING) {
			if (doors_proc > 0.0) doors_proc = max (0.0, doors_proc-da);
			else doors_status = DOOR_CLOSED;
		} else { // door opening
			if (doors_proc < 1.0) doors_proc = min (1.0, doors_proc+da);
			else doors_status = DOOR_OPEN;
		}
		owner->SetAnimation (anim_doors, doors_proc);
	}
	crane1.PostStep(simt, simdt, mjd);
}

bool TurnAroundHangar::clbkLoadStateEx (char *line)
{
    int doornrdummy;
	
	if (!strnicmp (line, "CRANE", 5)) sscanf (line+5, "%d", &cur_crane);
	else if (cur_crane>=0 && cur_crane<1) return crane1.clbkLoadStateEx(line);
	else if (!strnicmp (line, "DOOR", 4)) sscanf (line+4, "%d%d%lf", &doornrdummy, &doors_status, &doors_proc);
	else return false;
	return true;
}

void TurnAroundHangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;
	// Write custom parameters
	if (doors_status) {
		sprintf (cbuf, "0 %d %0.4f", doors_status, doors_proc);
		oapiWriteScenario_string (scn, "DOOR", cbuf);
	}

	for(i=0;i<1;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "CRANE", cbuf);
		crane1.clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "CRANE", cbuf);
}

void TurnAroundHangar::clbkPostCreation ()
{	
	owner->SetAnimation (anim_doors, doors_proc);
	crane1.clbkPostCreation();
}

void TurnAroundHangar::Init(VESSEL* owner, UINT meshIndex, const char *event_prefix)
{
	this->owner=owner;
	this->meshIndex=meshIndex;
	strcpy(this->event_prefix=new char[strlen(event_prefix)], event_prefix);
}

Crane *TurnAroundHangar::GetCrane()
{
	return &crane1;
}

bool TurnAroundHangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "CRANE", 5))
	{
		//Hangar event
		int crane=(int)(event_type+5)[0]-0x30;
		if (crane>=0 && crane<1) return crane1.clbkPlaybackEvent(simt, event_t, event_type+5, event);
	}
	if (!strnicmp (event_type, "DOOR", 4))
	{
		//Hangar event
		int door=(int)(event_type+4)[0]-0x30;
		if (door>=0 && door<4)
		{	
			ActivateOuterAirlock (!stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		}		
		return true;
	}
	return false;
}