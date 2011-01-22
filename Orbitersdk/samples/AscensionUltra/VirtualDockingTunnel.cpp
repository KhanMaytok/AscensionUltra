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

// ==============================================================
// Global variables

#include "orbitersdk.h"
#include <map>
#include <queue>

namespace OrbiterExtensions

{

struct EventEntry
{
	double MJD;
	char *event_type;
	char *event;
};

struct EventQueue
{
	CRITICAL_SECTION access;
	int backlog;
	std::queue<EventEntry> queue;
};

std::map<VESSEL *, OBJHANDLE> g_DockLink;
std::map<VESSEL *, EventQueue> g_Events;
std::map<VESSEL *, int> g_Handles;

DWORD g_Hook;
byte g_GetDockStatusOriginal[9]={0x8b,0x44,0x24,0x04,0x8b,0x40,0x48,0xc2,0x04};
byte g_RecordEventOriginal[9]={0x8B,0x09,0x80,0xB9,0x32,0x0D,0x00,0x00,0x00};

//The following array is:
//_asm
//{
//	pop eax;
//	push ecx;
//	push eax;
//	jmp dword ptr [GetDockStatus]; //Dynamically detected address
//	nop;
//}
byte g_Code[9] = {0x58, 0x51, 0x50, 0xff, 0x25, 0, 0, 0, 0};

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
	std::map<VESSEL *, EventQueue>::iterator el=g_Events.find(vessel);
	if (el!=g_Events.end())
	{
	EnterCriticalSection(&el->second.access);
		if (el->second.queue.size()<el->second.backlog)
		{
			EventEntry entry;
			entry.MJD=oapiGetSimMJD();
			entry.event=new char[strlen(event)+1];
			strcpy(entry.event, event);
			entry.event_type=new char[strlen(event_type)+1];
			strcpy(entry.event_type, event_type);
			el->second.queue.push(entry);
		}
	LeaveCriticalSection(&el->second.access);
	}
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

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
__declspec(dllexport) int __cdecl Init(VESSEL *handle, int flags=0)
{
	if (g_Handles.find(handle)!=g_Handles.end()) return -1;
	g_Handles[handle]=1;

	if (memcmp((void *)g_GetDockStatusOriginal, (void *)ADDRESS_GETDOCKSTATUS, 9)!=0 ||
		memcmp((void *)g_RecordEventOriginal, (void *)ADDRESS_RECORDEVENT, 9)!=0)
	{
		if (g_Handles.size()==1) return -2;
		return 1;
	}

	union
	{
		void *pointer;
		byte bytes[4];
		DWORD value;
	} p;

	g_Hook=(DWORD)(void *)GetDockStatus;
	p.pointer=(void *)&g_Hook;	
	for(int i=0;i<4;i++) g_Code[5+i] = p.bytes[i];	
	WriteCode((void *)ADDRESS_GETDOCKSTATUS, (void *)g_Code, 9);

	g_Hook=(DWORD)(void *)RecordEvent;
	p.pointer=(void *)&g_Hook;
	for(int i=0;i<4;i++) g_Code[5+i] = p.bytes[i];	
	WriteCode((void *)ADDRESS_RECORDEVENT, (void *)g_Code, 9);

	return 0;
}

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
__declspec(dllexport) int __cdecl Exit(VESSEL *handle)
{
	if (g_Handles.find(handle)==g_Handles.end()) return -1;
	g_Handles.erase(handle);
	if (g_Handles.size()>0) return 1;
	if (memcmp((void *)g_Code, (void *)ADDRESS_RECORDEVENT, 10)!=0) return -2;
	WriteCode((void *)ADDRESS_GETDOCKSTATUS, (void *)g_GetDockStatusOriginal, 10);
	WriteCode((void *)ADDRESS_RECORDEVENT, (void *)g_RecordEventOriginal, 10);
	return 0;
}

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
}

}