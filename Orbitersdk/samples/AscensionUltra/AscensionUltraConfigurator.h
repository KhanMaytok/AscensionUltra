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
#include "TalkerEntry.h"
#include "sapi.h"
#include <map>
#include <queue>
#include <vector>
#define SPAWN		"BaseAutoSpawn"
#define RESET		"BaseFastReset"
#define SCNSAVE		"BaseScenarioSave"
#define RECSAVE		"BaseRecorderSave"
#define IDLEROTATE	"ATCRotationTime"
#define ATCSLOT		"ATCAnnotationSlot" //ATCAnnotationSlot0 = 0.1, 0.1, 0.9, 0.2
#define ORBITERCONFIG "Orbiter.cfg"
#define CLASSNAME	"AscensionUltra"
#define AUNAME		"Wideawake_Int(AU)"
#define LONGITUDE	(-14.405L*RAD)
#define LATITUDE	(-7.955L*RAD)
#define	HEADING		(220.0L*RAD)

class AscensionUltraConfig;
class RootConfig;

struct Annotation {
	double x1;
	double y1;
	double x2;
	double y2;
	NOTEHANDLE handle;
	double size;
	VECTOR3 color;
	char *text;
};

struct gParamsType {
	HINSTANCE hInst;
	AscensionUltraConfig *Item;
	RootConfig *Root;
	bool SCNSave, RecSave, Spawn, Reset;
	double IdleRotate;

	std::vector<Annotation> slots;

	/* Main message queue structure.
	   One queue per object in order to only talk out text for focussed vessels. Pusher must create new entries and
	   allocate heap wide-character strings as well as valid TalkerEntries per sender/receiver pair. Note that the
	   entries for both objects must be made!
	   Talker removes entries or mark TalkerEntries as invalid, so no text get talked twice.*/
	std::map<OBJHANDLE, std::queue<TalkerEntry *>> messages;
	                                             
	HANDLE event;
	HANDLE stopped;
	HANDLE thread;
	bool   active;
	CRITICAL_SECTION lock;
};