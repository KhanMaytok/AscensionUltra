#include "AscensionTowerData.h"

AscensionTowerData::AscensionTowerData(void)
{
	ascensionHandle=NULL;
	ascensionName=NULL;
	ascension=NULL;
	page=0;
	selection=0;
	state=AscensionTowerState::MainMenu;
}

AscensionTowerData::~AscensionTowerData(void)
{
	delete [] ascensionName;
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
}

// Resolves object reference by means of:
// 1. using the index, check pointer
// 2. get all vessels of class AscensionUltra, check for unique name
// 3. return NULL
AscensionUltra *AscensionTowerData::GetAscension()
{	
	if (oapiIsVessel(ascensionHandle)) return ascension;
	Scan();
	int detected=-1;
	if (ascensionName!=NULL) for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++)
	{
		if (strcmp(i->Name, ascensionName)==0)
		{
			if (detected>=0) return NULL; //Name is not unique
			detected=i->Index;			
		}
	}
	if (detected<0)
	{
		if (scanList.size()!=1) return NULL;
		detected=scanList.begin()->Index;
	}
	SetAscension(detected);	
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
	strcpy(ascensionName=new char[strlen(name)+1], name);	
}

void AscensionTowerData::Scan()
{
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
	int k=oapiGetVesselCount();
	for (int i=0;i<k;i++)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			char *target=new char[strlen(source)+1];
			strcpy(target, source);
			AscensionTowerListPair pair={i,target};
			scanList.push_back(pair);
		}
	}	
}

int AscensionTowerData::GetListSize(){return scanList.size();}

bool AscensionTowerData::StartList(int index)
{
	listIter=index;
	return listIter<scanList.size();
}

bool AscensionTowerData::NextList()
{
	listIter++;
	return listIter<scanList.size();
}

AscensionTowerListPair AscensionTowerData::GetListItem(){return scanList[listIter];}

int AscensionTowerData::GetSelection(){return selection;}
void AscensionTowerData::SetSelection(int selection){this->selection=selection;}

AscensionTowerState AscensionTowerData::GetState(){return state;}

void AscensionTowerData::SetState(AscensionTowerState state)
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect:
		Scan();
		break;
	}
	this->state=state;
}

void AscensionTowerData::Select()
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect:		
		SetAscension(scanList[page*6+selection].Index);
		SetState(AscensionTowerState::MainMenu);
		break;	
	}
}