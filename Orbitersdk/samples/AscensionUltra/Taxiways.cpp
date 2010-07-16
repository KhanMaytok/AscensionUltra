#include "Taxiways.h"

Taxiways::~Taxiways(void)
{
	Clear();
}

void Taxiways::Clear()
{
	for (std::map<const char *, std::map<const char *, Taxiway *>>::iterator i=links.begin();i!=links.end();i++)
	{
		for (std::map<const char *, Taxiway *>::iterator j=i->second.begin();j!=i->second.end();j++) delete j->second;
		i->second.clear();
	}
	links.clear();
	for (std::map<const char *, std::map<const char *, Taxiway *>>::iterator i=reverse.begin();i!=reverse.end();i++) i->second.clear();
	reverse.clear();
}

void Taxiways::Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed)
{
	Taxiway *link=new Taxiway;
	link->Path=beaconPath;
	link->Inversed=inversed;
	link->OriginalPeriod=beaconPath->GetPeriod();
	reverse[end][start]=links[start][end]=link;
}

std::map<const char *, Taxiway *> *Taxiways::GetEnds(char const*start)
{
	std::map<const char *, std::map<const char *, Taxiway *>>::iterator starts=links.find(start);
	if (starts==links.end()) return NULL;
	return &starts->second;
}

std::map<const char *, Taxiway *> *Taxiways::GetStarts(char const*end)
{
	std::map<const char *, std::map<const char *, Taxiway *>>::iterator ends=reverse.find(end);
	if (ends==reverse.end()) return NULL;
	return &ends->second;
}

Taxiway *Taxiways::GetLink(char const*start, char const*end)
{
	std::map<const char *, Taxiway *> *map=GetEnds(start);
	if (map==NULL) return NULL;
	std::map<const char *, Taxiway *>::iterator ends=map->find(end);
	if (ends==map->end()) return NULL;
	return ends->second;
}

void Taxiways::Switch(Taxiway *link, bool on)
{
	if (link==NULL) return;
	BeaconPath *bp=link->Path;
	if (on) bp->SetPeriod((link->Inversed?-1:1) * link->OriginalPeriod);
	bp->Switch(on);
}

void Taxiways::Switch(const char *start, const char *end, bool on){Switch(GetLink(start, end), on);}

bool Taxiways::On(const char *start, const char *end)
{
	Taxiway *link=GetLink(start, end);
	if (link==NULL) return false;
	return link->Path->On(); 
}

void Taxiways::SwitchAll(const char *point, bool on, bool isEnd)
{
	std::map<const char *, Taxiway *> *points=isEnd?GetStarts(point):GetEnds(point);
	if (points==NULL) return;
	for (std::map<const char *, Taxiway *>::iterator i=points->begin();i!=points->end();i++) Switch(i->second, on);
}
	
bool Taxiways::AnyOn(const char *point, bool isEnd)
{
	std::map<const char *, Taxiway *> *points=isEnd?GetStarts(point):GetEnds(point);
	if (points==NULL) return false;
	for (std::map<const char *, Taxiway *>::iterator i=points->begin();i!=points->end();i++) if (i->second->Path->On()) return true;
	return false;
}

int Taxiways::GetPoints(bool isEnd, char *fromPoint)
{
	if (fromPoint==NULL) return isEnd?reverse.size():links.size();
	return isEnd?GetStarts(fromPoint)->size():GetEnds(fromPoint)->size();
}

char *Taxiways::GetPoint(int index, bool isEnd, char *fromPoint)
{
	int points=GetPoints(isEnd, fromPoint);
	if (index<0 || index>=points) return NULL;
	if (fromPoint==NULL)
	{
		if (index<points/2)
		{
			std::map<const char *, std::map<const char *, Taxiway *>>::iterator i;
			for(i=isEnd?reverse.begin():links.begin();index>0;index--) i++;
			return (char *)i->first;
		}
		std::map<const char *, std::map<const char *, Taxiway *>>::reverse_iterator i;
		for(i=isEnd?reverse.rbegin():links.rbegin();index<points-1;index++) i++;
		return (char *)i->first;
	}
	if (index<points/2)
	{
		std::map<const char *, Taxiway *>::iterator i;
		for(i=isEnd?GetStarts(fromPoint)->begin():GetEnds(fromPoint)->begin();index>0;index--) i++;
		return (char *)i->first;
	}
	std::map<const char *, Taxiway *>::reverse_iterator i;		
	for(i=isEnd?GetStarts(fromPoint)->rbegin():GetEnds(fromPoint)->rbegin();index<points-1;index++) i++;
	return (char *)i->first;		
}