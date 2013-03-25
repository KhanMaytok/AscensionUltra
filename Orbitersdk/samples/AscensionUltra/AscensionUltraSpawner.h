#pragma once
#include "orbitersdk.h"

class AscensionUltraSpawner : public oapi::Module
{
public:
	AscensionUltraSpawner(HINSTANCE hDLL);
	~AscensionUltraSpawner(){Save();}
	virtual void clbkSimulationStart (RenderMode mode);
private:
	void Save();
};