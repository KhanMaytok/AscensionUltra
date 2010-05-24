#pragma once
#include <map>
#include "AscensionUltra.h"
#include "orbitersdk.h"

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();
	void SetAscension(int index);
	int GetPage();
	void SetPage(int page);
	int GetListSize();
	bool StartList();
	bool NextList();
	int GetListIndex();
	char *GetListName();
	int GetSelection();
	void SetSelection(int selection);
private:	
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	AscensionUltra *ascension;
	std::map<UINT, char*> scanList;
	std::map<UINT, char*>::iterator listIter;
	int page;
	int selection;
	void Scan();
};
