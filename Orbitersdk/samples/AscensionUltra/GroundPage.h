#pragma once
#include "AscensionTowerPage.h"

class GroundPage: public AscensionTowerPage
{
public:
	GroundPage(AscensionTowerData *data):AscensionTowerPage(data){}
protected:
	char *LabelRenderer (int bt);
	int MenuRenderer (MFDBUTTONMENU *mnu);
	AscensionTowerPageInstance ButtonHandler(int bt);
	AscensionTowerPageInstance KeyHandler(DWORD key);
	char *GetTitle();
	char *GetSubtitle();
	int GetListSize();
	AscensionTowerListPair GetListItem(int index);
	AscensionTowerPageInstance Select(int index=-1);
};
