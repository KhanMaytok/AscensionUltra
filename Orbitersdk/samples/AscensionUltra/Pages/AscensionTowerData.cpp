#pragma once
#include "AscensionTowerData.h"
#include "BasePage.h"
#include "MainPage.h"
#include "GroundPage.h"
#include "ATCPage.h"
#include "HangarRoomPage.h"
#include "RosterPage.h"
#include "RoomListPage.h"
#include "HangarDoorPage.h"
#include "DoorListPage.h"
#include "TaxiRouteStartPage.h"
#include "TaxiRouteEndPage.h"
#include "HangarCranePage.h"
#include "RunwayListPage.h"
#include "PersonHangarPage.h"
#include "PersonRoomPage.h"
#include "PaXferPage.h"
#include "DoorPage.h"
#include "TemplatePage.h"
#include "PersonPage.h"
#include "CraneBasePage.h"
#include "CranePage.h"
#include "CraneListPage.h"
#include "CraneGrapplePage.h"
#pragma warning(disable : 4482)

//TODO: implement base auto-select and paxfer auto-clear

AscensionTowerData::AscensionTowerData()
{
	state=BaseSelect;
	pages[BaseSelect]=new BasePage(this);
	pages[MainMenu]=new MainPage(this);
	pages[GroundMenu]=new GroundPage(this);
	pages[ATCMenu]=new ATCPage(this);
	pages[HangarForRoomSelection]=new HangarRoomPage(this);
	pages[Roster]=new RosterPage(this);
	pages[RoomSelection]=new RoomListPage(this);
	pages[HangarForDoorSelection]=new HangarDoorPage(this);
	pages[DoorSelection]=new DoorListPage(this);
	pages[TaxiRouteStartSelection]=new TaxiRouteStartPage(this);
	pages[TaxiRouteEndSelection]=new TaxiRouteEndPage(this);
	pages[HangarForCraneSelection]=new HangarCranePage(this);
	pages[LandingRunwaySelection]=new RunwayListPage(this);
	pages[HangarForPersonSelection]=new PersonHangarPage(this);
	pages[RoomForPersonSelection]=new PersonRoomPage(this);
	pages[PassengerTransfer]=new PaXferPage(this);
	pages[PassengerTerminal]=new TemplatePage(this);
	pages[Fueling]=new TemplatePage(this);
	pages[LaunchPrepare]=new TemplatePage(this);
	pages[Bearing]=new TemplatePage(this);
	pages[Launch]=new TemplatePage(this);
	pages[DoorControl]=new DoorPage(this);
	pages[CraneControl]=new CranePage(this);
	pages[PersonControl]=new PersonPage(this);
	pages[CraneList]=new CraneListPage(this);
	pages[CraneGrapple]=new CraneGrapplePage(this);
	pages[Reset]=new TemplatePage(this);
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
void AscensionTowerData::Update(){pages[state]->Update();}
AscensionTower *AscensionTowerData::GetMFD(){return mfd;}
void AscensionTowerData::SetMFD(AscensionTower *mfd){this->mfd=mfd;}
AscensionUltra *AscensionTowerData::GetAscension(){return ((BasePage *)pages[BaseSelect])->GetAscension();}