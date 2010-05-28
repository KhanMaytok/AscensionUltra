#pragma once
#include <vector>
#include "AscensionUltra.h"
#include "orbitersdk.h"

typedef enum AscensionTowerState
{
	BaseSelect,
	MainMenu,
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
	void SetPage(int page);
	int GetListSize();
	bool StartList(int index=0);
	bool NextList();
	AscensionTowerListPair GetListItem();
	int GetSelection();
	void SetSelection(int selection);
	AscensionTowerState GetState();
	void SetState(AscensionTowerState state);
	void Select();
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
};
