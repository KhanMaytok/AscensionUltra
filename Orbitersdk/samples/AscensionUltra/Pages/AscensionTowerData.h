#pragma once
#include <vector>
#include "..\AscensionUltra.h"
#include "..\AscensionTower.h"
#include "AscensionTowerPage.h"
#include "orbitersdk.h"

class AscensionTowerPage;

class AscensionTowerData
{
public:
	AscensionTowerData();
	void Update();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	bool SetButton(int bt);
	bool SetKey(DWORD key);
	AscensionTower *GetMFD();
	void SetMFD(AscensionTower *mfd);
	AscensionUltra *GetAscension();
	AscensionTowerPage *GetPage(enum AscensionTowerPageInstance page);
	
private:
	AscensionTower *mfd;
	enum AscensionTowerPageInstance state;
	std::map<AscensionTowerPageInstance, AscensionTowerPage *> pages;
	bool StateChange(AscensionTowerPageInstance newstate);
};
