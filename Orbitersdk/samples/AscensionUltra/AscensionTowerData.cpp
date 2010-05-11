#include "AscensionTowerData.h"

AscensionTowerData::AscensionTowerData(void)
{
	ascensionIndex=0;
	ascension=NULL;
	page=0;
}

AscensionTowerData::~AscensionTowerData(void)
{
}

// Resolves object reference by means of:
// 1. using the index, check pointer
// 2. get all vessels of class AscensionUltra, check for unique name
// 3. return NULL
AscensionUltra *AscensionTowerData::GetAscension()
{	
	if (ascensionIndex<0) return NULL; //No selection	
	if (ascensionIndex<oapiGetVesselCount()) if (oapiGetVesselInterface(oapiGetVesselByIndex(ascensionIndex))==ascension) return ascension;
	Scan();
	int detected=-1;
	for (std::map<UINT, VESSEL *>::iterator i=scanList.begin(); i!=scanList.end(); i++)
	{
		if (strcmp(i->second->GetName(), ascensionName)==0)
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
	return NULL;
}

int AscensionTowerData::GetPage(){return page;}
void AscensionTowerData::SetPage(int page){this->page=page;}

void AscensionTowerData::SetAscension(int index)
{
	if (index<0 || index>=oapiGetVesselCount()) return Scan();
	VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(index));	
	if (strcmp(vessel->GetClassName(), "AscensionUltra")!=0) return Scan();
	ascension=(AscensionUltra *)vessel;
	ascensionIndex=index;
	char *name=ascension->GetName();
	delete [] ascensionName;
	strcpy(ascensionName=new char[strlen(name)], name);	
}

void AscensionTowerData::Scan()
{

}