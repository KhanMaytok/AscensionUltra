#pragma once
#include "orbitersdk.h"
#include "BeaconArray.h"
#include "BeaconPath.h"
#include "Routes.h"
#include "Group.h"
#include "Checklist.h"
#include <vector>

namespace BaseVessel
{
	namespace EventHandler
	{
		enum SourceType
		{
			Checklist,
		};

		struct Arguments
		{
			int Event;
			enum SourceType SourceType;
			void *Source;
		};
	}

	namespace Talker
	{
		struct Voice
		{
			LPCWSTR Definition;
			std::vector<LPCWSTR> Acknowledgement;
			double FontSize;
			VECTOR3 Color;
		};
	}
}

void RotateMesh(MESHHANDLE mesh, float angle, VECTOR3 v, VECTOR3 ref);
void ReadBeaconDefinition(std::vector<BeaconArray *> &beacons, const char *ini, const char *section, VECTOR3 position, VESSEL *owner);
void OverwriteBeaconParamsDefinition(std::vector<BeaconArray *> &beacons, const char *ini, const char *section);
void ReadBeaconPaths(std::vector<BeaconPath *> &paths, std::vector<BeaconArray *> &beacons, const char *ini, const char *section, VESSEL *owner);
void ReadBeaconEndPoints(std::vector<char *> &endPoints, const char *ini, const char *section);
void ReadBeaconRoutes(Routes &routes, std::vector<BeaconPath *> &paths, std::vector<char *> &endPoints, const char *ini, const char *section);
void ReadBeaconGroups(Group &groups, std::vector<BeaconArray *> &beacons, const char *ini, const char *section);
void ReadATCChecklist(Checklist *checklist, const char *ini, const char *section);
void ReadATCParameters(std::vector<LPCWSTR> &talkerSection, const char *ini, const char *section);