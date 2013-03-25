#pragma once
#include "orbitersdk.h"
#define SPAWN		"BaseAutoSpawn"
#define RESET		"BaseFastReset"
#define SCNSAVE		"BaseScenarioSave"
#define RECSAVE		"BaseRecorderSave"
#define ORBITERCONFIG "Orbiter.cfg"
#define CLASSNAME	"AscensionUltra"
#define AUNAME		"Wideawake_Int(AU)"
#define LONGITUDE	(-14.405L*RAD)
#define LATITUDE	(-7.955L*RAD)
#define	HEADING		(220.0L*RAD)

class AscensionUltraConfig;
class RootConfig;

struct gParamsType{
	HINSTANCE hInst;
	AscensionUltraConfig *Item;
	RootConfig *Root;
	bool SCNSave, RecSave, Spawn, Reset;
};