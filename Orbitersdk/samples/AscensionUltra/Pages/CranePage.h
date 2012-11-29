#pragma once
#include "AscensionTowerPage.h"
#include "CraneBasePage.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256

bool EditSpeed(void *id, char *str, void *usrdata)
{
	Crane *crane=(Crane*)((AscensionTowerCallbackData *)usrdata)->Data;
	VECTOR3 speed, crawl;
	if (sscanf(str, "%lf[%lf],%lf[%lf],%lf[%lf]", &speed.x, &crawl.x, &speed.y, &crawl.y, &speed.z, &crawl.z)!=6) return false;	
	crane->SetSpeed(speed);
	crane->SetCrawl(crawl);
	return true;
}

class CranePage: public CraneBasePage
{

public:

	CranePage(AscensionTowerData *data):CraneBasePage(data){}

protected:

	void MFDRenderer()
	{
		CraneBasePage::MFDRenderer();
		Crane *crane=(Crane *)((void **)dataRoot)[1];
		VECTOR3 pos=crane->GetPosition();
		VECTOR3 len=crane->GetLength();
		VECTOR3 speed=crane->GetSpeed();
		VECTOR3 crawl=crane->GetCrawl();
		pos.x/=len.x;
		pos.y/=len.y;
		pos.z/=len.z;
		char line[80];
		sprintf(line, "X: %5.2f%% @%6.3f(%6.3f)m/s", pos.x, speed.x, crawl.x);
		mfd->Write(line, 6, 2);
		sprintf(line, "Y: %5.2f%% @%6.3f(%6.3f)m/s", pos.y, speed.y, crawl.y);
		mfd->Write(line, 8, 2);
		sprintf(line, "Z: %5.2f%% @%6.3f(%6.3f)m/s", pos.z, speed.z, crawl.z);
		mfd->Write(line, 10, 2);	
	}

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 7: return "AUT";
			case 8: return "SPD";			
			case 9: return "DIR";			
			default: return CraneBasePage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[3] = 
		{
			{"Toggle auto", "sequence", 'O'},
			{"Set speeds", NULL, 'F'},		
			{"Toggle direct", "command", 'C'}
		};
		
		for(int i=0;i<3;i++) mnu[7+i]=menu[i];	
		return CraneBasePage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{		
		switch(bt)
		{
		case 7: return SetKey(OAPI_KEY_O);
		case 8: return SetKey(OAPI_KEY_F);
		case 9: return SetKey(OAPI_KEY_C);
		default: return CraneBasePage::ButtonHandler(bt);
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		Crane *crane=(Crane *)((void **)dataRoot)[1];
		int start=*(int *)((void **)dataRoot)[3];
		VECTOR3 pos=crane->GetPosition();
		VECTOR3 speed=crane->GetSpeed();
		VECTOR3 crawl=crane->GetCrawl();

		editSpeed.Data=crane;

		switch(key)
		{		
		case OAPI_KEY_O:
			crane->SetMode(crane->GetMode()>CRANEMANUAL?CRANEMANUAL:start);						
			return NoChange;
		case OAPI_KEY_F:
			sprintf(buffer, "%6.2f[%6.2f], %6.2f[%6.2f], %6.2f[%6.2f]", speed.x, crawl.x, speed.y, crawl.y, speed.z, crawl.z);
			oapiOpenInputBox("Edit speeds (long,short,reel with fast[slow]):", EditSpeed, buffer, 52, (void *)&editSpeed);
			return NoChange;
		case OAPI_KEY_C:
			crane->SetMode(crane->GetMode()==CRANEDIRECT?CRANEMANUAL:CRANEDIRECT);
			return NoChange;
		case OAPI_KEY_M: return CraneList;
		default: return CraneBasePage::KeyHandler(key);
		}
	}

	char *GetSubtitle(){return strcat(CraneBasePage::GetSubtitle(), "Settings");}

private:
	AscensionTowerCallbackData editSpeed;
	char buffer[BUFFERLEN+1];

};