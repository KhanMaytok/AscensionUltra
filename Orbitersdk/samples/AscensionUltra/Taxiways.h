#pragma once
#include "Routes.h"
#include "Module.h"

class Taxiways :
	public Routes
{
public:
	Taxiways(void);
	~Taxiways(void);
	void Init(VESSEL* owner, const char *ini, const char *classname, VECTOR3 &position);
private:
	std::vector<char *> endPoints;
	std::vector<BeaconArray *> beacons;
	std::vector<BeaconPath *> paths;
};
