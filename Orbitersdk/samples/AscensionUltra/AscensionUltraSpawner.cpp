#pragma unmanaged
#include "AscensionUltraSpawner.h"
#include "AscensionUltraConfigurator.h"

void AscensionUltraSpawner::clbkSimulationStart(oapi::Module::RenderMode mode)
{
	if (GetPrivateProfileInt(SECTION, SPAWN, 1, INIFILE)==0) return;

	//Check for earth
	OBJHANDLE earth=oapiGetGbodyByName("Earth");
	if (earth==NULL) return; //No earth here, so no default AU

	VESSELSTATUS2 stat;
	memset(&stat, 0, sizeof(VESSELSTATUS2));
	stat.version=2;
	
	//Check for AscensionUltra vessel already present in the scenario at the proper location
	for (int i=oapiGetVesselCount();i-->0;)
	{
		OBJHANDLE obj=oapiGetVesselByIndex(i);
		if (oapiIsVessel(obj))
		{
			VESSEL *vessel=oapiGetVesselInterface(obj);
			if (strcmp(vessel->GetClassNameA(), CLASSNAME)==0)
			{
				//Check for proper location, could be another instance of the base!
				vessel->GetStatusEx(&stat);
				if (stat.status==1 &&
					abs(stat.surf_lat-LATITUDE)<0.1 &&
					abs(stat.surf_lng-LONGITUDE)<0.1 &&
					abs(stat.surf_hdg-HEADING)<1) return;
			}
		}
	}
	
	//Create a new AscensionUltra vessel at the proper location	
	memset(&stat, 0, sizeof(VESSELSTATUS2));
	stat.version=2;
	stat.status=1;
	stat.rbody=earth;
	stat.surf_lat=LATITUDE;
	stat.surf_lng=LONGITUDE;
	stat.surf_hdg=HEADING;
	oapiCreateVesselEx(AUNAME, CLASSNAME, &stat);
}