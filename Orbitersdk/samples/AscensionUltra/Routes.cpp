#include "Routes.h"

Routes::~Routes(void)
{
	Clear();
}

void Routes::Clear()
{
	for (std::map<const char *, std::map<const char *, Route *>>::iterator i=links.begin();i!=links.end();i++)
	{
		for (std::map<const char *, Route *>::iterator j=i->second.begin();j!=i->second.end();j++) delete j->second;
		i->second.clear();
	}
	links.clear();
	for (std::map<const char *, std::map<const char *, Route *>>::iterator i=reverse.begin();i!=reverse.end();i++) i->second.clear();
	reverse.clear();
}

void Routes::Init(double size, double fallOff, double period, double duration, double propagate)
{
	this->size=size;
	this->fallOff=fallOff;
	this->period=period;
	this->duration=duration;
	this->propagate=propagate;
}

void Routes::Switch(bool on)
{
	for (std::map<const char *, std::map<const char *, Route *>>::iterator i=links.begin();i!=links.end();i++)
		for (std::map<const char *, Route *>::iterator j=i->second.begin();j!=i->second.end();j++)
			j->second->Path->Switch(j->second->On=on);
}

void Routes::Strobe(bool on)
{
	for (std::map<const char *, std::map<const char *, Route *>>::iterator i=links.begin();i!=links.end();i++)
		for (std::map<const char *, Route *>::iterator j=i->second.begin();j!=i->second.end();j++)
			Strobe(j->second, on);
}

void Routes::Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed)
{
	Route *link=new Route;
	link->Path=beaconPath;
	link->Inversed=inversed;
	link->Strobing=false;
	reverse[end][start]=links[start][end]=link;
	beaconPath->SetSize(size);
	beaconPath->SetFallOff(0);
	beaconPath->SetPeriod(0);
	beaconPath->SetDuration(duration);
	beaconPath->SetPropagate(propagate);
}

std::map<const char *, Route *> *Routes::GetEnds(char const*start)
{
	std::map<const char *, std::map<const char *, Route *>>::iterator starts=links.find(start);
	if (starts==links.end()) return NULL;
	return &starts->second;
}

std::map<const char *, Route *> *Routes::GetStarts(char const*end)
{
	std::map<const char *, std::map<const char *, Route *>>::iterator ends=reverse.find(end);
	if (ends==reverse.end()) return NULL;
	return &ends->second;
}

Route *Routes::GetLink(char const*start, char const*end)
{
	std::map<const char *, Route *> *map=GetEnds(start);
	if (map==NULL) return NULL;
	std::map<const char *, Route *>::iterator ends=map->find(end);
	if (ends==map->end()) return NULL;
	return ends->second;
}

void Routes::Strobe(Route *link, bool on)
{
	if (link==NULL) return;
	BeaconPath *bp=link->Path;
	if (on)
	{
		bp->SetFallOff(fallOff);
		bp->SetPeriod(link->Inversed?-period:period);
	}
	else
	{
		bp->SetFallOff(0);
		bp->SetPeriod(0);
	}
	link->Strobing=on;
}

void Routes::Strobe(const char *start, const char *end, bool on){Strobe(GetLink(start, end), on);}

bool Routes::Strobing(const char *start, const char *end)
{
	Route *link=GetLink(start, end);
	if (link==NULL) return false;
	return link->Strobing; 
}

void Routes::Switch(const char *start, const char *end, bool on)
{
	Route *link=GetLink(start, end);
	if (link==NULL) return;
	link->Path->Switch(link->On=on);
}

bool Routes::On(const char *start, const char *end)
{
	Route *link=GetLink(start, end);
	if (link==NULL) return false;
	return link->On; 
}

bool Routes::AnyStrobing(const char *point, bool isEnd)
{
	std::map<const char *, Route *> *points=isEnd?GetStarts(point):GetEnds(point);
	if (points==NULL) return false;
	for (std::map<const char *, Route *>::iterator i=points->begin();i!=points->end();i++) if (i->second->Strobing) return true;
	return false;
}

int Routes::GetPoints(bool isEnd, char *fromPoint)
{
	if (fromPoint==NULL) return isEnd?reverse.size():links.size();
	return isEnd?GetStarts(fromPoint)->size():GetEnds(fromPoint)->size();
}

char *Routes::GetPoint(int index, bool isEnd, char *fromPoint)
{
	int points=GetPoints(isEnd, fromPoint);
	if (index<0 || index>=points) return NULL;
	if (fromPoint==NULL)
	{
		if (index<points/2)
		{
			std::map<const char *, std::map<const char *, Route *>>::iterator i;
			for(i=isEnd?reverse.begin():links.begin();index>0;index--) i++;
			return (char *)i->first;
		}
		std::map<const char *, std::map<const char *, Route *>>::reverse_iterator i;
		for(i=isEnd?reverse.rbegin():links.rbegin();index<points-1;index++) i++;
		return (char *)i->first;
	}
	if (index<points/2)
	{
		std::map<const char *, Route *>::iterator i;
		for(i=isEnd?GetStarts(fromPoint)->begin():GetEnds(fromPoint)->begin();index>0;index--) i++;
		return (char *)i->first;
	}
	std::map<const char *, Route *>::reverse_iterator i;		
	for(i=isEnd?GetStarts(fromPoint)->rbegin():GetEnds(fromPoint)->rbegin();index<points-1;index++) i++;
	return (char *)i->first;		
}