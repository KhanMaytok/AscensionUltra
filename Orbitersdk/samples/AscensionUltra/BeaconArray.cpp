#include "BeaconArray.h"

BeaconArray::BeaconArray(void)
{
	owner=NULL;
	spec=NULL;
	start=_V(0,0,0);
	end=_V(0,0,0);
	beacons=0;
	period=0;
	duration=0;
	propagate=0;
	restart=0;
}

BeaconArray::~BeaconArray(void)
{
	if (owner!=NULL) for(int i=0;i<beacons;i++) owner->DelBeacon(&spec[i]);
	delete [] spec;
}

void BeaconArray::Init(VESSEL *owner, VECTOR3 &start, VECTOR3 &end, VECTOR3 &col, int beacons)
{
}

void BeaconArray::SetLine(VECTOR3 &start, VECTOR3 &end)
{
}

void BeaconArray::SetColor(VECTOR3 &color)
{
}

void BeaconArray::SetBeacons(int beacons)
{
}

void BeaconArray::SetShape(DWORD shape)
{
}

void BeaconArray::SetSize(double size)
{
}

void BeaconArray::SetFallOff(double falloff)
{
}

void BeaconArray::GetLine(VECTOR3 &start, VECTOR3 &end) { start=this->start; end=this->end; }
void BeaconArray::GetColor(VECTOR3 &color) { color=(spec!=NULL && beacons>0)?*(spec[0].col):_V(0,0,0); }
int BeaconArray::GetBeacons() { return beacons; }
DWORD BeaconArray::GetShape() { return (spec!=NULL && beacons>0)?spec[0].shape:0; }
double BeaconArray::GetSize() { return (spec!=NULL && beacons>0)?spec[0].size:0; }
double BeaconArray::GetFallOff() { return (spec!=NULL && beacons>0)?spec[0].falloff:0; }

void BeaconArray::Switch(bool on)
{
}

bool BeaconArray::On() { return (spec!=NULL && beacons>0)?spec[0].active:false; }

void BeaconArray::SetPeriod(double period)
{
}

double BeaconArray::GetPeriod() { return period; }

void BeaconArray::SetDuration(double duration)
{
}

double BeaconArray::GetDuration() { return duration; }

void BeaconArray::SetPropagate(double propagate)
{
}

double BeaconArray::GetPropagate() { return propagate; }

void BeaconArray::SetRestart(double restart)
{
}

double BeaconArray::GetRestart() { return propagate; }
