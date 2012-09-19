#include "AscensionTowerPage.h"

AscensionTowerPage::AscensionTowerPage(AscensionTowerData *data)
{
	this->data=data;
}

AscensionTowerPage::~AscensionTowerPage(void)
{
}

void AscensionTowerPage::WriteMFD(char *text, int line, int column, int flags)
{
	int l=strlen(text);
	int x=0;
	int y=0;
	if (line<0)
	{
		x=(1+(36-l)/2)*width;
		y=13*height;		
	}
	else
	{
		y=(int)(line*height) >> (HALFLINES(flags)?1:0);
		if (column<0 && !(RIGHTALINED(flags))) x=(1+(36-l)/2)*width;
		else
		{
			if (RIGHTALINED(flags)) x=(column<0?mfdWidth:column*width)-(l+1)*width;
			else x=column*width;
		}
	}
	if (HIGHLIGHTED(flags))
	{
		SelectObject(hDC, g_Bar);
		Rectangle(hDC, width-2, y-2, mfdWidth-width+2, y+height+6 );
	}
	TextOut(hDC, x, y, text, l);
}

void AscensionTowerPage::RenderPage()
{
	char line[40];
	MFD *mfd=data->GetMfd();
	int size=GetListSize();
	int pages=(size+5)/6;
	if (page>=pages)
	{
		page=max(pages-1,0);
		mfd->InvalidateButtons();
	}
		
	mfd->SelectDefaultFont (hDC, 0);
	for(int i=0; i+page*6<size && i<6; i++) WriteMFD(GetListItem(i+page*6).Name, AT_BUTTON[i], 1, WRITEMFD_HALFLINES | (i==selection?WRITEMFD_HIGHLIGHTED:0));
	if (pages>0)
	{
		sprintf(line, "p.%d/%d", page+1, pages);
		WriteMFD(line, 27, -1, WRITEMFD_RIGHTALINED);
	}
	else WriteMFD("N O   B A S E S   A V A I L A B L E");
}

int AscensionTowerPage::GetListSize() {return 0;}

AscensionTowerListPair AscensionTowerPage::GetListItem(int index) {AscensionTowerListPair nullitem={0,""}; return nullitem;}

void AscensionTowerPage::Select(int index)
{
	if (index<0) selectedIndex=GetListItem(page*6+selection).Index;
	else
	{
		selectedIndex=index;
		page=index / 6;
		selection=index % 6;
	}
}

char *AscensionTowerPage::GetButtonLabel (int bt) {return "";}

int AscensionTowerPage::GetButtonMenu (MFDBUTTONMENU *mnu) { mnu=NULL; return 0;}

AscensionTowerPageInstance AscensionTowerPage::SetButton(int bt)
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
		break;
	return Undefined;
}

AscensionTowerPageInstance AscensionTowerPage::SetKey(DWORD key) {return Undefined;}

char *AscensionTowerPage::GetTitle() { static char title[57]; return GetNameSafeTitle(title, "");}

char *AscensionTowerPage::GetSubtitle() {return "";}

char *AscensionTowerPage::GetNameSafeTitle(char *title, char *trailer)
{
	char *name=data->GetAscension()->GetName();
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