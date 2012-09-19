#pragma once
#include "AscensionUltraTowerPage.h"

class BasePage::AscensionTowerPage
{
public:
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
};
