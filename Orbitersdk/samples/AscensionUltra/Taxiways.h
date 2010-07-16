#pragma once
#include "orbitersdk.h"
#include <map>
#include "BeaconPath.h"

struct Taxiway
{
	BeaconPath *Path;
	bool Inversed;
	double OriginalPeriod;
};

class Taxiways
{
public:
	~Taxiways(void);
	void Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed);
	void Clear();
	virtual void Switch(const char *start, const char *end, bool on);
	void SwitchAll(const char *point, bool on, bool isEnd=false);
	virtual bool On(const char *start, const char *end);
	virtual bool AnyOn(const char *point, bool isEnd=false);
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);
	virtual char *GetPoint(int index, bool isEnd=false, char *fromPoint=NULL);
	
private:	
	std::map<const char *, std::map<const char *, Taxiway *>> links;
	std::map<const char *, std::map<const char *, Taxiway *>> reverse;
	std::map<const char *, Taxiway *> *GetEnds(char const*start);
	std::map<const char *, Taxiway *> *GetStarts(char const*end);
	Taxiway *Taxiways::GetLink(char const*start, char const*end);
	void Taxiways::Switch(Taxiway *link, bool on);
};
