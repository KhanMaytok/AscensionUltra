#pragma once
#include "orbitersdk.h"
#include <map>
#include "BeaconPath.h"

struct BeaconLink
{
	BeaconPath *Path;
	bool Inversed;
	double OriginalPeriod;
};

class BeaconLinks
{
public:
	~BeaconLinks(void);
	void Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed);
	void Clear();
	virtual void Switch(const char *start, const char *end, bool on);
	void SwitchAll(const char *point, bool on, bool isEnd=false);
	virtual bool On(const char *start, const char *end);
	virtual bool AnyOn(const char *point, bool isEnd=false);
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);
	virtual char *GetPoint(int index, bool isEnd=false, char *fromPoint=NULL);
	
private:	
	std::map<const char *, std::map<const char *, BeaconLink *>> links;
	std::map<const char *, std::map<const char *, BeaconLink *>> reverse;
	std::map<const char *, BeaconLink *> *GetEnds(char const*start);
	std::map<const char *, BeaconLink *> *GetStarts(char const*end);
	BeaconLink *BeaconLinks::GetLink(char const*start, char const*end);
	void BeaconLinks::Switch(BeaconLink *link, bool on);
};
