#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class CraneBasePage: public AscensionTowerPage
{

public:

	CraneBasePage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	void MFDRenderer()
	{
		char line[40];
		Crane *crane=(Crane *)((void **)dataRoot)[1];
		double step=*(double *)((void **)dataRoot)[2];
		VECTOR3 pos=crane->GetPosition();
		mfd->SetWriteStyle(0);
		int mode=crane->GetMode();
		switch (mode)
		{
		case CRANEMANUAL:
			sprintf(line, "%6.2f %6.2f %6.2f MANUAL %6.2f", pos.x, pos.y, pos.z, step);
			break;
		case CRANEDIRECT:
			sprintf(line, "%6.2f %6.2f %6.2f DIRECT", pos.x, pos.y, pos.z);
			break;
		default:
			sprintf(line, "%6.2f %6.2f %6.2f AUTO %d", pos.x, pos.y, pos.z, mode);
		}		
		mfd->Write(line, 50, -1, WRITEMFD_HALFLINES);
	}

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 0: return "X+";
			case 1: return "X-";
			case 2: return "Y+";
			case 3: return "Y-";
			case 4: return "Z+";
			case 5: return "Z-";
			case 6: return "STP";
			case 11: return "BCK";
			default: return NULL;
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[8] = 
		{
			{"Move X axis", "pos. by step", 'A'},
			{"Move X axis", "neg. by step", 'D'},
			{"Move Y axis", "pos. by step", 'W'},
			{"Move Y axis", "neg. by step", 'S'},
			{"Move Z axis", "up by step", 'Q'},
			{"Move Z axis", "down by step", 'E'},
			{"Toggle step", "size", 'R'},
			{"Go back", NULL, 'B'}
		};
		
		for(int i=0;i<7;i++) mnu[i]=menu[i];
		mnu[11]=menu[7];
		return 8;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		switch(bt)
		{
			case 0: return SetKey(OAPI_KEY_A);
			case 1: return SetKey(OAPI_KEY_D);
			case 2: return SetKey(OAPI_KEY_W);
			case 3: return SetKey(OAPI_KEY_S);
			case 4: return SetKey(OAPI_KEY_Q);
			case 5: return SetKey(OAPI_KEY_E);
			case 6: return SetKey(OAPI_KEY_R);
			case 11: return SetKey(OAPI_KEY_B);
			default: return Undefined;
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		Crane *crane=(Crane *)((void **)dataRoot)[1];
		double *pstep=(double *)((void **)dataRoot)[2];
		double step=*pstep;
		VECTOR3 pos=crane->GetPosition();
		if (crane->GetMode()<=CRANEMANUAL) switch(key)
		{
		case OAPI_KEY_A:
			pos.x+=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_D:
			pos.x-=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_W:
			pos.y+=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_S:
			pos.y-=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_Q:
			pos.z+=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_E:
			pos.z-=step;
			crane->SetPosition(pos);
			return NoChange;
		case OAPI_KEY_R:
			step*=10;
			if (step>100) step=0.01;
			*pstep=step;			
			return NoChange;
		case OAPI_KEY_B:
			return HangarForCraneSelection;
		default:
			return Undefined;
		}
		else
		{
			//TODO: invalid sound
			if (key==OAPI_KEY_B) return HangarForCraneSelection;
		}
		return Undefined;
	}

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle()
	{
		static char subtitle[57];
		sprintf(subtitle, "%s -> Crane -> ", ((Hangar *)((void **)dataRoot)[0])->GetName());
		return subtitle;		
	}
	
};