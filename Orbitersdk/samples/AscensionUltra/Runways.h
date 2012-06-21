#pragma once
#include "Routes.h"

class Runways :
	public Routes
{
public:
	Runways(void);
	~Runways(void);
	void Init(VESSEL* owner, const char *classname, VECTOR3 &position);
private:
	std::vector<char *> endPoints;
	std::vector<BeaconArray *> beacons;
	std::vector<BeaconPath *> paths;
};
