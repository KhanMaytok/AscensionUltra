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
#define ADDRESS_GETDOCKSTATUS 0x004217E0

// ==============================================================
// Global variables

#include <map>

std::map<void *, void *> g_DockLink;
std::map<void *, int> g_Handles;

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

void *_stdcall GetDockStatus(void *vessel, void *dock)
{
	//Do my own GetDockStatus
	std::map<void *, void *>::iterator el=g_DockLink.find(vessel);
	if (el!=g_DockLink.end()) return el->second;
	//Original function content
	return *(void *)((char *)dock+DOCKSTRUCTOFFSET_CURRENTDOCKOBJECT);
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
}

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
int Init(void *handle)
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
	if (memcmp((void *)g_original, (void *)ADDRESS_GETDOCKSTATUS, 10)!=0)
	{
		if (g_Handles.size()==1) return -2;
		return 1;
	}
	WriteCode((void *)ADDRESS_GETDOCKSTATUS, (void *)g_code, 10);
	return 0;
}

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
int Exit(void *handle)
{
	if (g_Handles.find(handle)==g_Handles.end()) return -1;
	g_Handles.erase(handle);
	if (g_Handles.size()>0) return 1;
	if (memcmp((void *)g_code, (void *)ADDRESS_GETDOCKSTATUS, 10)!=0) return -2;
	WriteCode((void *)ADDRESS_GETDOCKSTATUS, (void *)g_original, 10);
	return 0;
}