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

void Taxiways::Init(VESSEL* owner, const char *classname, VECTOR3 &position)
{
	ReadBeaconDefinition(beacons, classname, position, owner);
	ReadBeaconPaths(paths, beacons, classname, owner);
	ReadBeaconEndPoints(endPoints, classname);
	Routes::Init(
		beacons[0]->GetSize(),
		beacons[0]->GetFallOff(),
		beacons[0]->GetPeriod(),
		beacons[0]->GetDuration(),
		beacons[0]->GetPropagate());
	ReadBeaconRoutes(*this, paths, endPoints, classname);
	Switch(true);
	PriorityFinalize();
}
