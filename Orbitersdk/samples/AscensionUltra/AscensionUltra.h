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
#include "LeaseLightHangar.h"
#include "LeaseHeavyHangar.h"
#include "LaunchTunnelHangar.h"
#include "AirportHangar.h"
#include "Routes.h"
#include "Person.h"
#include "UMmuSDK.h"
#include <map>

const double EMPTY_MASS    = 11000.0;  // standard configuration

#define TURNAROUNDHANGARS 3
#define LEASELIGHTHANGARS 6
#define LEASEHEAVYHANGARS 3
#define TAXIWAYSUBSECTIONS 32
#define TAXIWAYPATHS 22
#define RUNWAYSUBSECTIONS 92
#define RUNWAYPATHS 16

//Defines for person change API
#define PERSON_EVA		0x00
#define PERSON_DELETE	0xFF
#define PERSON_NAME		0x01
#define PERSON_MISCID	0x02
#define PERSON_AGE		0x04
#define PERSON_PULS		0x08
#define PERSON_WEIGHT	0x10
#define PERSON_LOCATION	0x20
#define ERROR_CHANGE_FAIL	-7

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

	virtual Hangar *GetHangar(int type, int index);
	virtual int GetHangars(int type);
	virtual Routes *GetTaxiways();
	virtual Routes *GetRunways();
	virtual Room *GetControlRoom();
	virtual void SwitchView(Room *room);
	virtual int GetPersons();
	virtual Person GetPerson(int index);
	virtual int ChangePerson(int index, int flags, ...);
	virtual Hangar *GetNearestHangar(int type, VESSEL *vessel);	
	virtual void DockVessel(Room *room, VESSEL *vessel);
	
private:
	void InitSubObjects();
	void MoveGroup(int mesh, VECTOR3 v);	
	Room *GetPersonLocation(int &index);
	Room* GetPersonLocationFromHangar(int &index, Hangar *hangar);

	enum {CAM_GENERIC, CAM_PANELMAIN, CAM_PANELUP, CAM_PANELDN, CAM_VCPILOT, CAM_VCPSNGR1, CAM_VCPSNGR2, CAM_VCPSNGR3, CAM_VCPSNGR4} campos;

	MESHHANDLE meshHangar, meshWindow, meshTopo, meshPlaceHolder, meshLeaseLight, meshLeaseHeavy, meshLaunch, meshLaunchWindow, meshLeaseLightWindow, meshLeaseHeavyWindow;
	TurnAroundHangar turnAround[TURNAROUNDHANGARS];
	LeaseLightHangar leaseLight[LEASELIGHTHANGARS];
	LeaseHeavyHangar leaseHeavy[LEASEHEAVYHANGARS];
	LaunchTunnelHangar launchTunnel;
	AirportHangar airport;
	BeaconArray taxiwaySubsection[TAXIWAYSUBSECTIONS];
	BeaconPath taxiwayPath[TAXIWAYPATHS];
	BeaconArray runwaySubsection[RUNWAYSUBSECTIONS];
	BeaconPath runwayPath[RUNWAYPATHS];
	Routes taxiways;
	Routes runways;
	Room *controlRoom;
	UMMUCREWMANAGMENT *crew;
	int orbiterExtensionsResult;
	float orbiterExtensionsVersion;
	std::map<Room*,VESSEL*> roomVessel;
	std::map<VESSEL*, Room*> vesselRoom;

	int modelidx;                                // flight model index
	VISHANDLE visual;                            // handle to DG visual representation	
	
	int cur_TurnAround, cur_Lease, cur_LaunchTunnel, cur_Airport;

	//DEBUG

	int cur_Path;
	int cur_Section;

	bool coords;

	double disx, disy, disz, stp;
	int mnr;

};

typedef struct {
	HINSTANCE hDLL;
	HFONT font[2];
	DWORD col[4];
	HBRUSH brush[4];
	HPEN pen[2];
} GDIParams;

#endif // !__ASCENSIONULTRA_H