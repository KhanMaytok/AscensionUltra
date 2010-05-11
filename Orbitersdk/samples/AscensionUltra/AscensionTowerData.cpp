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

	return NULL;
}

int AscensionTowerData::GetPage(){return page;}
void AscensionTowerData::SetPage(int page){this->page=page;}