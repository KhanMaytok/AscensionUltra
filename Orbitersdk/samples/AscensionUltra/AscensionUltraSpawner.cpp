#pragma unmanaged
#include "AscensionUltraSpawner.h"
#include "AscensionUltraConfigurator.h"
#include "AscensionUltra.h"

extern gParamsType gParams;

DWORD WINAPI TalkerThread(LPVOID params)
{
	ISpVoice *ATC;
	CoInitialize(NULL);
	if(SUCCEEDED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&ATC)))
	{
		ResetEvent(gParams.stopped);
		while(gParams.active)
		{
			WaitForSingleObject(gParams.event, INFINITE);
			bool go=gParams.active;
			while(go && gParams.active)
			{
				OBJHANDLE focus=oapiGetFocusObject();

				EnterCriticalSection(&gParams.lock);
					if (gParams.messages.find(focus)==gParams.messages.end())
					{
				LeaveCriticalSection(&gParams.lock);
						break; //Note: this means we need a signaling if queue OR focus object is changing.
					}
					TalkerEntry *entry=gParams.messages[focus].front();
					gParams.messages[focus].pop();					
					if (entry->valid)
					{
						entry->valid=false; //Setting this before releasing the lock, so queue manipulators only delete valid entries.
				LeaveCriticalSection(&gParams.lock);
						ATC->Speak(entry->message, NULL, NULL);
						delete [] entry->message; //Release message memory
				EnterCriticalSection(&gParams.lock);
					}
					else delete entry; //Here is the only place where entries get deleted in simulation.
					if (!(go=!gParams.messages[focus].empty())) gParams.messages.erase(focus); //Check for empty queue, as somebody might have added while speaking.
				LeaveCriticalSection(&gParams.lock);
			}
			ResetEvent(gParams.event);
		}
		ATC->Release();
	}
	::CoUninitialize();
	SetEvent(gParams.stopped);
	return 0;
}

void Talk(LPCWSTR message, OBJHANDLE sender, OBJHANDLE receiver, int flags)
{
	TalkerEntry *entry=new TalkerEntry;
	wcscpy((WCHAR *)(entry->message=new WCHAR[wcslen(message)+1]), message);
	entry->sender=sender;
	entry->receiver=receiver;
	entry->valid=true;

	//TODO: clear queues with override flag set
	EnterCriticalSection(&gParams.lock);
		gParams.messages[sender].push(entry);
		gParams.messages[receiver].push(entry);
	LeaveCriticalSection(&gParams.lock);

	SetEvent(gParams.event);
}

AscensionUltraSpawner::AscensionUltraSpawner(HINSTANCE hDLL) : oapi::Module(hDLL)
{
	FILEHANDLE f=oapiOpenFile(ORBITERCONFIG, FILE_IN);
	if (!oapiReadItem_bool(f, SCNSAVE, gParams.SCNSave)) gParams.SCNSave=true; //Scenario saving default is true
	if (!oapiReadItem_bool(f, RECSAVE, gParams.RecSave)) gParams.RecSave=true; //Recorder saving default is true
	if (!oapiReadItem_bool(f, SPAWN, gParams.Spawn)) gParams.Spawn=true; //Spawning default is true
	if (!oapiReadItem_bool(f, RESET, gParams.Reset)) gParams.Reset=true; //Reseting default is true
	oapiCloseFile(f, FILE_IN);

	//Activate talker thread
	gParams.event  = CreateEvent(NULL, TRUE, FALSE, NULL);
	gParams.stopped= CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&gParams.lock);
	gParams.active = true;
	gParams.thread = CreateThread(NULL, 0, TalkerThread, NULL, NULL, NULL);
}

AscensionUltraSpawner::~AscensionUltraSpawner()
{
	//Deactivate the talker thread
	gParams.active = false;
	SetEvent(gParams.event);
	WaitForSingleObject(gParams.stopped, 100000);
	CloseHandle(gParams.thread);
	DeleteCriticalSection(&gParams.lock);
	CloseHandle(gParams.stopped);
	CloseHandle(gParams.event);

	Save();
}

void AscensionUltraSpawner::clbkFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
	EnterCriticalSection(&gParams.lock);
		bool found=gParams.messages.find(new_focus)!=gParams.messages.end();
	LeaveCriticalSection(&gParams.lock);
	if (found) SetEvent(gParams.event);
}

void AscensionUltraSpawner::clbkSimulationStart(oapi::Module::RenderMode mode)
{
	Save();

	bool create = gParams.Spawn;
	
	//Check for earth
	OBJHANDLE earth=oapiGetGbodyByName("Earth");
	create &= earth!=NULL; //No earth => no default AU

	VESSELSTATUS2 stat;
	memset(&stat, 0, sizeof(VESSELSTATUS2));
	stat.version=2;
	
	//Check for AscensionUltra vessel already present in the scenario at the proper location
	//as well as apply GenericMessage in order to hand-over Talk function pointer
	bool found = false;
	for (int i=oapiGetVesselCount();i-->0;)
	{
		OBJHANDLE obj=oapiGetVesselByIndex(i);
		if (oapiIsVessel(obj))
		{
			VESSEL *vessel=oapiGetVesselInterface(obj);
			if (strcmp(vessel->GetClassNameA(), CLASSNAME)==0)
			{
				((AscensionUltra *)vessel)->SetTalker(Talk);
				if (!found)
				{
					//Check for proper location, could be another instance of the base!
					vessel->GetStatusEx(&stat);
					found = stat.status==1 && stat.rbody==earth &&	abs(stat.surf_lat-LATITUDE)<0.1 &&
							abs(stat.surf_lng-LONGITUDE)<0.1 &&	abs(stat.surf_hdg-HEADING)<1;
				}
			}
		}
	}

	if (!create || found) return;
	
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