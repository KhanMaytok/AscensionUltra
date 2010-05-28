#pragma once
#include <vector>
#include "AscensionUltra.h"
#include "orbitersdk.h"

typedef enum AscensionTowerState
{
	BaseSelect,
	MainMenu,
	GroundMenu,
	ATCMenu,
	HangarForRoomSelection,
	RoomSelection,
	HangarForDoorSelection,
	DoorSelection,
	TaxiRouteSelection,
	HangarForCraneSelection,
	PassengerTerminal,
	Fueling,
	LaunchTunnel,
	Bearing,
	LandingRunwaySelection,
	Launch,
};

typedef struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();	
	int GetPage();
	int GetListSize();
	bool StartList(int index=0);
	bool NextList();
	AscensionTowerListPair GetListItem();
	int GetSelection();
	AscensionTowerState GetState();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	bool SetButton(int bt);
	bool SetKey(DWORD key);
	void SetState(AscensionTowerState state);
	void SetPage(int page);
	char *GetTitle();
	char *GetSubtitle();
private:	
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	AscensionUltra *ascension;
	std::vector<AscensionTowerListPair> scanList;
	int listIter;
	int page[10]; //stores page information - 10 is arbitrary for now
	AscensionTowerState state;
	int selection[10]; //stores selection information - 10 is arbitrary for now
	void Scan();
	void SetAscension(int index);
	bool ListEnd();	
	void Select();
	void Back();
};
