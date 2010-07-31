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
	for(int i=0;i<length;i++)
	{
		BeaconArray *ba=new BeaconArray();
		ba->Init(owner, arrays[0], arrays[1], color, beacons[i]);
		arrays+=2;
		selfCreated.push_back(ba);
		Add(ba);
	}
}

void BeaconPath::Add(BeaconArray *beaconArray, bool inverse)
{
	Item item={inverse, beaconArray};
	arrays.push_back(item);
}

void BeaconPath::SetColor(VECTOR3 &color){ for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++) i->beaconArray->SetColor(color); }
void BeaconPath::SetShape(DWORD shape){ for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++) i->beaconArray->SetShape(shape); }
void BeaconPath::SetSize(double size){ for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++) i->beaconArray->SetSize(size); }
void BeaconPath::SetFallOff(double falloff){ for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++) i->beaconArray->SetFallOff(falloff); }
void BeaconPath::Switch(bool on){ for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++) i->beaconArray->Switch(on); }

void BeaconPath::GetColor(VECTOR3 &color) { if (arrays.empty()) color=_V(0,0,0); else arrays.front().beaconArray->GetColor(color); }
DWORD BeaconPath::GetShape() { return arrays.empty()?0:arrays.front().beaconArray->GetShape(); }
double BeaconPath::GetSize() { return arrays.empty()?0:arrays.front().beaconArray->GetSize(); }
double BeaconPath::GetFallOff() { return arrays.empty()?0:arrays.front().beaconArray->GetFallOff(); }
bool BeaconPath::On() { return arrays.empty()?0:arrays.front().beaconArray->On(); }
void BeaconPath::SetPeriod(double period){ CalculateStrobe(period, GetDuration(), GetPropagate()); }
double BeaconPath::GetPeriod() { return arrays.empty()?0:arrays.front().beaconArray->GetPeriod(); }
void BeaconPath::SetDuration(double duration){ CalculateStrobe(GetPeriod(), duration, GetPropagate()); }
double BeaconPath::GetDuration() { return arrays.empty()?0:arrays.front().beaconArray->GetDuration(); }
void BeaconPath::SetPropagate(double propagate){ CalculateStrobe(GetPeriod(), GetDuration(), propagate); }
double BeaconPath::GetPropagate() { return arrays.empty()?0:arrays.front().beaconArray->GetPropagate(); }

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
		for(std::list<Item>::reverse_iterator i=arrays.rbegin(); i!=arrays.rend(); i++)
		{
			BeaconArray *b=i->beaconArray;
			b->SetPeriod(period*(i->inversed?-1:1));
			b->SetDuration(duration);
			b->SetPropagate(propagate);
			b->SetOffset(offset);
			offset=b->GetOffsetPropagation();		
		}	
	}
	else
	{
		for(std::list<Item>::iterator i=arrays.begin(); i!=arrays.end(); i++)
		{
			BeaconArray *b=i->beaconArray;
			b->SetPeriod(period*(i->inversed?-1:1));
			b->SetDuration(duration);
			b->SetPropagate(propagate);
			b->SetOffset(offset);
			offset=b->GetOffsetPropagation();		
		}
	}
}