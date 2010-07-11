#include "BeaconPath.h"

BeaconPath::~BeaconPath(void)
{
	Clear();
}

void BeaconPath::Clear()
{
	for(std::list<BeaconArray *>::iterator i=selfCreated.begin(); i!=selfCreated.end(); i++) delete *i;
	arrays.clear();
	selfCreated.clear();
}

void BeaconPath::Init(VESSEL *owner, VECTOR3 *arrays, VECTOR3 &color, int *beacons, int length)
{
	Clear();
	offset=0;
	on=false;
	for(int i=0;i<length;i++)
	{
		BeaconArray *ba=new BeaconArray();
		ba->Init(owner, arrays[0], arrays[1], color, beacons[i]);
		arrays+=2;
		selfCreated.push_back(ba);
		Add(ba);
	}
}

void BeaconPath::Add(BeaconArray *beaconArray){ arrays.push_back(beaconArray); }

void BeaconPath::SetColor(VECTOR3 &color){ for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++) (*i)->SetColor(color); }
void BeaconPath::SetShape(DWORD shape){ for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++) (*i)->SetShape(shape); }
void BeaconPath::SetSize(double size){ for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++) (*i)->SetSize(size); }
void BeaconPath::SetFallOff(double falloff){ for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++) (*i)->SetFallOff(falloff); }
void BeaconPath::Switch(bool on)
{
	for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++) (*i)->Switch(on);
	this->on=on;
}

void BeaconPath::GetColor(VECTOR3 &color) { if (arrays.empty()) color=_V(0,0,0); else arrays.front()->GetColor(color); }
DWORD BeaconPath::GetShape() { return arrays.empty()?0:arrays.front()->GetShape(); }
double BeaconPath::GetSize() { return arrays.empty()?0:arrays.front()->GetSize(); }
double BeaconPath::GetFallOff() { return arrays.empty()?0:arrays.front()->GetFallOff(); }
bool BeaconPath::On() { return on; }

void BeaconPath::SetPeriod(double period){ CalculateStrobe(period, GetDuration(), GetPropagate()); }
double BeaconPath::GetPeriod() { return arrays.empty()?0:arrays.front()->GetPeriod(); }
void BeaconPath::SetDuration(double duration){ CalculateStrobe(GetPeriod(), duration, GetPropagate()); }
double BeaconPath::GetDuration() { return arrays.empty()?0:arrays.front()->GetDuration(); }
void BeaconPath::SetPropagate(double propagate){ CalculateStrobe(GetPeriod(), GetDuration(), propagate); }
double BeaconPath::GetPropagate() { return arrays.empty()?0:arrays.front()->GetPropagate(); }

void BeaconPath::SetOffset(double offset)
{
	this->offset=offset;
	CalculateStrobe(GetPeriod(), GetDuration(), GetPropagate());
}
double BeaconPath::GetOffset() { return offset; }

void BeaconPath::CalculateStrobe(double period, double duration, double propagate)
{
	double offset=this->offset;
	if (period<0)
	{
		for(std::list<BeaconArray *>::reverse_iterator i=arrays.rbegin(); i!=arrays.rend(); i++)
		{
			(*i)->SetPeriod(period);
			(*i)->SetDuration(duration);
			(*i)->SetPropagate(propagate);
			(*i)->SetOffset(offset);
			offset=(*i)->GetOffsetPropagation();		
		}	
	}
	else
	{
		for(std::list<BeaconArray *>::iterator i=arrays.begin(); i!=arrays.end(); i++)
		{
			(*i)->SetPeriod(period);
			(*i)->SetDuration(duration);
			(*i)->SetPropagate(propagate);
			(*i)->SetOffset(offset);
			offset=(*i)->GetOffsetPropagation();		
		}
	}
}