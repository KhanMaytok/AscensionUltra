#include "BeaconLinks.h"

BeaconLinks::~BeaconLinks(void)
{
	Clear();
}

void BeaconLinks::Clear()
{
	for (std::map<const char *, std::map<const char *, BeaconLink *>>::iterator i=links.begin();i!=links.end();i++)
	{
		for (std::map<const char *, BeaconLink *>::iterator j=i->second.begin();j!=i->second.end();j++) delete j->second;
		i->second.clear();
	}
	links.clear();
	for (std::map<const char *, std::map<const char *, BeaconLink *>>::iterator i=reverse.begin();i!=reverse.end();i++) i->second.clear();
	reverse.clear();
}

void BeaconLinks::Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed)
{
	BeaconLink *link=new BeaconLink;
	link->Path=beaconPath;
	link->Inversed=inversed;
	link->OriginalPeriod=beaconPath->GetPeriod();
	reverse[end][start]=links[start][end]=link;
}

std::map<const char *, BeaconLink *> *BeaconLinks::GetEnds(char const*start)
{
	std::map<const char *, std::map<const char *, BeaconLink *>>::iterator starts=links.find(start);
	if (starts==links.end()) return NULL;
	return &starts->second;
}

std::map<const char *, BeaconLink *> *BeaconLinks::GetStarts(char const*end)
{
	std::map<const char *, std::map<const char *, BeaconLink *>>::iterator ends=reverse.find(end);
	if (ends==reverse.end()) return NULL;
	return &ends->second;
}

BeaconLink *BeaconLinks::GetLink(char const*start, char const*end)
{
	std::map<const char *, BeaconLink *> *map=GetEnds(start);
	if (map==NULL) return NULL;
	std::map<const char *, BeaconLink *>::iterator ends=map->find(end);
	if (ends==map->end()) return NULL;
	return ends->second;
}

void BeaconLinks::Switch(BeaconLink *link, bool on)
{
	if (link==NULL) return;
	BeaconPath *bp=link->Path;
	if (on) bp->SetPeriod((link->Inversed?-1:1) * link->OriginalPeriod);
	bp->Switch(on);
}

void BeaconLinks::Switch(const char *start, const char *end, bool on){Switch(GetLink(start, end), on);}

bool BeaconLinks::On(const char *start, const char *end)
{
	BeaconLink *link=GetLink(start, end);
	if (link==NULL) return false;
	return link->Path->On(); 
}

void BeaconLinks::SwitchAll(const char *point, bool on, bool isEnd)
{
	std::map<const char *, BeaconLink *> *points=isEnd?GetStarts(point):GetEnds(point);
	if (points==NULL) return;
	for (std::map<const char *, BeaconLink *>::iterator i=points->begin();i!=points->end();i++) Switch(i->second, on);
}
	
bool BeaconLinks::AnyOn(const char *point, bool isEnd)
{
	std::map<const char *, BeaconLink *> *points=isEnd?GetStarts(point):GetEnds(point);
	if (points==NULL) return false;
	for (std::map<const char *, BeaconLink *>::iterator i=points->begin();i!=points->end();i++) if (i->second->Path->On()) return true;
	return false;
}

int BeaconLinks::GetPoints(bool isEnd, char *fromPoint)
{
	if (fromPoint==NULL) return isEnd?reverse.size():links.size();
	return isEnd?GetStarts(fromPoint)->size():GetEnds(fromPoint)->size();
}

char *BeaconLinks::GetPoint(int index, bool isEnd, char *fromPoint)
{
	int points=GetPoints(isEnd, fromPoint);
	if (index<0 || index>=points) return NULL;
	if (fromPoint==NULL)
	{
		if (index<points/2)
		{
			std::map<const char *, std::map<const char *, BeaconLink *>>::iterator i;
			for(i=isEnd?reverse.begin():links.begin();index>0;index--) i++;
			return (char *)i->first;
		}
		std::map<const char *, std::map<const char *, BeaconLink *>>::reverse_iterator i;
		for(i=isEnd?reverse.rbegin():links.rbegin();index<points-1;index++) i++;
		return (char *)i->first;
	}
	if (index<points/2)
	{
		std::map<const char *, BeaconLink *>::iterator i;
		for(i=isEnd?GetStarts(fromPoint)->begin():GetEnds(fromPoint)->begin();index>0;index--) i++;
		return (char *)i->first;
	}
	std::map<const char *, BeaconLink *>::reverse_iterator i;		
	for(i=isEnd?GetStarts(fromPoint)->rbegin():GetEnds(fromPoint)->rbegin();index<points-1;index++) i++;
	return (char *)i->first;		
}