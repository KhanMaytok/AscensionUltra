#pragma once
#include "orbitersdk.h"
#define SECTION		"Settings"
#define SPAWN		"auto-spawn"
#define RESET		"fast-reset"
#define SCNSAVE		"scenario-save"
#define RECSAVE		"recorder-save"
#define INIFILE		"Modules\\AscensionUltra.ini"
#define CLASSNAME	"AscensionUltra"
#define AUNAME		"Wideawake International"
#define LONGITUDE	(-14.405L*RAD)
#define LATITUDE	(-7.955L*RAD)
#define	HEADING		(220.0L*RAD)

class AscensionUltraConfig;
class RootConfig;

struct gParamsType{
	HINSTANCE hInst;
	AscensionUltraConfig *item;
	RootConfig *root;
};