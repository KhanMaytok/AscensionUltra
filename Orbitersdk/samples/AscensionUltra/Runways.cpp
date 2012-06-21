#include "Runways.h"
#include "Module.h"

Runways::Runways(void)
{
}

Runways::~Runways(void)
{
	for(std::vector<BeaconArray *>::iterator i=beacons.begin();i!=beacons.end();i++) delete *i;
	for(std::vector<BeaconPath *>::iterator i=paths.begin();i!=paths.end();i++) delete *i;
	for(std::vector<char *>::iterator i=endPoints.begin();i!=endPoints.end();i++) delete [] *i;
}

void Runways::Init(VESSEL* owner, const char *classname, VECTOR3 &position)
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
	Switch(false);
	Switch(endPoints[0],endPoints[6],true); //Runway 13L static
	Switch(endPoints[1],endPoints[6],true); //Runway 13R static
	Switch(endPoints[2],endPoints[6],true); //Runway 31L static
	Switch(endPoints[3],endPoints[6],true); //Runway 31R static
	Switch(endPoints[4],endPoints[6],true); //Runway 13L/31R static
	Switch(endPoints[5],endPoints[6],true);//Runway 31L/13R static
	OverwriteBeaconParamsDefinition(beacons, classname);
	PriorityFinalize();
}