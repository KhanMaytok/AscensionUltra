#pragma once
#include "orbitersdk.h"
#include "BeaconArray.h"

void RotateMesh(MESHHANDLE mesh, float angle, VECTOR3 v, VECTOR3 ref);
void ReadBeaconDefinition(BeaconArray *beacons, int count, const char *section, VECTOR3 position, VESSEL *owner);