#pragma once
#include "orbitersdk.h"

class AscensionUltraSpawner : public oapi::Module
{
public:
	AscensionUltraSpawner(HINSTANCE hDLL);
	~AscensionUltraSpawner();
	void clbkSimulationStart (RenderMode mode);
	void clbkSimulationEnd();
	void clbkFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus);
private:
	void Save();
};