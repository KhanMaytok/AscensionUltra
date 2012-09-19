#include "AscensionTowerPage.h"

extern HBRUSH g_Bar;
extern COLORREF g_MiddleGreen;

AscensionTowerPage::AscensionTowerPage(AscensionTowerData *data)
{
	this->data=data;
	page=0;
	selection=0;
	selectedIndex=0;
	ascension=NULL;
	int bp[6]={8, 16, 24, 33, 41, 50}; //Best choice for certain MFD size in half-height units;
	int bdp[10]={10,14,18,22,26,30,34,38,42,46}; //Best choice for certain MFD size in half-height units to display 10 entries
	for(int i=0;i<6;i++) AT_BUTTON[i]=bp[i];
	for(int i=0;i<10;i++) AT_BUTTONDOUBLED[i]=bdp[i];
}

void AscensionTowerPage::Update(HDC hDC)
{
	mfd=data->GetMFD();
	ascension=data->GetAscension();	
	MFDRenderer();

	mfd->SelectDefaultFont(hDC, 1);
	
	mfd->Title (hDC, GetTitle());	
	mfd->Write (GetSubtitle(), 2, 2);
}

void AscensionTowerPage::MFDRenderer()
{
	char line[40];
	int size=GetListSize();
	int pages=(size+5)/6;
	if (page>=pages)
	{
		page=max(pages-1,0);
		mfd->InvalidateButtons();
	}
		
	mfd->SelectDefaultFont (hDC, 0);
	for(int i=0; i+page*6<size && i<6; i++)
		mfd->Write(GetListItem(i+page*6).Name, AT_BUTTON[i], 1, WRITEMFD_HALFLINES | (i==selection?WRITEMFD_HIGHLIGHTED:0));
	if (pages>0)
	{
		sprintf(line, "p.%d/%d", page+1, pages);
		mfd->Write(line, 27, -1, WRITEMFD_RIGHTALINED);
	}
	else mfd->Write("N O   B A S E S   A V A I L A B L E");
}

int AscensionTowerPage::GetListSize() {return 0;}

AscensionTowerListPair AscensionTowerPage::GetListItem(int index) {AscensionTowerListPair nullitem={0,""}; return nullitem;}

AscensionTowerPageInstance AscensionTowerPage::Select(int index)
{
	if (index<0) selectedIndex=GetListItem(page*6+selection).Index;
	else
	{
		selectedIndex=index;
		page=index / 6;
		selection=index % 6;
	}
	return NoChange;
}

char *AscensionTowerPage::GetButtonLabel (int bt)
{
	if (bt==0) ascension=data->GetAscension();
	return LabelRenderer(bt);
}

char *AscensionTowerPage::LabelRenderer (int bt) {return "";}

int AscensionTowerPage::GetButtonMenu (MFDBUTTONMENU *mnu)
{
	ascension=data->GetAscension();
	return MenuRenderer(mnu);
}

int AscensionTowerPage::MenuRenderer (MFDBUTTONMENU *mnu) {return 0;}

AscensionTowerPageInstance AscensionTowerPage::SetButton(int bt)
{
	ascension=data->GetAscension();
	return ButtonHandler(bt);
}

AscensionTowerPageInstance AscensionTowerPage::ButtonHandler(int bt)
{
	switch(bt)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5: return SetKey(OAPI_KEY_1+bt);
	case 6: return SetKey(OAPI_KEY_H);
	case 7: return SetKey(OAPI_KEY_B);
	case 8: return SetKey(OAPI_KEY_R);
	case 10: return SetKey(OAPI_KEY_N);
	case 11: return SetKey(OAPI_KEY_P);	
	}
	return Undefined;
}

AscensionTowerPageInstance AscensionTowerPage::SetKey(DWORD key)
{
	ascension=data->GetAscension();
	return KeyHandler(key);
}

AscensionTowerPageInstance AscensionTowerPage::KeyHandler(DWORD key) {return Undefined;}

char *AscensionTowerPage::GetTitle() { static char title[57]; return GetNameSafeTitle(title, "");}

char *AscensionTowerPage::GetSubtitle() {return "";}

char *AscensionTowerPage::GetNameSafeTitle(char *title, char *trailer)
{
	char *name=ascension->GetName();
	int i=56-strlen(trailer);
	bool longer=false;
	if (strlen(name)>i)
	{
		i-=2;
		longer=true;
	}
	strncpy(title, name, i);
	title[i]=0;
	if (longer) strcat(title, "..");
	strcat(title, trailer);
	return title;
}