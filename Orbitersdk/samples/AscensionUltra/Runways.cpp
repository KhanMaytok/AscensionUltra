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

	//Detect and activate static beacons
	int k=endPoints.size();
	for(int i=0;i<k;i++)
		if (strcmp(endPoints[i], "Static")==0)
		{
			staticIndex=i;
			break;
		}
	Routes::Switch(false);
	for(int i=0;i<staticIndex;i++)
		Routes::Switch(endPoints[i],endPoints[staticIndex],true);
	k=GetPoints(false, endPoints[staticIndex]);
	for(int i=0;i<k;i++)
		Routes::Switch(endPoints[staticIndex], Routes::GetPoint(i, false, endPoints[staticIndex]), true);
	
	OverwriteBeaconParamsDefinition(beacons, classname);
	PriorityFinalize();
}

void Runways::Activate(int index)
{
	char *point=GetPoint(index);
	bool on=!On(point);
	int k=endPoints.size();
	for(int i=0;i<staticIndex;i++)
		for(int j=staticIndex+1;j<k;j++)
			Switch(endPoints[i], endPoints[j], endPoints[i]==point?on:false);
}

bool Runways::On(const char *point, bool isEnd)
{
	return Routes::On(point, endPoints[staticIndex+1]);
}

int Runways::GetPoints(bool isEnd, char *fromPoint)
{
	return Routes::GetPoints(isEnd, fromPoint)-((!isEnd && fromPoint==NULL)?1:0);
}