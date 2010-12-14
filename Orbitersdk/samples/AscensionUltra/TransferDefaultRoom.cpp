#include "TransferDefaultRoom.h"
#include "Hangar.h"

void TransferDefaultRoom::PostStep (double simt, double simdt, double mjd)
{
	UMMUCREWMANAGMENT *crew=GetCrew();
	Hangar *hangar=GetHangar();
	char *name=GetName();
	switch (crew->ProcessUniversalMMu())
	{
	case UMMU_RETURNED_TO_OUR_SHIP:
		sprintf(oapiDebugString(),"%s \"%s\" aged %i entered %s through %s",
		crew->GetCrewMiscIdByName(crew->GetLastEnteredCrewName()),crew->GetLastEnteredCrewName()
		,crew->GetCrewAgeByName(crew->GetLastEnteredCrewName()),hangar->GetName(), name);
		break;
	case UMMU_TRANSFERED_TO_OUR_SHIP:
		VESSEL *vessel=oapiGetFocusInterface();
		if (vessel!=NULL && vessel==GetDock())
		{
			sprintf(oapiDebugString(),"%s \"%s\" aged %i transfered to %s at %s",
			crew->GetCrewMiscIdByName(crew->GetLastEnteredCrewName()),crew->GetLastEnteredCrewName()
			,crew->GetCrewAgeByName(crew->GetLastEnteredCrewName()), name, hangar->GetName());
		}
		else if (GetTransfered()) crew->RemoveCrewMember(crew->GetLastEnteredCrewName()); //Redundant transfer, we need to remove it
		else
		{
			sprintf(oapiDebugString(),"%s \"%s\" aged %i exiled to %s at %s",
			crew->GetCrewMiscIdByName(crew->GetLastEnteredCrewName()),crew->GetLastEnteredCrewName()
			,crew->GetCrewAgeByName(crew->GetLastEnteredCrewName()), name, hangar->GetName());
		}
		break;
	}
	SetTransfered(false);
}