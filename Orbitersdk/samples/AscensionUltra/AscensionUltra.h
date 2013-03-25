// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltra.h
// Class interface of Ascension Ultra vessel.
// ==============================================================

#pragma once
#include "orbitersdk.h"
#include "Module.h"
#include "TurnAroundHangar.h"
#include "LeaseLightHangar.h"
#include "LeaseHeavyHangar.h"
#include "LaunchTunnelHangar.h"
#include "VerticalLaunchHangar.h"
#include "SmallVerticalLaunchHangar.h"
#include "DocksHangar.h"
#include "AirportHangar.h"
#include "Routes.h"
#include "Person.h"
#include "Tracker.h"
#include "Taxiways.h"
#include "Runways.h"
#include "Group.h"
#include "UMmuSDK.h"
#include <map>
#include <vector>
#include "sapi.h"

const double EMPTY_MASS    = 11000.0;  // standard configuration

#define TURNAROUNDHANGARS 3
#define LEASELIGHTHANGARS 6
#define LEASEHEAVYHANGARS 3
#define DRADARS 2
#define LAUNCHTUNNELS 1
#define VERTICALLAUNCHES 2
#define DOCKYARDS 1
#define AIRPORTS 1
#define STATICMESHES 4
#define STATICTRANSPARENCIES 1

//Defines for person change API
#define PERSON_EVA		0x00
#define PERSON_DELETE	0xFF
#define PERSON_NAME		0x01
#define PERSON_MISCID	0x02
#define PERSON_AGE		0x04
#define PERSON_PULS		0x08
#define PERSON_WEIGHT	0x10
#define PERSON_LOCATION	0x20
#define PERSON_FILTER   0x80
#define ERROR_CHANGE_FAIL	-7

namespace BaseVessel
{
	namespace EventHandler
	{
		enum SourceType
		{
			Checklist,
		};

		struct Arguments
		{
			int Event;
			enum SourceType SourceType;
			void *Source;
		};
	}
}	

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
	virtual Taxiways *GetTaxiways();
	virtual Runways *GetRunways();
	virtual Room *GetControlRoom();
	virtual void SwitchView(Room *room);
	virtual int GetPersons();
	virtual Person GetPerson(int index, Room *room=NULL);
	virtual int ChangePerson(int index, int flags, ...);
	virtual Hangar *GetNearestHangar(int type, VESSEL *vessel);	
	virtual void DockVessel(Room *room, VESSEL *vessel);
	virtual int GetChecklists(int type, VESSEL *vessel=NULL);
	virtual Checklist *GetChecklist(int type, int index, VESSEL *vessel=NULL);
	virtual int GetChecklists(Hangar *hangar, VESSEL *vessel);
	virtual Checklist *GetChecklist(Hangar *hangar, int index, VESSEL *vessel);
	virtual void RegisterEventHandler(void (*handler)(BaseVessel::EventHandler::Arguments args, void *context), void *context);
	virtual void UnregisterEventHandler(void (*handler)(BaseVessel::EventHandler::Arguments args, void *context), void *context);

	void SendEvent(BaseVessel::EventHandler::Arguments args);

	void Talk(LPCWSTR message, ...);
	bool Talking(void);

private:
	void InitSubObjects();
	void MoveGroup(int mesh, VECTOR3 v);	
	Room *GetPersonLocation(int &index);
	Room* GetPersonLocationFromHangar(int &index, Hangar *hangar);
	
	enum {CAM_GENERIC, CAM_PANELMAIN, CAM_PANELUP, CAM_PANELDN, CAM_VCPILOT, CAM_VCPSNGR1, CAM_VCPSNGR2, CAM_VCPSNGR3, CAM_VCPSNGR4} campos;

	MESHHANDLE meshHangar, meshWindow, meshTopo, meshPlaceHolder, meshLeaseLight, meshLeaseHeavy;
	MESHHANDLE meshLaunch, meshLaunchWindow, meshLeaseLightWindow, meshLeaseHeavyWindow, meshVertical, meshVerticalWindow, meshDRadar;
	MESHHANDLE meshVerticalSmall, meshVerticalSmallWindow, meshDocks, meshAirport, meshAirportWindow;
	TurnAroundHangar turnAround[TURNAROUNDHANGARS];
	LeaseLightHangar leaseLight[LEASELIGHTHANGARS];
	LeaseHeavyHangar leaseHeavy[LEASEHEAVYHANGARS];
	VerticalLaunchHangar vertical;
	SmallVerticalLaunchHangar verticalSmall;
	LaunchTunnelHangar launchTunnel;
	AirportHangar airport;
	DocksHangar docks;
	Taxiways taxiways;
	Runways runways;
	Room *controlRoom;
	Tracker dradar[DRADARS];
	UMMUCREWMANAGMENT *crew;
	int orbiterExtensionsResult;
	float orbiterExtensionsVersion;
	std::map<Room*,VESSEL*> roomVessel;
	std::map<VESSEL*, Room*> vesselRoom;
	std::map<void*, void (*)(BaseVessel::EventHandler::Arguments args, void *context)> eventHandlers;

	int modelidx;                                // flight model index
	VISHANDLE visual;                            // handle to DG visual representation	
	
	int cur_TurnAround, cur_Lease, cur_LaunchTunnel, cur_Airport, cur_Vertical, cur_DopplerRadar, cur_VerticalSmall, cur_Docks;
	bool oldCameraMode;

	ISpVoice *ATC;

	char *ini;
	bool scnsave, recsave;

	//DEBUG

	int cur_Path;
	int cur_Section;

	bool coords;

	double disx, disy, disz, stp;
	int mnr;

};