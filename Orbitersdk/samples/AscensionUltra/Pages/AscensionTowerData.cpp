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
#include "HangarLaunchPage.h"
#include "LaunchTunnelWizardPage.h"
#include "LaunchTunnelATCPage.h"
#pragma warning(disable : 4482)

void EventHandler(BaseVessel::EventHandler::Arguments args, void *context)
{
	AscensionTowerData *data=(AscensionTowerData *) context;
	if (!data->StateChange(data->GetPage(CurrentState)->SetEvent(args))) return;
	data->GetMFD()->InvalidateButtons();
	data->GetMFD()->InvalidateDisplay();
}

AscensionTowerData::AscensionTowerData()
{
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
	pages[HangarForLaunchSelection]=new HangarLaunchPage(this);
	pages[LaunchTunnelWizard]=new LaunchTunnelWizardPage(this);
	pages[VerticalLaunchWizard]=new TemplatePage(this);
	pages[Bearing]=new TemplatePage(this);
	pages[LaunchTunnelATC]=new LaunchTunnelATCPage(this);
	pages[DoorControl]=new DoorPage(this);
	pages[CraneControl]=new CranePage(this);
	pages[PersonControl]=new PersonPage(this);
	pages[CraneList]=new CraneListPage(this);
	pages[CraneGrapple]=new CraneGrapplePage(this);
	pages[Reset]=new TemplatePage(this);

	//Auto-detect base on start-up
	state=BaseSelect;
	if (GetAscension()!=NULL) StateChange(MainMenu);	
}

AscensionTowerData::~AscensionTowerData ()
{
	GetAscension()->UnregisterEventHandler(EventHandler, this);
}

char *AscensionTowerData::GetButtonLabel (int bt)
{
	PreCheck();
	return pages[state]->GetButtonLabel(bt);
}

int AscensionTowerData::GetButtonMenu (MFDBUTTONMENU *mnu)
{
	PreCheck();
	return pages[state]->GetButtonMenu(mnu);
}

bool AscensionTowerData::SetButton(int bt)
{
	PreCheck();
	return StateChange(pages[state]->SetButton(bt));
}

bool AscensionTowerData::SetKey(DWORD key)
{
	PreCheck();
	return StateChange(pages[state]->SetKey(key));
}

bool AscensionTowerData::StateChange(AscensionTowerPageInstance newstate)
{
	if (newstate==Undefined) return false;
	if (newstate==NoChange) return true;
	//If we are switching FROM base menu, register handler
	if (state==BaseSelect) GetAscension()->RegisterEventHandler(EventHandler, this);
	//If we are switching TO base menu, unregister handler
	if (newstate==BaseSelect) GetAscension()->UnregisterEventHandler(EventHandler, this);
	state=newstate;
	return true;
}

AscensionTowerPage *AscensionTowerData::GetPage(AscensionTowerPageInstance page){return pages[page!=CurrentState?page:state];}

void AscensionTowerData::Update()
{
	PreCheck();
	pages[state]->Update();
}

AscensionTower *AscensionTowerData::GetMFD(){return mfd;}
void AscensionTowerData::SetMFD(AscensionTower *mfd){this->mfd=mfd;}
AscensionUltra *AscensionTowerData::GetAscension(){return ((BasePage *)pages[BaseSelect])->GetAscension();}

void AscensionTowerData::PreCheck()
{
	AscensionUltra *au=GetAscension();
	if (au==NULL) state=BaseSelect;
	if (state==PassengerTransfer && au->GetNearestHangar(HANGARS, GetMFD()->GetVessel())==NULL) state==GroundMenu;
	pages[state]->RefreshHandles(au);
}