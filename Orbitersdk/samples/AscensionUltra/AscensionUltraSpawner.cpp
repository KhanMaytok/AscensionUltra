#pragma unmanaged
#include "AscensionUltraSpawner.h"
#include "AscensionUltraConfigurator.h"

extern gParamsType gParams;

AscensionUltraSpawner::AscensionUltraSpawner(HINSTANCE hDLL) : oapi::Module(hDLL)
{
	FILEHANDLE f=oapiOpenFile(ORBITERCONFIG, FILE_IN);
	if (!oapiReadItem_bool(f, SCNSAVE, gParams.SCNSave)) gParams.SCNSave=true; //Scenario saving default is true
	if (!oapiReadItem_bool(f, RECSAVE, gParams.RecSave)) gParams.RecSave=true; //Recorder saving default is true
	if (!oapiReadItem_bool(f, SPAWN, gParams.Spawn)) gParams.Spawn=true; //Spawning default is true
	if (!oapiReadItem_bool(f, RESET, gParams.Reset)) gParams.Reset=true; //Reseting default is true
	oapiCloseFile(f, FILE_IN);
}

void AscensionUltraSpawner::clbkSimulationStart(oapi::Module::RenderMode mode)
{
	Save();
	if (!gParams.Spawn) return;

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

void AscensionUltraSpawner::Save()
{
	FILEHANDLE f=oapiOpenFile(ORBITERCONFIG, FILE_APP);
	oapiWriteLine(f, "\n; === Base Vessel Configuration ===");
	oapiWriteItem_bool(f, SCNSAVE, gParams.SCNSave);
	oapiWriteItem_bool(f, RECSAVE, gParams.RecSave);
	oapiWriteItem_bool(f, SPAWN, gParams.Spawn);
	oapiWriteItem_bool(f, RESET, gParams.Reset);
	oapiCloseFile(f, FILE_APP);
}