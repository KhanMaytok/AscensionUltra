#pragma once
#include <map>
#include "AscensionUltra.h"

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();
	void SetAscension(int index);
	int GetPage();
	void SetPage(int page);
	bool StartList();
	bool NextList();
	int GetListIndex();
	char *GetListName();
private:	
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	AscensionUltra *ascension;
	std::map<UINT, char*> scanList;
	std::map<UINT, char*>::iterator listIter;
	int page;
	void Scan();
};
