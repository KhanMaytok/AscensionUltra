#include "BasePage.h"
#pragma warning(disable : 4482)

BasePage::BasePage(AscensionTowerData *data):AscensionTowerPage(data)
{
	ascensionHandle=NULL;
	ascensionName=NULL;
}

BasePage::~BasePage(void)
{
	delete [] ascensionName;
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
}

// Resolves object reference by means of:
// 1. using the index, check pointer
// 2. get all vessels of class AscensionUltra, check for unique name
// 3. return NULL
AscensionUltra *BasePage::GetAscension()
{	
	if (oapiIsVessel(ascensionHandle)) return ascension;
	Scan();
	int detected=-1;
	if (ascensionName!=NULL) for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++)
	{
		if (strcmp(i->Name, ascensionName)==0)
		{
			if (detected>=0)
			{
				return ascension=NULL; //Name is not unique
			}
			detected=i->Index;			
		}
	}
	if (detected<0)
	{
		if (scanList.size()!=1)
		{
			return ascension=NULL;
		}
		detected=scanList.begin()->Index;
	}
	SetAscension(detected);
	return ascension;
}

void BasePage::SetAscension(int index)
{
	ascension=NULL;
	if (index<0 || index>=(int)oapiGetVesselCount()) return Scan();
	ascensionHandle=oapiGetVesselByIndex(index);
	VESSEL *vessel=oapiGetVesselInterface(ascensionHandle);	
	if (strcmp(vessel->GetClassName(), "AscensionUltra")!=0) return Scan();
	ascension=(AscensionUltra *)vessel;
	char *name=ascension->GetName();
	double version=ascension->GetVersion();
	if (version!=1.0)
	{
		ascension=NULL;
		sprintf(oapiDebugString(), "%s has version %f (need 1.0). Please upgrade MFD and/or vessel.", name, version);
		return Scan();
	}
	delete [] ascensionName;
	strcpy(ascensionName=new char[strlen(name)+1], name);	
}

void BasePage::Scan()
{
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
	int k=oapiGetVesselCount();
	char line[20]; //enough for int conversion base 10
	char *target;
	for (int i=0;i<k;i++)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			int l=strlen(_itoa(i, line, 10));
			int sum=strlen(source)+l+3;
			if (sum<37) sprintf(target=new char[sum+1], "[%s] %s", line, source);
			else
			{
				sprintf(target=new char[37], "[%s] ", line);
				strncat(target, source, 31-l);
				strcat(target, "..");
			}
			AscensionTowerListPair pair={i,target};
			scanList.push_back(pair);
		}
	}	
}

AscensionTowerPageInstance BasePage::Select(int index)
{
	AscensionTowerPage::Select(index);
	SetAscension(selectedIndex);
	return MainMenu;
}

// Return button labels
char *BasePage::LabelRenderer (int bt)
{
	switch (bt)
	{
		case 6: return "BAS";
		default: return AscensionTowerPage::LabelRenderer(bt);
	}
}


// Return button menus
int BasePage::MenuRenderer (MFDBUTTONMENU *mnu)
{	
	static MFDBUTTONMENU menu={"Scan for bases",NULL,'H'};
	mnu[0].line1="Select base";
	mnu[6]=menu;
	return AscensionTowerPage::MenuRenderer(mnu);
}

// Handling shortcut keys
AscensionTowerPageInstance BasePage::KeyHandler(DWORD key)
{	
	switch(key)
	{	
	case OAPI_KEY_H://Scan for bases
		return BaseSelect;
	default:
		return AscensionTowerPage::KeyHandler(key);
	}
}