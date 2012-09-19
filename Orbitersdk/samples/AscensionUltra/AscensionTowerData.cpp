#include "AscensionTowerData.h"
#pragma warning(disable : 4482)


AscensionTowerData::AscensionTowerData(MFD* mfd, VESSEL *vessel)
{
	this->vessel=vessel;
	for(int i=0;i<STATES;i++)
	{
		page[i]=0;
		selection[i]=0;
		selectedIndex[i]=0;
		object[i]=NULL;
		step[i]=1.0;
	}
	state=AscensionTowerState::MainMenu;
	changePerson.Data=this;
	this->mfd=mfd;
}

double AscensionTowerData::GetStep(){return step[state];}
int AscensionTowerData::GetPage(){return page[state];}
void AscensionTowerData::SetPage(int page){this->page[state]=page;}

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

void *AscensionTowerData::GetObject(){return object[state];}

int AscensionTowerData::GetSelectedIndex(){return selectedIndex[state];}

MFD *AscensionTowerData::GetMfd(){return mfd;}