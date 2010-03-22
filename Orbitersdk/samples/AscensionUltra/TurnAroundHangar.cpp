#include "TurnAroundHangar.h"

TurnAroundHangar::TurnAroundHangar(void)
{
	crane1.SetSpeed(_V(10,10,10));
	crane1.SetCrawl(_V(1,1,1));

	doors_status      = DOOR_CLOSED;
	doors_proc        = 0.0;
}

TurnAroundHangar::~TurnAroundHangar(void)
{
}

void TurnAroundHangar::DefineAnimations ()
{
	static UINT DoorGrp[8] = {2,3,0,1,6,4,5,7};
	static MGROUP_ROTATE Door1 (0, DoorGrp, 1,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD));
	static MGROUP_ROTATE Door2 (0, DoorGrp+1, 1,	_V(0,0,0), _V(1,0,0), (float)(30*RAD));
	static MGROUP_TRANSLATE Door3 (0, DoorGrp+2, 1, _V(0,6,0));
	static MGROUP_TRANSLATE Door4 (0, DoorGrp+3, 1, _V(0,6,0));
	static MGROUP_TRANSLATE CraneX (0, DoorGrp+4, 1, _V(CRANEXOFFSET,0,0));
	static MGROUP_TRANSLATE CraneY (0, DoorGrp+5, 1, _V(0,0,CRANEYOFFSET));
	static MGROUP_TRANSLATE CraneZ (0, DoorGrp+7, 1, _V(0,-CRANEREELLOWERPOINT,0));
	static MGROUP_SCALE CraneReel (0, DoorGrp+6, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1));

	anim_doors = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_doors, 0, 1, &Door1);
	owner->AddAnimationComponent (anim_doors, 0, 1, &Door2);
	owner->AddAnimationComponent (anim_doors, 0, 1, &Door3);
	owner->AddAnimationComponent (anim_doors, 0, 1, &Door4);

	crane1.Init(owner, &CraneX, &CraneY, &CraneZ, &CraneReel);
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
	owner->RecordEvent ("DOOR", close ? "CLOSE" : "OPEN");
}

void TurnAroundHangar::RevertOuterAirlock ()
{
	ActivateOuterAirlock (doors_status == DOOR_CLOSED || doors_status == DOOR_CLOSING ?
		                  DOOR_OPENING : DOOR_CLOSING);
}

void TurnAroundHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate outer airlock
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
	
	if (!strnicmp (line, "DOOR", 4))
	{
		sscanf (line+4, "%d%d%lf", &doornrdummy, &doors_status, &doors_proc);
		return true;
	}
	else false;
}

// Write status to scenario file
void TurnAroundHangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	// Write custom parameters
	if (doors_status) {
		sprintf (cbuf, "0 %d %0.4f", doors_status, doors_proc);
		oapiWriteScenario_string (scn, "DOOR", cbuf);
	}	
}

void TurnAroundHangar::clbkPostCreation ()
{	
	owner->SetAnimation (anim_doors, doors_proc);	
}

void TurnAroundHangar::Init(VESSEL* owner)
{
	this->owner=owner;
}