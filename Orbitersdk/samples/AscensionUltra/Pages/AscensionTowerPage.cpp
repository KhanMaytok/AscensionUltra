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

void AscensionTowerPage::RefreshHandles(AscensionUltra *ascension)
{
	mfd=data->GetMFD();
	vessel=mfd->GetVessel();
	this->ascension=ascension;
}

void AscensionTowerPage::Update()
{
	MFDRenderer();

	mfd->SetWriteStyle(1,2);
	
	mfd->Write (GetTitle(), 0, 0);
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
		
	mfd->SetWriteStyle(0);
	for(int i=0; i+page*6<size && i<6; i++)
		mfd->Write(GetListItem(i+page*6).Name, AT_BUTTON[i], 1, WRITEMFD_HALFLINES | (i==selection?WRITEMFD_HIGHLIGHTED:0));
	if (pages>0)
	{
		sprintf(line, "p.%d/%d", page+1, pages);
		mfd->Write(line, 27, -1, WRITEMFD_RIGHTALIGNED);
	}
	else mfd->Write("N O N E   A V A I L A B L E");
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

char *AscensionTowerPage::GetButtonLabel (int bt){return LabelRenderer(bt);}

char *AscensionTowerPage::LabelRenderer (int bt) 
{
	int size=GetListSize();
	switch (bt)
	{
		case  0:
		case  1:
		case  2:
		case  3:
		case  4:
		case  5: return size>page*6+bt?" > ":NULL;
		case 10: return size>6?"NXT":NULL;
		case 11: return size>6?"PRV":NULL;
		default: return NULL;
	}
}

int AscensionTowerPage::GetButtonMenu (MFDBUTTONMENU *mnu){return MenuRenderer(mnu);}

int AscensionTowerPage::MenuRenderer (MFDBUTTONMENU *mnu)
{
	static MFDBUTTONMENU menu[9] = {
		{NULL, "next to the button", '1'},
		{NULL, menu[0].line2, '2'},
		{NULL, menu[0].line2, '3'},
		{NULL, menu[0].line2, '4'},
		{NULL, menu[0].line2, '5'},
		{NULL, menu[0].line2, '6'},
		{NULL, NULL, 0},
		{"Switch to", "next page", 'N'},
		{"Switch to", "previous page", 'P'}};
	
	const char *label=mnu[0].line1;
	for(int i=0;i<6;i++)
	{
		mnu[i]=menu[i];
		mnu[i].line1=label;
	}
	int size=GetListSize();
	for(int i=min(size-page*6, 6);i<6;i++) mnu[i]=menu[6];
	if (size<7) return 10;
	for(int i=10;i<12;i++) mnu[i]=menu[i-3];
	return 12;
}

AscensionTowerPageInstance AscensionTowerPage::SetButton(int bt){return ButtonHandler(bt);}

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

AscensionTowerPageInstance AscensionTowerPage::SetKey(DWORD key){return KeyHandler(key);}

AscensionTowerPageInstance AscensionTowerPage::KeyHandler(DWORD key)
{
	int size=GetListSize();
	int pages=(size+5)/6;
	switch(key)
	{	
	case OAPI_KEY_N://Next page
		if (size<7) return Undefined;
		if (page<pages-1) page++;
		else page=0;
		selection=0;
		return NoChange;
	case OAPI_KEY_P://Previous page
		if (size<7) return Undefined;
		if (page>0) page--;
		else page=pages-1;
		selection=min(size-page*6, 6)-1;
		return NoChange;
	default:
		if (key<OAPI_KEY_1 || key>OAPI_KEY_6) return Undefined;
		{
			int bt=key-OAPI_KEY_1;
			if (bt>=min(size-page*6, 6)) return Undefined;
			selection=bt;
			return Select();
		}		
	}
	return Undefined;
}

AscensionTowerPageInstance AscensionTowerPage::SetEvent(BaseVessel::EventHandler::Arguments args){return EventHandler(args);}

AscensionTowerPageInstance AscensionTowerPage::EventHandler(BaseVessel::EventHandler::Arguments args){return Undefined;}

char *AscensionTowerPage::GetTitle() { return GetNameSafeTitle("");}

char *AscensionTowerPage::GetSubtitle() {return "";}

char *AscensionTowerPage::GetNameSafeTitle(char *trailer)
{
	static char title[57];
	char *name=ascension->GetName();
	int i=55-strlen(trailer); //55=57-EOL-Space
	bool longer=false;
	if (strlen(name)>i)
	{
		i-=2;
		longer=true;
	}
	strncpy(title, name, i);
	title[i]=0;
	strcat(title, " ");
	if (longer) strcat(title, "..");
	strcat(title, trailer);
	return title;
}

void AscensionTowerPage::SetDataRoot(void *dataRoot){this->dataRoot=dataRoot;}

void AscensionTowerPage::SetIndex(int index){Select(index);}