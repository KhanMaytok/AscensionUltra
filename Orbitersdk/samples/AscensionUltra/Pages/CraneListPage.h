#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256

bool EditPosition(void *id, char *str, void *usrdata)
{
	AscensionTowerCallbackData *cp=(AscensionTowerCallbackData *)usrdata;
	Crane *crane=(Crane*)cp->Data;
	char *end=str+strlen(str);
	for(;str!=end;str++) if (*str>32) break;
	switch (*str)
	{
	case 0:
		crane->SetWaypoint(cp->Index, _V(LISTEMPTY, 0, 0));
		break;
	case 's':
		str++;
		end=str+strlen(str);
		for(;str!=end;str++) if (*str>32) break;
		if (str==end) crane->SetWaypoint(cp->Index, _V(LISTSTOP, 0, 0));
		else
		{
			float speed, crawl;
			if (sscanf(str, "%f %f", &speed, &crawl)!=2) return false;
			crane->SetWaypoint(cp->Index, _V(LISTSPEEDS, speed, crawl));
		}
		break;
	case 'j':
		{
			unsigned int dest;
			if (sscanf(++str, "%u", &dest)!=1) return false;
			crane->SetWaypoint(cp->Index, _V(LISTJUMP, dest, 0));
		}
		break;
	case 'p':
		{
			float pause;
			if (sscanf(++str, "%f", &pause)!=1) return false;
			crane->SetWaypoint(cp->Index, _V(LISTPAUSE, pause, 0));
		}
		break;
	case 'g':
		crane->SetWaypoint(cp->Index, _V(LISTGRAPPLE, 0, 0));
		break;
	case 'r':
		crane->SetWaypoint(cp->Index, _V(LISTRELEASE, 0, 0));
		break;
	default:
		{
			float x,y,z;
			if (sscanf(str, "%f %f %f", &x,&y,&z)!=3) return false;			
			VECTOR3 len=crane->GetLength();
			if (x<0 || y<0 || z<0 || x>len.x || y>len.y || z>len.z) return false;
			crane->SetWaypoint(cp->Index, _V(x, y, z));
		}
	}
	return true;
}

class CraneListPage: public CraneBasePage
{

public:

	CraneListPage(AscensionTowerData *data):CraneBasePage(data){}

protected:

	void MFDRenderer()
	{
		char line[80];
		int size=GetListSize();
		int pages=(size+9)/10;
		
		if (page>=pages)
		{
			page=pages-1;
			mfd->InvalidateButtons();
		}
		
		mfd->SetWriteStyle(0);
		for(int i=0; i+page*10<size && i<10; i++)
			mfd->Write(GetListItem(i+page*10).Name, AT_BUTTONDOUBLED[i], 4, WRITEMFD_HALFLINES | (i==selection?WRITEMFD_HIGHLIGHTED:0));	
		sprintf(line, "p.%d/%d", page+1, pages);
		mfd->Write(line, 27, -1, WRITEMFD_RIGHTALIGNED);

		CraneBasePage::MFDRenderer();
		for(int i=0; i+page*10<size && i<10; i++)
		{
			sprintf(line, "%d", i+page*10);
			mfd->Write(line, AT_BUTTONDOUBLED[i], 1, WRITEMFD_HALFLINES);
		}
	}

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 7: return "TEA";
			case 8: return "NXT";
			case 9: return "PRV";
			default: return CraneBasePage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[3] = 
		{
			{"Teach position", NULL, 'T'},
			{"Next list", "entry", 'N'},
			{"Previous list", "entry", 'P'}
		};
		
		for(int i=0;i<3;i++) mnu[7+i]=menu[i];	
		return CraneBasePage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		switch(bt)
		{
		case 7: return SetKey(OAPI_KEY_T);
		case 8: return SetKey(OAPI_KEY_N);
		case 9: return SetKey(OAPI_KEY_P);
		default: return CraneBasePage::ButtonHandler(bt);
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		int size=GetListSize();
		int pages=(size+9)/10;		
		selectedIndex=page*10+selection;		
		switch(key)
		{		
		case OAPI_KEY_T:
			{
				Crane *crane=(Crane *)((void **)dataRoot)[1];
				VECTOR3 pos=crane->GetPosition();
				VECTOR3 waypoint=crane->GetWaypoint(selectedIndex);
				sprintf(buffer, "%6.2f %6.2f %6.2f", pos.x, pos.y, pos.z);
				if (waypoint.x<0)
				{
					if (waypoint.y<0 && waypoint.z<0) break;
					else switch ((int)waypoint.x)
					{
					case LISTSTOP:		sprintf(buffer, "s"); break;
					case LISTJUMP:		sprintf(buffer, "j%d", (int)waypoint.y); break;
					case LISTPAUSE:		sprintf(buffer, "p%f", waypoint.y); break;
					case LISTGRAPPLE:	sprintf(buffer, "g"); break;
					case LISTRELEASE:	sprintf(buffer, "r"); break;
					case LISTSPEEDS:	sprintf(buffer, "s%f %f", waypoint.y, waypoint.z); break;
					}
				}
				editPosition.Data=crane;
				editPosition.Index=selectedIndex;
			}
			oapiOpenInputBox("Edit list entry ( * * *|s|j*|p*|g|r|s* *|empty ):", EditPosition, buffer, 26, (void *)&editPosition);
			return NoChange;
		case OAPI_KEY_N:
			if (selection<min(size-page*10, 10)-1) selection++;
			else
			{
				if (page<pages-1) page++;
				else page=0;
				selection=0;
			}
			return NoChange;
		case OAPI_KEY_P:
			if (selection>0) selection--;
			else
			{
				if (page>0) page--;
				else page=pages-1;
				selection=min(size-page*10, 10)-1;
			}
			return NoChange;
		case OAPI_KEY_M: return CraneGrapple;
		default: return CraneBasePage::KeyHandler(key);
		}
	}

	char *GetSubtitle(){return strcat(CraneBasePage::GetSubtitle(), "List");}

	int GetListSize(){return ((Crane *)((void **)dataRoot)[1])->GetWaypoints();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		static AscensionTowerListPair item=
		{
			index,
			text
		};
		
		VECTOR3 waypoint=((Crane *)((void **)dataRoot)[1])->GetWaypoint(index);
		if (waypoint.x<0)
		{
			if (waypoint.y<0 && waypoint.z<0)
								sprintf(text, "OUT OF RANGE");
			else switch ((int)waypoint.x)
			{
			case LISTSTOP:		sprintf(text, "STOP"); break;
			case LISTJUMP:		sprintf(text, "JUMP %d", (int)waypoint.y); break;
			case LISTPAUSE:		sprintf(text, "PAUSE %f", waypoint.y); break;
			case LISTGRAPPLE:	sprintf(text, "GRAPPLE"); break;
			case LISTRELEASE:	sprintf(text, "RELEASE"); break;
			case LISTSPEEDS:	sprintf(text, "SPEED %f, CRAWL %f", waypoint.y, waypoint.z); break;
			case LISTEMPTY:		sprintf(text, ""); break;
			default:			sprintf(text, "ILLEGAL OPERATION"); break;
			}
		} else					sprintf(text, "GOTO %6.2f %6.2f %6.2f", waypoint.x, waypoint.y, waypoint.z);
		
		return item;
	}

private:
	AscensionTowerCallbackData editPosition;
	char buffer[BUFFERLEN+1];

};