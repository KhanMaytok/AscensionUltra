#pragma once
#include "AscensionTowerPage.h"

class MainPage: public AscensionTowerPage
{
public:
	MainPage(AscensionTowerData *data):AscensionTowerPage(data){}
protected:
	char *LabelRenderer (int bt);
	int MenuRenderer (MFDBUTTONMENU *mnu);
	AscensionTowerPageInstance KeyHandler(DWORD key);
	char *GetTitle(){return "Ascension Tower";}
	char *GetSubtitle(){return "Select request";}
	int GetListSize(){return 5;}
	AscensionTowerListPair GetListItem(int index){static AscensionTowerListPair menu[5]=
	{	{0," Ground Operations"},
		{1," Air Traffic Control"},
		{2," Control Rooms"},
		{3, " Roster"},
		{4, " Reset to Defaults"}};return menu[index];}
	AscensionTowerPageInstance Select(int index=-1);
};
