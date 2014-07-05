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

#include "BeaconArray.h"

BeaconArray::BeaconArray(void)
{
	owner=NULL;
	spec=NULL;
	beacons=0;
	period=0;
	duration=0;
	propagate=0;
	offset=0;
	offsetPropagation=0;
	color=_V(0,1,0);
	start=end=_V(0,0,0);
}

BeaconArray::~BeaconArray(void)
{
	Clear();
}

void BeaconArray::Clear()
{
	if (owner!=NULL) for(int i=0;i<beacons;i++) owner->DelBeacon(&spec[i]);
	for(int i=0;i<beacons;i++) delete spec[i].pos;
	delete [] spec;
}

void BeaconArray::Init(VESSEL *owner, VECTOR3 &start, VECTOR3 &end, VECTOR3 &color, int beacons)
{
	Clear();
	this->owner=owner;
	this->beacons=beacons;	
	spec=new BEACONLIGHTSPEC[beacons];
	for(int i=0;i<beacons;i++)
	{
		spec[i].active=false;
		spec[i].duration=0;
		spec[i].falloff=0;
		spec[i].period=0;
		spec[i].shape=BEACONSHAPE_COMPACT;
		spec[i].size=1;
		spec[i].tofs=0;
		spec[i].pos=new VECTOR3(_V(0,0,0));
		spec[i].col=&this->color;
		owner->AddBeacon(&(spec[i]));
	}
	SetLine(start, end);
	SetColor(color);
	SetShape(BEACONSHAPE_STAR);
	SetSize(1.0);
	SetFallOff(0.5);
	SetPeriod(0.0);
	SetDuration(0.0);
	SetPropagate(0.0);
}

void BeaconArray::SetLine(VECTOR3 &start, VECTOR3 &end)
{
	this->start=start;
	this->end=end;
	SetPositions(start, end);
}

void BeaconArray::SetPositions(VECTOR3 &start, VECTOR3 &end)
{
	if (beacons<1) return;
	*(spec[0].pos)=start;
	if (beacons==1)	return;
	VECTOR3 step=(end-start)/(beacons-1);
	for (int i=1;i<beacons;i++)	*(spec[i].pos)=start+step*i;
}

void BeaconArray::SetColor(VECTOR3 &color){ this->color=color; } //NOTE: Beacons already use a pointer to this member here!

void BeaconArray::SetBeacons(int beacons)
{
	if (this->beacons==beacons || beacons<1) return;
	VECTOR3 start,end;
	GetLine(start,end);
	DWORD shape=GetShape();
	double size=GetSize();
	double falloff=GetFallOff();
	double period=GetPeriod();
	double duration=GetDuration();
	double propagate=GetPropagate();
	Clear();
	this->beacons=beacons;	
	spec=new BEACONLIGHTSPEC[beacons];
	for(int i=0;i<beacons;i++)
	{
		spec[i].active=false;
		spec[i].pos=new VECTOR3;
		spec[i].col=&this->color;
		owner->AddBeacon(&spec[i]);
	}
	SetLine(start, end);
	SetShape(shape);
	SetSize(size);
	SetFallOff(falloff);
	CalculateStrobe();
}

void BeaconArray::SetShape(DWORD shape){ for (int i=0;i<beacons;i++) spec[i].shape=shape; }

void BeaconArray::SetSize(double size){ for (int i=0;i<beacons;i++) spec[i].size=size; }

void BeaconArray::SetFallOff(double falloff){ for (int i=0;i<beacons;i++) spec[i].falloff=falloff; }

void BeaconArray::GetLine(VECTOR3 &start, VECTOR3 &end)
{
	start=this->start;
	end=this->end;	
}
void BeaconArray::GetColor(VECTOR3 &color) { color=(spec!=NULL && beacons>0)?*(spec[0].col):_V(0,0,0); }
int BeaconArray::GetBeacons() { return beacons; }
DWORD BeaconArray::GetShape() { return (spec!=NULL && beacons>0)?spec[0].shape:0; }
double BeaconArray::GetSize() { return (spec!=NULL && beacons>0)?spec[0].size:0; }
double BeaconArray::GetFallOff() { return (spec!=NULL && beacons>0)?spec[0].falloff:0; }

void BeaconArray::Switch(bool on){ for (int i=0;i<beacons;i++) spec[i].active=on; }

bool BeaconArray::On() { return (spec!=NULL && beacons>0)?spec[0].active:false; }

void BeaconArray::SetPeriod(double period)
{
	this->period=period;
	CalculateStrobe();
}

double BeaconArray::GetPeriod() { return period; }

void BeaconArray::SetDuration(double duration)
{
	this->duration=duration;
	CalculateStrobe();

}

double BeaconArray::GetDuration() { return duration; }

void BeaconArray::SetPropagate(double propagate)
{
	this->propagate=propagate;
	CalculateStrobe();
}

double BeaconArray::GetPropagate() { return propagate; }

void BeaconArray::SetOffset(double offset)
{
	this->offset=offset;
	CalculateStrobe();
}

double BeaconArray::GetOffset() { return offset; }
double BeaconArray::GetOffsetPropagation() { return offsetPropagation; }

void BeaconArray::CalculateStrobe()
{
	double tofs=offset;
	int i=period<0?beacons-1:0;
	int end=period<0?-1:beacons;
	int step=period<0?-1:1;
	for (;i!=end;i+=step)	
	{
		spec[i].period=abs(period);
		spec[i].duration=duration<0?period+duration:duration;
		spec[i].tofs=tofs;
		tofs+=spec[i].duration+propagate;
	}
	offsetPropagation=tofs;
}