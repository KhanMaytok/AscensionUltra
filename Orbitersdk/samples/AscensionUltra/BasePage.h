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
	AscensionTowerPageInstance KeyHandler(DWORD key);
	char *GetTitle(){return "Ascension Tower";}
	char *GetSubtitle(){return scanList.size()>0?"Select base":"";}
	int GetListSize(){return scanList.size();}
	AscensionTowerListPair GetListItem(int index){return scanList[index];}
	AscensionTowerPageInstance Select(int index=-1);
private:
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	std::vector<AscensionTowerListPair> scanList;
	void Scan();
	void SetAscension(int index);
};
