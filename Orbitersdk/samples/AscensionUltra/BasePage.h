#pragma once
#include "AscensionTowerPage.h"

class BasePage::AscensionTowerPage
{
public:
	BasePage(AscensionTowerData *data);
	~BasePage(void);
	virtual void RenderPage();
	virtual int GetListSize();
	virtual AscensionTowerListPair GetListItem(int index);
	virtual AscensionTowerPageInstance Select(int index=-1);
	virtual char *GetButtonLabel (int bt);
	virtual int GetButtonMenu (MFDBUTTONMENU *mnu);
	virtual AscensionTowerPageInstance SetButton(int bt);
	virtual AscensionTowerPageInstance SetKey(DWORD key);
	virtual char *GetTitle();
	virtual char *GetSubtitle();
	AscensionUltra *GetAscension();
private:
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	AscensionUltra *ascension;
	std::vector<AscensionTowerListPair> scanList;
	void Scan();
	void SetAscension(int index);
};
