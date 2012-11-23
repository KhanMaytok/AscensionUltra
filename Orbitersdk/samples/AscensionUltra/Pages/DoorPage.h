#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class DoorPage: public AscensionTowerPage
{

public:

	DoorPage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return "RES";
			default: return AscensionTowerPage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[3] = 
		{
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Reset to default", "values", 'R'}
		};
		
		mnu[0].line1="Select command";
		for(int i=0;i<3;i++) mnu[6+i]=menu[i];	
		return AscensionTowerPage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		switch(key)
		{	
		case OAPI_KEY_H:
			return MainMenu;
		case OAPI_KEY_B:
			return DoorSelection;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle()
	{
		static char subtitle[57];
		sprintf(subtitle, "%s -> %s",
			(char *)((void **)dataRoot)[0],
			((Door *)((void **)dataRoot)[1])->GetName());
		return subtitle;
	}

	int GetListSize(){return 3;}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair doorMenu[3]={{0," Open"},{1," Close"},{2," Stop"}};	
		return doorMenu[index];
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		Door *door=(Door *)((void **)dataRoot)[1];
		switch(selectedIndex)
		{
		case 0: door->Open(); break;
		case 1: door->Close(); break;
		case 2: door->Stop(); break;
		}
		return NoChange;
	}

	void MFDRenderer()
	{
		AscensionTowerPage::MFDRenderer();
		mfd->SetWriteStyle(0,2);
		Door* door=(Door *)((void **)dataRoot)[1];
		if (door->GetPosition()<=0) mfd->Write("Closed", 15);
		else if (door->GetPosition()>=1) mfd->Write("Open", 15);
		else mfd->Write("Moving", 15);
	}

};