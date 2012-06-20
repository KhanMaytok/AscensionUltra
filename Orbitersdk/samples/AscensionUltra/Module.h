#pragma once
#include "orbitersdk.h"
#include "BeaconArray.h"
#include "BeaconPath.h"
#include "Routes.h"
#include <vector>

void RotateMesh(MESHHANDLE mesh, float angle, VECTOR3 v, VECTOR3 ref);
void ReadBeaconDefinition(BeaconArray *beacons, int count, const char *section, VECTOR3 position, VESSEL *owner);
void OverwriteBeaconParamsDefinition(BeaconArray *beacons, int count, const char *section);
void ReadBeaconPaths(BeaconPath *paths, int count, BeaconArray *beacons, const char *section, VESSEL *owner);
void ReadBeaconEndPoints(std::vector<char *> *endPoints, const char *section);
void ReadBeaconRoutes(Routes *routes, BeaconPath *paths, std::vector<char *> *endPoints, const char *section);