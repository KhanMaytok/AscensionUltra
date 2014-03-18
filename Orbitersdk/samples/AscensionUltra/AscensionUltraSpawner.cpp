#pragma unmanaged
#include "AscensionUltraSpawner.h"
#include "AscensionUltraConfigurator.h"
#include "AscensionUltra.h"

extern gParamsType gParams;

void ShiftSlots(int slots, TalkerEntry *entry)
{
	//Delete last slot text
	if (gParams.slots[--slots].text) delete [] gParams.slots[slots].text;
	gParams.slots[slots].text=NULL;
	//Shift slots
	for(int i=slots;i>0;i--)
	{
		NOTEHANDLE h=gParams.slots[i].handle;
		oapiAnnotationSetSize(h, gParams.slots[i].size=gParams.slots[i-1].size);
		oapiAnnotationSetColour(h, gParams.slots[i].color=gParams.slots[i-1].color);
		oapiAnnotationSetText(h, gParams.slots[i].text=gParams.slots[i-1].text);
	}
	NOTEHANDLE h=gParams.slots[0].handle;							
	oapiAnnotationSetSize(h, gParams.slots[0].size=entry->size);
	oapiAnnotationSetColour(h, gParams.slots[0].color=entry->color);
	oapiAnnotationSetText(h, gParams.slots[0].text=entry->display);
}

DWORD WINAPI TalkerThread(LPVOID params)
{
	ISpVoice *ATC;
	CoInitialize(NULL);
	if(SUCCEEDED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&ATC)))
	{
		ResetEvent(gParams.stopped);
		while(gParams.active)
		{
			DWORD wait=WaitForSingleObject(gParams.event, gParams.IdleRotate<=0?INFINITE:(DWORD)(gParams.IdleRotate*1000));
			bool go=gParams.active;
			while(go && gParams.active)
			{
				OBJHANDLE focus=oapiGetFocusObject();

				EnterCriticalSection(&gParams.lock);
					TalkerEntry *entry;
					int slots=gParams.slots.size();
					if (gParams.messages.find(focus)==gParams.messages.end())
					{
						if (wait==WAIT_TIMEOUT && slots>0 && gParams.slots[0].handle)
						{
							entry=new TalkerEntry;
							entry->display=NULL;
							entry->color=_V(0,0,0);
							entry->size=1;
							ShiftSlots(slots, entry);
							delete entry;
						}
				LeaveCriticalSection(&gParams.lock);
						break; //Note: this means we need a signaling if queue OR focus object is changing.
					}
					entry=gParams.messages[focus].front();
					gParams.messages[focus].pop();					
					if (entry->valid)
					{						
						if (slots>0 && gParams.slots[0].handle)
						{
							ShiftSlots(slots, entry);							
							entry->display=NULL;
						}
						entry->valid=false; //Setting this before releasing the lock, so queue manipulators only delete valid entries.
				LeaveCriticalSection(&gParams.lock);
						ATC->Speak(entry->message, NULL, NULL);
						if (entry->message) delete [] entry->message;
						if (entry->display) delete [] entry->display;
				EnterCriticalSection(&gParams.lock);
					}
					else delete entry; //Here is the only place where entries get deleted in simulation.
					if (!(go=!gParams.messages[focus].empty())) gParams.messages.erase(focus); //Check for empty queue, as somebody might have added while speaking.
					if (!go) go=oapiGetFocusObject()!=focus; //This ensures loop entry if focus change happened while talking
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

void Talk(const TalkerEntry &input, const OBJHANDLE sender, const OBJHANDLE receiver)
{
	//Copy the input across DLL boundaries
	TalkerEntry *entry=new TalkerEntry(input);
	entry->valid=true;
	entry->message=NULL;
	entry->display=NULL;
	if (input.message) wcscpy((WCHAR *)(entry->message=new WCHAR[wcslen(input.message)+1]), input.message);
	if (input.display) strcpy(entry->display=new char[strlen(input.display)+1], input.display);

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
	if (!oapiReadItem_float(f, IDLEROTATE, gParams.IdleRotate)) gParams.IdleRotate=-1; //Idle rotation time default to off
	int k=strlen(ATCSLOT);
	char item[80], buf[256];
	strcpy(item, ATCSLOT);
	for(int i=0;;i++)
	{
		_itoa(i, item+k, 10);
		if (!oapiReadItem_string(f, item, buf)) break;
		Annotation slot;
		slot.handle=NULL;
		sscanf(buf, "%lf,%lf,%lf,%lf", &slot.x1, &slot.y1, &slot.x2, &slot.y2);				
		gParams.slots.push_back(slot);
	}
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

	//Create annotation slots for ATC display
	EnterCriticalSection(&gParams.lock);
		for(std::vector<Annotation>::iterator i=gParams.slots.begin();i!=gParams.slots.end();i++)
		{
			i->handle=oapiCreateAnnotation(true, 1, _V(1,1,1));
			i->text=NULL;
			oapiAnnotationSetPos(i->handle, i->x1, i->y1, i->x2, i->y2);
		}
	LeaveCriticalSection(&gParams.lock);

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

void AscensionUltraSpawner::clbkSimulationEnd()
{
	//Remove annotation slots
	EnterCriticalSection(&gParams.lock);
		for(std::vector<Annotation>::iterator i=gParams.slots.begin();i!=gParams.slots.end();i++)
		{
			if (i->handle) oapiDelAnnotation(i->handle);
			i->handle=NULL;
			if (i->text) delete [] i->text;
			i->text=NULL;
		}
	LeaveCriticalSection(&gParams.lock);
}

void AscensionUltraSpawner::Save()
{
	FILEHANDLE f=oapiOpenFile(ORBITERCONFIG, FILE_APP);
	oapiWriteLine(f, "\n; === Base Vessel Configuration ===");
	oapiWriteItem_bool(f, SCNSAVE, gParams.SCNSave);
	oapiWriteItem_bool(f, RECSAVE, gParams.RecSave);
	oapiWriteItem_bool(f, SPAWN, gParams.Spawn);
	oapiWriteItem_bool(f, RESET, gParams.Reset);
	oapiWriteItem_float(f, IDLEROTATE, gParams.IdleRotate);
	//Save should never be called while slots are re-defined, so no lock is necessary here
	int j=0;
	int k=strlen(ATCSLOT);
	char item[80], buf[256];
	strcpy(item, ATCSLOT);
	for(std::vector<Annotation>::iterator i=gParams.slots.begin();i!=gParams.slots.end();i++)
	{
		sprintf(item+k, "%d", j++);
		sprintf(buf, "%lf, %lf, %lf, %lf", i->x1, i->y1, i->x2, i->y2);
		oapiWriteItem_string(f, item, buf);
	}
	oapiCloseFile(f, FILE_APP);
}