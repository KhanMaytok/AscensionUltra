#include "TransferDefaultRoom.h"
#include "Hangar.h"

void TransferDefaultRoom::PostPostStep()
{
	Hangar *hangar=GetHangar();
	char *name=GetName();
	VesselRoomTransfer *transfer=GetTransfered();
	if (transfer->Name!=NULL)
	{
		Room *room=transfer->Room;
		if (room->GetCrew()->GetCrewTotalNumber()>=room->GetMaxPersons()) room=this; //Default to this room
		room->GetCrew()->AddCrewMember(transfer->Name, transfer->Age, transfer->Puls, transfer->Weight, transfer->MiscId);		
	}
	ResetTransfered();
}