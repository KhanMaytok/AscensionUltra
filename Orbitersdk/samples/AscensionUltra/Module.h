#pragma once
#include "orbitersdk.h"
#include "BeaconArray.h"
#include "BeaconPath.h"
#include "Routes.h"
#include <vector>

void RotateMesh(MESHHANDLE mesh, float angle, VECTOR3 v, VECTOR3 ref);
void ReadBeaconDefinition(std::vector<BeaconArray *> &beacons, const char *section, VECTOR3 position, VESSEL *owner);
void OverwriteBeaconParamsDefinition(std::vector<BeaconArray *> &beacons, const char *section);
void ReadBeaconPaths(std::vector<BeaconPath *> &paths, std::vector<BeaconArray *> &beacons, const char *section, VESSEL *owner);
void ReadBeaconEndPoints(std::vector<char *> &endPoints, const char *section);
void ReadBeaconRoutes(Routes &routes, std::vector<BeaconPath *> &paths, std::vector<char *> &endPoints, const char *section);