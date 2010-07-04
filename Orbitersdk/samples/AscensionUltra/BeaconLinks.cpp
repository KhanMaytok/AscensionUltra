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

void BeaconLinks::SwitchAll(const char *point, bool isEnd, bool on)
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