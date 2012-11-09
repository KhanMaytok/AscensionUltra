#pragma once
#include "AscensionTowerData.h"
#include "BasePage.h"
#include "MainPage.h"
#include "GroundPage.h"
#include "ATCPage.h"
#include "HangarRoomPage.h"
#include "RosterPage.h"
#pragma warning(disable : 4482)

AscensionTowerData::AscensionTowerData(AscensionTower* mfd)
{
	this->mfd=mfd;
	state=BaseSelect;
	pages[BaseSelect]=new BasePage(this);
	pages[MainMenu]=new MainPage(this);
	pages[GroundMenu]=new GroundPage(this);
	pages[ATCMenu]=new ATCPage(this);
	pages[HangarForRoomSelection]=new HangarRoomPage(this);
	pages[Roster]=new RosterPage(this);
}

char *AscensionTowerData::GetButtonLabel (int bt){return pages[state]->GetButtonLabel(bt);}
int AscensionTowerData::GetButtonMenu (MFDBUTTONMENU *mnu){return pages[state]->GetButtonMenu(mnu);}
bool AscensionTowerData::SetButton(int bt){return StateChange(pages[state]->SetButton(bt));}
bool AscensionTowerData::SetKey(DWORD key){return StateChange(pages[state]->SetKey(key));}

bool AscensionTowerData::StateChange(AscensionTowerPageInstance newstate)
{
	if (newstate==Undefined) return false;
	if (newstate==NoChange) return true;
	state=newstate;
	return true;
}

AscensionTowerPage *AscensionTowerData::GetPage(AscensionTowerPageInstance page){return pages[page];}

void AscensionTowerData::Update()
{
	pages[state]->Update();
}

AscensionTower *AscensionTowerData::GetMFD(){return mfd;}
AscensionUltra *AscensionTowerData::GetAscension(){return ((BasePage *)pages[BaseSelect])->GetAscension();}