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

// ==============================================================
// Global variables

#include "orbitersdk.h"
#include <map>

namespace OrbiterExtensions

{

std::map<VESSEL *, OBJHANDLE> g_DockLink;
std::map<VESSEL *, int> g_Handles;

DWORD g_Hook;
byte g_original[10]={0x8b,0x44,0x24,0x04,0x8b,0x40,0x48,0xc2,0x04,0x00};
//The following array is:
//_asm
//{
//	pop eax;
//	push ecx;
//	push eax;
//	jmp dword ptr [GetDockStatus]; //Dynamically detected address
//	nop;
//}
byte g_code[10] = {0x58, 0x51, 0x50, 0xff, 0x25, 0, 0, 0, 0, 0x90};

OBJHANDLE _stdcall GetDockStatus(VESSEL *vessel, DOCKHANDLE dock)
{
	//Do my own GetDockStatus
	std::map<VESSEL *, OBJHANDLE>::iterator el=g_DockLink.find(vessel);
	if (el!=g_DockLink.end()) return el->second;
	//Original function content
	return *(OBJHANDLE *)(void *)((char *)dock+DOCKSTRUCTOFFSET_CURRENTDOCKOBJECT);
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
__declspec(dllexport) int __cdecl Init(VESSEL *handle)
{
	if (g_Handles.find(handle)!=g_Handles.end()) return -1;
	g_Handles[handle]=1;
	union
	{
		void *pointer;
		byte bytes[4];
		DWORD value;
	} p;
	g_Hook=(DWORD)(void *)GetDockStatus;
	p.pointer=(void *)&g_Hook;	
	for(int i=0;i<4;i++) g_code[5+i] = p.bytes[i];

	union
	{
		OBJHANDLE (__thiscall VESSEL::* function )(DOCKHANDLE) const;
		void *address;
	} pointer;
	pointer.function=&VESSEL::GetDockStatus;
	if (memcmp((void *)g_original, pointer.address, 10)!=0)
	{
		if (g_Handles.size()==1) return -2;
		return 1;
	}
	WriteCode(pointer.address, (void *)g_code, 10);
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
	union
	{
		OBJHANDLE (__thiscall VESSEL::* function )(DOCKHANDLE) const;
		void *address;
	} pointer;
	pointer.function=&VESSEL::GetDockStatus;
	if (memcmp((void *)g_code, pointer.address, 10)!=0) return -2;
	WriteCode(pointer.address, (void *)g_original, 10);
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

__declspec(dllexport) float __cdecl GetVersion(){return (float)0.1;}
}

}