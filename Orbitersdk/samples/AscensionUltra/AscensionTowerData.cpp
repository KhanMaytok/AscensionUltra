#include "AscensionTowerData.h"

AscensionTowerData::AscensionTowerData(void)
{
	ascensionHandle=NULL;
	ascensionName=NULL;
	ascension=NULL;
	page=0;
}

AscensionTowerData::~AscensionTowerData(void)
{
	delete [] ascensionName;
	for (std::map<UINT, char*>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->second;
	scanList.clear();
}

// Resolves object reference by means of:
// 1. using the index, check pointer
// 2. get all vessels of class AscensionUltra, check for unique name
// 3. return NULL
AscensionUltra *AscensionTowerData::GetAscension()
{	
	if (page<0) return NULL;
	if (oapiIsVessel(ascensionHandle)) return ascension;
	Scan();
	int detected=-1;
	if (ascensionName!=NULL) for (std::map<UINT, char*>::iterator i=scanList.begin(); i!=scanList.end(); i++)
	{
		if (strcmp(i->second, ascensionName)==0)
		{
			if (detected>=0) return NULL; //Name is not unique
			detected=i->first;			
		}
	}
	if (detected<0)
	{
		if (scanList.size()!=1) return NULL;
		detected=scanList.begin()->first;
	}
	SetAscension(detected);
	if (page<0) return NULL;
	return ascension;
}

int AscensionTowerData::GetPage(){return page;}
void AscensionTowerData::SetPage(int page){this->page=page;}

void AscensionTowerData::SetAscension(int index)
{
	if (index<0 || index>=oapiGetVesselCount()) return Scan();
	ascensionHandle=oapiGetVesselByIndex(index);
	VESSEL *vessel=oapiGetVesselInterface(ascensionHandle);	
	if (strcmp(vessel->GetClassName(), "AscensionUltra")!=0) return Scan();
	ascension=(AscensionUltra *)vessel;
	char *name=ascension->GetName();
	delete [] ascensionName;
	strcpy(ascensionName=new char[strlen(name)], name);	
	page=0;
}

void AscensionTowerData::Scan()
{
	for (std::map<UINT, char*>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->second;
	scanList.clear();
	for (int i=oapiGetVesselCount()-1;i>=0;i--)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			char *target=new char[strlen(source)];
			strcpy(target, source);
			scanList[i]=target;
		}
	}
	page=-1;
}