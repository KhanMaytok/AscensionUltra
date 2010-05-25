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
	for (std::map<UINT, char*>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->second;
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
	for (std::map<UINT, char*>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->second;
	scanList.clear();
	for (int i=oapiGetVesselCount()-1;i>=0;i--)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			char *target=new char[strlen(source)+1];
			strcpy(target, source);
			scanList[i]=target;
		}
	}	
}

int AscensionTowerData::GetListSize(){return scanList.size();}

bool AscensionTowerData::StartList()
{
	listIter=scanList.begin();
	return listIter!=scanList.end();
}

bool AscensionTowerData::NextList()
{
	listIter++;
	return listIter!=scanList.end();
}

int AscensionTowerData::GetListIndex(){return listIter->first;}

char *AscensionTowerData::GetListName(){return listIter->second;}

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
		StartList();
		for (int i=0;i<page;i++) for (int j=0;j<6;j++) NextList();
		for (int i=0;i<selection;i++) NextList();
		SetAscension(GetListIndex());
		SetState(AscensionTowerState::MainMenu);
		break;	
	}
}