#pragma once
#include <map>
#include "AscensionUltra.h"
#include "orbitersdk.h"

typedef enum AscensionTowerState
{
	BaseSelect,
	MainMenu,
};

typedef struct AscensionTowerListPair
{
	UINT index;
	char *name;
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
	bool StartList();
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
	std::list<AscensionTowerListPair> scanList;
	std::list<AscensionTowerListPair>::iterator listIter;
	int page;
	AscensionTowerState state;
	int selection;
	void Scan();
	void SetAscension(int index);
};
