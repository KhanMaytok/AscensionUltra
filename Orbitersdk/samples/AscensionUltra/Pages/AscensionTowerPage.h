#pragma once
#include "AscensionTowerData.h"

typedef enum AscensionTowerPageInstance
{
	BaseSelect,
	MainMenu,
	GroundMenu,
	ATCMenu,
	HangarForRoomSelection,
	RoomSelection,
	HangarForDoorSelection,
	DoorSelection,
	TaxiRouteStartSelection,
	TaxiRouteEndSelection,
	HangarForCraneSelection,
	PassengerTerminal,
	Fueling,
	LaunchPrepare,
	Bearing,
	LandingRunwaySelection,
	Launch,
	DoorControl,
	CraneControl,
	Roster,
	PersonControl,
	RoomForPersonSelection,
	HangarForPersonSelection,
	PassengerTransfer,
	CraneList,
	CraneGrapple,
	Reset,
	NoChange,
	Undefined,
};

struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

class AscensionTower;
class AscensionTowerData;

class AscensionTowerPage
{
public:
	AscensionTowerPage(AscensionTowerData *data);
	void Update();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	AscensionTowerPageInstance SetButton(int bt);
	AscensionTowerPageInstance SetKey(DWORD key);
	void SetDataRoot(void *dataRoot); // Sets the root object of the page data items, e.g. the hangar the doors are in on the doors list page
	void SetIndex(int index=-1);
	
protected:
	AscensionTowerData *data;
	AscensionUltra *ascension;
	AscensionTower *mfd;
	VESSEL *vessel;
	int selectedIndex, selection, page;
	char *GetNameSafeTitle(char *trailer);
	virtual void MFDRenderer();
	virtual int GetListSize();
	virtual AscensionTowerListPair GetListItem(int index);
	virtual AscensionTowerPageInstance Select(int index=-1);
	virtual char *GetTitle();
	virtual char *GetSubtitle();
	virtual char *LabelRenderer (int bt);
	virtual int MenuRenderer (MFDBUTTONMENU *mnu);
	virtual AscensionTowerPageInstance ButtonHandler(int bt);
	virtual AscensionTowerPageInstance KeyHandler(DWORD key);
	int AT_BUTTON[6];
	int AT_BUTTONDOUBLED[10];
	void *dataRoot;

private:
	void RefreshHandles();
};
