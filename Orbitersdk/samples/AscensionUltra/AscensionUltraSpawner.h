#pragma once
#include "orbitersdk.h"

class AscensionUltraSpawner : public oapi::Module
{
public:
	AscensionUltraSpawner(HINSTANCE hDLL) : oapi::Module(hDLL) {};
	virtual void clbkSimulationStart (RenderMode mode);
};