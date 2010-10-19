// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltra.h
// Class interface of Ascension Ultra vessel.
// ==============================================================

#ifndef __ASCENSIONULTRA_H
#define __ASCENSIONULTRA_H

#define STRICT

#include "orbitersdk.h"
#include "resource.h"
#include "TurnAroundHangar.h"
#include "LightStorageHangar.h"
#include "LaunchTunnelHangar.h"
#include "Routes.h"
#include "Person.h"
#include "UMmuSDK.h"

const double EMPTY_MASS    = 11000.0;  // standard configuration

#define TURNAROUNDHANGARS 5
#define LIGHTSTORAGEHANGARS 12
#define TAXIWAYSUBSECTIONS 32
#define TAXIWAYPATHS 22
#define RUNWAYSUBSECTIONS 56
#define RUNWAYPATHS 10

#define PERSON_EVA		0x00
#define PERSON_DELETE	0xFF
#define PERSON_NAME		0x01
#define PERSON_MISCID	0x02
#define PERSON_AGE		0x04
#define PERSON_PULS		0x08
#define PERSON_WEIGHT	0x10

class AscensionUltra: public VESSEL2 {
public:
	AscensionUltra (OBJHANDLE hObj, int fmodel);
	~AscensionUltra ();
	virtual double GetVersion();
	void DefineAnimations ();
	void DrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
	
	// Overloaded callback functions
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkLoadStateEx (FILEHANDLE scn, void *vs);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC);
	int  clbkConsumeBufferedKey (DWORD key, bool down, char *kstate);	
	bool clbkLoadGenericCockpit ();

	virtual Hangar *GetHangar(HangarType type, int index);
	virtual int GetHangars(HangarType type);
	virtual Routes *GetTaxiways();
	virtual Routes *GetRunways();
	virtual Room *GetControlRoom();
	virtual void SwitchView(Room *room);
	virtual int GetPersons();
	virtual Person GetPerson(int index);
	virtual int ChangePerson(int index, int flags, ...);

private:
	void InitSubObjects();
	void MoveGroup(int mesh, VECTOR3 v);
	void RotateGroup(int mesh, float angle, VECTOR3 v, VECTOR3 ref);
	Room *GetPersonLocation(int &index);

	enum {CAM_GENERIC, CAM_PANELMAIN, CAM_PANELUP, CAM_PANELDN, CAM_VCPILOT, CAM_VCPSNGR1, CAM_VCPSNGR2, CAM_VCPSNGR3, CAM_VCPSNGR4} campos;

	MESHHANDLE meshHangar, meshWindow, meshTopo, meshPlaceHolder, meshLightStorage, meshLaunch, meshLaunchWindow;
	TurnAroundHangar turnAround[TURNAROUNDHANGARS];
	LightStorageHangar lightStorage[LIGHTSTORAGEHANGARS];
	LaunchTunnelHangar launchTunnel;
	BeaconArray taxiwaySubsection[TAXIWAYSUBSECTIONS];
	BeaconPath taxiwayPath[TAXIWAYPATHS];
	BeaconArray runwaySubsection[RUNWAYSUBSECTIONS];
	BeaconPath runwayPath[RUNWAYPATHS];
	Routes taxiways;
	Routes runways;
	Room *controlRoom;
	Room entrance;

	int modelidx;                                // flight model index
	VISHANDLE visual;                            // handle to DG visual representation	
	
	int cur_TurnAround, cur_LightStorage, cur_LaunchTunnel;

	//DEBUG

	int cur_Path;
	int cur_Section;

	bool coords;

};

typedef struct {
	HINSTANCE hDLL;
	HFONT font[2];
	DWORD col[4];
	HBRUSH brush[4];
	HPEN pen[2];
} GDIParams;

#endif // !__ASCENSIONULTRA_H