#pragma once
#include "AscensionTowerPage.h"

class BasePage: public AscensionTowerPage
{
public:
	BasePage(AscensionTowerData *data);
	~BasePage(void);
	AscensionUltra *GetAscension();
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
private:
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	std::vector<AscensionTowerListPair> scanList;
	void Scan();
	void SetAscension(int index);
};
