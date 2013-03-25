#pragma once
#include "Routes.h"

class Runways :
	public Routes
{
public:
	Runways(void);
	~Runways(void);
	void Init(VESSEL* owner, const char *ini, const char *classname, VECTOR3 &position);
	virtual void Activate(int index);
	virtual bool On(const char *point, bool isEnd=false);
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);
private:
	std::vector<char *> endPoints;
	std::vector<BeaconArray *> beacons;
	std::vector<BeaconPath *> paths;
	int staticIndex;
};
