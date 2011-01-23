// ==============================================================
//                ORBITER MODULE: VIRTUALDOCKINGTUNNEL
//                  
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VirtualDockingTunnel.cpp
// Implementation of virtual docking functions.
// ==============================================================

#define DOCKSTRUCTOFFSET_CURRENTDOCKOBJECT 0x48
#define ADDRESS_GETDOCKSTATUS 0x00476210
#define VESSELSTRUCTOFFSET_RECORDING 0x0D32
#define ADDRESS_RECORDEVENT 0x00476FA0
#define ADDRESS_INTERNALRECORDEVENT 0x00465FB0
#define HOOKS 2

// ==============================================================
// Global variables

#include "orbitersdk.h"
#include <map>
#include <queue>

namespace OrbiterExtensions

{

struct EventEntry
{
	double mjd;
	char *event_type;
	char *event;
};

struct EventQueue
{	
	int backlog;
	std::queue<EventEntry> queue;
};

std::map<VESSEL *, OBJHANDLE> g_DockLink;
std::map<VESSEL *, EventQueue> g_Events;
std::map<int, std::map<VESSEL *, int> > g_Handles;
CRITICAL_SECTION g_EventsAccess;

DWORD g_Hook;

void *g_Addresses[HOOKS]=
{
	(void *)ADDRESS_GETDOCKSTATUS,
	(void *)ADDRESS_RECORDEVENT
};

byte g_Original[HOOKS][9]=
{
	{0x8b,0x44,0x24,0x04,0x8b,0x40,0x48,0xc2,0x04},
	{0x8B,0x09,0x80,0xB9,0x32,0x0D,0x00,0x00,0x00}
};

//The following arrays are:
//_asm
//{
//	pop eax                        //reorder stack to inject ECX (object pointer)
//	push ecx
//	push eax
//	jmp dword ptr [HOOKFUNCTION]; //jump to dynamically detected address
//}
byte g_Code[HOOKS][9]=
{
	{0x58, 0x51, 0x50, 0xff, 0x25, 0, 0, 0, 0},
	{0x58, 0x51, 0x50, 0xff, 0x25, 0, 0, 0, 0}
};

OBJHANDLE _stdcall GetDockStatus(VESSEL *vessel, DOCKHANDLE dock)
{
	//Do my own GetDockStatus
	std::map<VESSEL *, OBJHANDLE>::iterator el=g_DockLink.find(vessel);
	if (el!=g_DockLink.end()) return el->second;
	//Original function content
	return *(OBJHANDLE *)(void *)((char *)dock+DOCKSTRUCTOFFSET_CURRENTDOCKOBJECT);
}

void _stdcall RecordEvent(VESSEL *vessel, const char *event_type, const char *event)
{
	//Do my own RecordEvent
	EnterCriticalSection(&g_EventsAccess);
	std::map<VESSEL *, EventQueue>::iterator el=g_Events.find(vessel);
	if (el!=g_Events.end())
	{	
		EventEntry entry;
		entry.mjd=oapiGetSimMJD();
		entry.event=new char[strlen(event)+1];
		strcpy(entry.event, event);
		entry.event_type=new char[strlen(event_type)+1];
		strcpy(entry.event_type, event_type);		
		while(el->second.queue.size()>=el->second.backlog)
		{
			delete [] el->second.queue.front().event_type;
			delete [] el->second.queue.front().event;
			el->second.queue.pop();
		}		
		el->second.queue.push(entry);	
	}
	LeaveCriticalSection(&g_EventsAccess);
	//Original function content
	if (*(*(char **)vessel+VESSELSTRUCTOFFSET_RECORDING)==0) return;
	_asm
	{
		push event
		push event_type
		mov eax, vessel
		mov ecx,dword ptr ds:[eax]
		mov eax,ADDRESS_INTERNALRECORDEVENT
		call eax
	}
}

int WriteCode(void *address, void *code, DWORD len)
{
	//Get process information
	HANDLE hSelf = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
	MEMORY_BASIC_INFORMATION mbi;

	//Open up page of linked address
	if(VirtualQueryEx(hSelf, (LPVOID)address, &mbi, sizeof(mbi)) != sizeof(mbi)) return -1;
	PVOID pvRgnBaseAddress = mbi.BaseAddress;
	DWORD dwOldProtect1, dwOldProtect2, dwFake;
	if(!::VirtualProtectEx(hSelf, pvRgnBaseAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect1)) return -2;
	BOOL bStridePage = FALSE;
	LPBYTE lpByte = (LPBYTE)pvRgnBaseAddress;
	lpByte += 4096;
	if((DWORD)lpByte < (DWORD)address + 4) bStridePage = TRUE;
	PVOID pvRgnBaseAddress2 = (LPVOID)lpByte;
	if(bStridePage)
	   if(!::VirtualProtectEx(hSelf, pvRgnBaseAddress2, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect2))
	   {
		   ::VirtualProtectEx(hSelf, pvRgnBaseAddress, 4, dwOldProtect1, &dwFake);
			return -3;
	   }
	
	//Write code
	memcpy(address, code, len);		

	//Lock vtables again
	::VirtualProtectEx(hSelf, pvRgnBaseAddress, 4, dwOldProtect1, &dwFake);
	if(bStridePage) ::VirtualProtectEx(hSelf, pvRgnBaseAddress2, 4, dwOldProtect2, &dwFake);

	return 0;
}

extern "C"
{

// Initializes specified hook. Default is docking status hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//        -3 if hook is unknown
__declspec(dllexport) int __cdecl InitHook(VESSEL *handle, int hook)
{
	if (hook>=HOOKS || hook<0) return -3;

	std::map<int, std::map<VESSEL *, int> >::iterator list=g_Handles.find(hook);
	if (list!=g_Handles.end())
	{
		//Hook already established by OrbiterExtensions		
		if (list->second.find(handle)!=list->second.end()) return -1;
		list->second[handle]=0;
		return 1;
	}
	g_Handles[hook][handle]=0;

	union
	{
		void *pointer;
		byte bytes[4];
		DWORD value;
	} p;

	switch(hook)
	{
	case 0: g_Hook=(DWORD)(void *)GetDockStatus; break;
	case 1:
		InitializeCriticalSection(&g_EventsAccess);
		g_Hook=(DWORD)(void *)RecordEvent;
		break;	
	}
	p.pointer=(void *)&g_Hook;

	if (memcmp((void *)g_Original[hook], g_Addresses[hook], 9)!=0) return -2;
	for(int i=0;i<4;i++) g_Code[hook][5+i] = p.bytes[i];	
	WriteCode(g_Addresses[hook], (void *)g_Code[hook], 9);

	return 0;
}
__declspec(dllexport) int __cdecl Init(VESSEL *handle){return InitHook(handle, 0);}

// Removes the specified hook. Default is docking status hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
//        -3 if hook is unknown
__declspec(dllexport) int __cdecl ExitHook(VESSEL *handle, int hook)
{
	if (hook>=HOOKS || hook<0) return -3;
	std::map<int, std::map<VESSEL *, int> >::iterator list=g_Handles.find(hook);
	if (list==g_Handles.end()) return -1;
	if (list->second.find(handle)==list->second.end()) return -1;
	list->second.erase(handle);	
	if (list->second.size()>0) return 1;
	if (memcmp((void *)g_Code[hook], g_Addresses[hook], 9)!=0) return -2;
	WriteCode(g_Addresses[hook], (void *)g_Original[hook], 9);
	switch(hook)
	{
	case 1:
		DeleteCriticalSection(&g_EventsAccess);		
		break;
	}
	return 0;
}
__declspec(dllexport) int __cdecl Exit(VESSEL *handle){return ExitHook(handle, 0);}

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
__declspec(dllexport) int __cdecl SetDockState(VESSEL *handle, OBJHANDLE obj)
{
	if (obj==NULL)
	{
		if (g_DockLink.find(handle)==g_DockLink.end()) return -1;
		g_DockLink.erase(handle);
		return 0;
	}
	g_DockLink[handle]=obj;
	return 0;
}

__declspec(dllexport) float __cdecl GetVersion(){return (float)0.2;}

//*** Version 0.2 functions

// Register the given vessel for RecordEvent hooking and/or sets the specified backlog.
// Returns 0
__declspec(dllexport) int __cdecl RecordVesselEvents(VESSEL *handle, int backlog)
{
	EnterCriticalSection(&g_EventsAccess);
	std::map<VESSEL *, EventQueue>::iterator el=g_Events.find(handle);	
	if (el==g_Events.end())
	{
		g_Events[handle].backlog=backlog;
		LeaveCriticalSection(&g_EventsAccess);
		return 0;		
	}
	el->second.backlog=backlog;
	LeaveCriticalSection(&g_EventsAccess);
	return 0;
}

// Unregister the given vessel for RecordEvent hooking and delete the queue.
// Returns 0 if successful
//        -1 if vessel was not registered
__declspec(dllexport) int __cdecl DeleteVesselEvents(VESSEL *handle)
{
	EnterCriticalSection(&g_EventsAccess);
	std::map<VESSEL *, EventQueue>::iterator el=g_Events.find(handle);	
	if (el!=g_Events.end())
	{
		LeaveCriticalSection(&g_EventsAccess);
		return -1;
	}
	while (!el->second.queue.empty())
	{
		delete [] el->second.queue.front().event_type;
		delete [] el->second.queue.front().event;
		el->second.queue.pop();
	}
	g_Events.erase(el);
	LeaveCriticalSection(&g_EventsAccess);
	return 0;
}

// Gets the specified amount of recorded events for the given vessel.
// The arrays needs to be initialized with the appropriate count.
// Returns 0 if queue was empty
//        >0 number of read events
//        -1 if vessel was not registered
__declspec(dllexport) int __cdecl GetVesselEvents(VESSEL *handle, double *mjds, char **event_types, char **events, int size)
{
	EnterCriticalSection(&g_EventsAccess);
	std::map<VESSEL *, EventQueue>::iterator el=g_Events.find(handle);	
	if (el!=g_Events.end())
	{
		LeaveCriticalSection(&g_EventsAccess);
		return -1;
	}
	int i=0;
	while (!el->second.queue.empty() && i<size)
	{
		mjds[i]=el->second.queue.front().mjd;
		event_types[i]=el->second.queue.front().event_type;
		events[i]=el->second.queue.front().event;
		el->second.queue.pop();
		i++;
	}	
	LeaveCriticalSection(&g_EventsAccess);
	return i;
}

}
}
