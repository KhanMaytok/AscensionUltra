#include "Taxiways.h"

Taxiways::Taxiways(void)
{	
}

Taxiways::~Taxiways(void)
{
	for(std::vector<BeaconArray *>::iterator i=beacons.begin();i!=beacons.end();i++) delete *i;
	for(std::vector<BeaconPath *>::iterator i=paths.begin();i!=paths.end();i++) delete *i;
	for(std::vector<char *>::iterator i=endPoints.begin();i!=endPoints.end();i++) delete [] *i;
}

void Taxiways::Init(VESSEL* owner, const char *ini, const char *classname, VECTOR3 &position)
{
	ReadBeaconDefinition(beacons, ini, classname, position, owner);
	ReadBeaconPaths(paths, beacons, ini, classname, owner);
	ReadBeaconEndPoints(endPoints, ini, classname);
	Routes::Init(
		beacons[0]->GetSize(),
		beacons[0]->GetFallOff(),
		beacons[0]->GetPeriod(),
		beacons[0]->GetDuration(),
		beacons[0]->GetPropagate());
	ReadBeaconRoutes(*this, paths, endPoints, ini, classname);
	Switch(true);
	PriorityFinalize();
}
