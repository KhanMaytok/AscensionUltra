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

#include "TurnAroundHangar.h"

TurnAroundHangar::TurnAroundHangar(void):Hangar()
{
	crane1.SetSpeed(_V(0.5,0.5,0.5));
	crane1.SetCrawl(_V(0.1,0.1,0.1));	
}

int TurnAroundHangar::GetType(){return HANGARTYPETA;}

void TurnAroundHangar::DefineAnimations ()
{
	static UINT DoorGrp[10] = {0,1,2,3,4,8,5,9,6,7};
	char prefix[20]="";
	int i=0;

	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[0].Init(owner, "South Main Door", prefix, 1, new MGROUP_ROTATE(meshIndex, DoorGrp+4, 2,	_V(0,0,0), _V(-1,0,0), (float)(30*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, "North Main Door", prefix, 1, new MGROUP_ROTATE(meshIndex, DoorGrp+6, 2,	_V(0,0,0), _V(1,0,0), (float)(30*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[2].Init(owner, "East Maintenance Door", prefix, 1, new MGROUP_TRANSLATE(meshIndex, DoorGrp+8, 1, _V(0,6,0)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, "West Maintenance Door", prefix, 1, new MGROUP_TRANSLATE(meshIndex, DoorGrp+9, 1, _V(0,6,0)));
	Hangar::DefineAnimations();
	
	sprintf(prefix, "%sCRANE%d", event_prefix, 0);
	crane1.Init(owner, "Cargo Crane",
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+0, 1, _V(CRANEXOFFSET,0,0)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+1, 1, _V(0,0,CRANEYOFFSET)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,-CRANEREELLOWERPOINT,0)),
		new MGROUP_SCALE(meshIndex, DoorGrp+2, 1, _V(0,CRANEREELUPPERPOINT,0), _V(1,CRANEREELUPPERPOINT/CRANEREELHEIGHT,1)),
		prefix);

	char *name[TAROOMS]={"East Control Room","West Control Room"};
	VECTOR3 room[TAROOMS][3]={ TAROOM_EAST , TAROOM_WEST };
	for(int i=0;i<TAROOMS;i++) rooms[i].Init(owner, this, name[i], room[i][0], room[i][1], room[i][2], 4 );

	crane1.DefineAnimations();
}

void TurnAroundHangar::clbkPostStep (double simt, double simdt, double mjd)
{
	Hangar::clbkPostStep(simt, simdt, mjd);
	crane1.PostStep(simt, simdt, mjd);
}

bool TurnAroundHangar::clbkLoadStateEx (char *line)
{
	if (Hangar::clbkLoadStateEx(line)) return true;
	else if (!_strnicmp (line, "CRANE", 5)) sscanf (line+5, "%d", &cur_crane);
	else if (cur_crane>=0 && cur_crane<1) return crane1.clbkLoadStateEx(line);
	return false;
}

void TurnAroundHangar::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;
	// Write custom parameters
	Hangar::clbkSaveState(scn);
	for(i=0;i<1;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "\tCRANE", cbuf);
		crane1.clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "\tCRANE", cbuf);
}

void TurnAroundHangar::clbkPostCreation ()
{	
	Hangar::clbkPostCreation();
	crane1.clbkPostCreation();
}

void TurnAroundHangar::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	//Texture handling for TA
	DEVMESHHANDLE mesh=owner->GetDevMesh(vis, meshIndex);
	MESHHANDLE meshTemplate=owner->GetMeshTemplate(meshIndex);
	for(int k=8;k<10;k++)
	{
		MESHGROUPEX *group=oapiMeshGroupEx(meshTemplate, k); //Get original vertices
		GROUPEDITSPEC change;
		change.flags=GRPEDIT_VTXTEXU; //Change only U coordinates
		change.nVtx=group->nVtx;
		change.vIdx=NULL; //Just use the mesh order
		change.Vtx=new NTVERTEX[group->nVtx];
		for(int i=0;i<(int)group->nVtx;i++)
		{
			change.Vtx[i]=group->Vtx[i];
			change.Vtx[i].tu+=(float)(TEXTURE_OFFSET*instance); //Offset the U coordinate		
		}
		oapiEditMeshGroup(mesh, k, &change);
		delete [] change.Vtx;
	}
}

Crane *TurnAroundHangar::GetCrane(){return &crane1;}

int TurnAroundHangar::GetDoors(){return TADOORS;}

Door *TurnAroundHangar::GetDoor(int index){return (index>=0 && index<TADOORS)?doors+index:NULL;}

int TurnAroundHangar::GetRooms(){return TAROOMS;}

Room *TurnAroundHangar::GetRoom(int index){return (index>=0 && index<TAROOMS)?rooms+index:NULL;}

void TurnAroundHangar::SetPosition(VECTOR3 position){this->position=position;}

bool TurnAroundHangar::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!_strnicmp (event_type, "CRANE", 5))
	{
		//Crane event
		int crane=(int)(event_type+5)[0]-0x30;
		if (crane>=0 && crane<1) return crane1.clbkPlaybackEvent(simt, event_t, event_type+6, event);
	}
	return Hangar::clbkPlaybackEvent(simt, event_t, event_type, event);
}

int TurnAroundHangar::InitActionAreas(UMMUCREWMANAGMENT *crew, int index)
{
	VECTOR3 areas[4]={TAAREA_SOUTH, TAAREA_NORTH, TAAREA_EAST, TAAREA_WEST};
	areaStart=index;
	for(int i=0;i<TADOORS;i++) doors[i].LinkActionArea(crew, index++, position+areas[i], 5);
	areaEnd=index-1;
	return index;
}

bool TurnAroundHangar::ActionAreaActivated(int action)
{
	if (action<areaStart || action>areaEnd) return false;
	int door=action-areaStart;
	double pos=doors[door].GetPosition();
	if (pos<=0) doors[door].Open();
	else doors[door].Close();
	return true;
}

bool TurnAroundHangar::CheckVincinity(VECTOR3 *pos, int index){return pos->x>position.x-45 && pos->x<position.x+45 && pos->z<position.z+40 && pos->z>position.z-40;}