#pragma once
#include "orbitersdk.h"
#define SECTION		"Settings"
#define SPAWN		"auto-spawn"
#define RESET		"fast-reset"
#define SCNSAVE		"scenario-save"
#define RECSAVE		"recorder-save"
#define INIFILE		"Modules\\AscensionUltra.ini"

class AscensionUltraConfig;
class RootConfig;

struct gParamsType{
	HINSTANCE hInst;
	AscensionUltraConfig *item;
	RootConfig *root;
};