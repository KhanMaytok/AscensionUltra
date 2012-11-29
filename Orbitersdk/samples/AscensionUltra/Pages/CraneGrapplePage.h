#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class CraneGrapplePage: public CraneBasePage
{

public:

	CraneGrapplePage(AscensionTowerData *data):CraneBasePage(data){}

protected:

	void MFDRenderer()
	{	
		CraneBasePage::MFDRenderer();
		Crane *crane=(Crane *)((void **)dataRoot)[1];
		char line[40];
		sprintf(line, "Cargo:");
		mfd->Write(line, 6, 4);
		sprintf(line, "Type:");
		mfd->Write(line, 8, 4);
		sprintf(line, "Weight:");
		mfd->Write(line, 10, 4);	
	}

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 7: return "";
			case 8: return "GRP";
			case 9: return "TYP";
			default: return CraneBasePage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{			
		static MFDBUTTONMENU menu[3] =
		{
			{NULL, NULL, 0},
			{"Grapple/Release", "cargo", 'G'},
			{"Toggle cargo", "type", 'T'}
		};

		for(int i=0;i<3;i++) mnu[7+i]=menu[i];	
		return CraneBasePage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		switch(bt)
		{
		case 8: return SetKey(OAPI_KEY_G);
		case 9: return SetKey(OAPI_KEY_T);
		default: return CraneBasePage::ButtonHandler(bt);
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{			
		switch(key)
		{
		case OAPI_KEY_G: return NoChange;
		case OAPI_KEY_T: return NoChange;			
		case OAPI_KEY_M: return CraneControl;
		default: return CraneBasePage::KeyHandler(key);
		}
	}

	char *GetSubtitle(){return strcat(CraneBasePage::GetSubtitle(), "Cargo");}

};