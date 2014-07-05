// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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
			double Size;
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
void ReadGroupGroups(Group &groups, const char *ini, const char *section);
void ReadATCChecklist(Checklist *checklist, const char *ini, const char *section);
void ReadATCParameters(BaseVessel::Talker::Voice &voice, const char *ini, const char *section);