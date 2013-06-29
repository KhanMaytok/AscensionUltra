// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltra.cpp
// Class implementation of Ascension Ultra vessel.
// ==============================================================

#include "AscensionUltra.h"
#include "Module.h"
#include <stdio.h>
#include <math.h>
#include "KeyboardFilter.h"
#include "VirtualDockingTunnel.h"

#define TA1MATRIXOFFSET _V(266,0,0)
#define TA1OFFSET _V(-1710,0,580)
#define OFFSET _V(2700,0,-950)
#define _V_(x,y,z) _V(-x,y,z)+OFFSET
#define LL1OFFSET _V(-2330,0,671)
#define LL1MATRIXOFFSET _V(70,0,0)
#define LH1OFFSET _V(-2315,0,577.5)
#define LH1MATRIXOFFSET _V(160,0,0)
#define MAXSTATICRUNWAYLINES 14
#define STATICRUNWAYLINESSPLIT 7
#define RUNWAYENDPOINTS 15
#define LFMCOFFSET _V(-1463.55,-0.19,1260)
#define VLC1OFFSET _V(-1866,-0.28,2560.5)
#define VLC2OFFSET _V(-2086,-0.28,2560.5)
#define DRADAROFFSET _V(-895,0,970)
#define DRADARMATRIXOFFSET _V(-4495,0,0)
#define DRADARPIVOT 10
#define DOCKSOFFSET _V(-130.5,0,645)
#define AIRPORTOFFSET _V(-4653,0,605)
#define SPAWN		"BaseAutoSpawn"
#define RESET		"BaseFastReset"
#define SCNSAVE		"BaseScenarioSave"
#define RECSAVE		"BaseRecorderSave"
#define CONFIGDIRTAG   "ConfigDir"
#define CONFIGDIRDEFAULT   ".\\Config\\"
#define CONFIGPATH    "%s%s.cfg"
#define CONFIG2PATH   "%sVessels\\%s.cfg"
#define ORBITERCONFIG "Orbiter.cfg"
#define CONFIGEXCEPTION "abort: neither vessel configuration \"%s%s.cfg\" nor \"%sVessels\\%s.cfg\" was found!"  

// Static methods

void OnLeaseHeavyLoad(MESHHANDLE hMesh, bool firstload)
{
	if (firstload) RotateMesh(hMesh, (float)PI, _V(0,1,0), _V(0,0,0));
}

// Constructor
AscensionUltra::AscensionUltra (OBJHANDLE hObj, int fmodel)
: VESSEL2 (hObj, fmodel)
{
	modelidx = (fmodel ? 1 : 0);
	visual            = NULL;
	campos            = CAM_GENERIC;

	cur_TurnAround=-1;
	cur_Lease=-1;
	cur_Airport=-1;
	cur_Docks=-1;
	cur_LaunchTunnel=-1;
	cur_Vertical=-1;

	cur_Path=0;
	cur_Section=0;

	oldCameraMode=false;

	coords=false;

	ini=NULL;

	talker=NULL;

	voiceVessel.Definition=NULL;
	voiceATC.Definition=NULL;
}

// --------------------------------------------------------------
// Destructor
// --------------------------------------------------------------
AscensionUltra::~AscensionUltra ()
{	
	//Remove dynamic INI parameters
	delete [] ini;
	OrbiterExtensions::Exit(this);
}

double AscensionUltra::GetVersion(){return 1.0;}

void AscensionUltra::InitSubObjects()
{
	int i;

	ReadATCParameters(voiceVessel, ini, "TALKERVESSEL");
	ReadATCParameters(voiceATC, ini, "TALKERATC");

	char name[40]="Turn-around Hangar #x";
	int k=strlen(name)-1;
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		name[k]=0x31+i;
		turnAround[i].Init(this, ini, name, i+STATICMESHES, "HANGAR", i);
	}

	strcpy(name, "Lease Hangar #x ");
	k=strlen(name)-2;
	for(i=0;i<LEASELIGHTHANGARS;i++)
	{
		name[i>8?k+1:k]=0x30+((i+1) % 10);
		if (i>8) name[k]=0x31;
		leaseLight[i].Init(this, ini, name, i+STATICMESHES+TURNAROUNDHANGARS, "LIGHTLEASE", i, "LEASE");
	}

	for(;i<LEASELIGHTHANGARS+LEASEHEAVYHANGARS;i++)
	{
		name[i>8?k+1:k]=0x30+((i+1) % 10);
		if (i>8) name[k]=0x31;
		leaseHeavy[i-LEASELIGHTHANGARS].Init(this, ini, name, i+STATICMESHES+TURNAROUNDHANGARS, "HEAVYLEASE", i, "LEASE");
	}

	strcpy(name, "Winged Launch Facility");
	launchTunnel.Init(this, ini, name, STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS, "LAUNCHTUNNEL", -1);

	strcpy(name, "Vertical Launch Facility 1");
	vertical.Init(this, ini, name, STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS, "VERTICALLAUNCH", 0);

	strcpy(name, "Vertical Launch Facility 2");
	verticalSmall.Init(this, ini, name, STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+1, "VERTICALLAUNCH", 1);

	/* Tracker definition */
	//0-1,7-10 groups are dishes, 4 is static plate, 2-3/5-6 is rotation stand
	static UINT RotGrp[10] = {2,3,5,6,0,1,7,8,9,10};
	strcpy(name, "Doppler Radar #x");
	k=strlen(name)-1;
	for(i=0;i<DRADARS;i++)
	{
		name[k]=0x31+i;
		dradar[i].Init(this, ini, name,
			new MGROUP_ROTATE(i+STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES, RotGrp+0, 4, _V(0,0,0), _V(0,1,0), (float)(-360*RAD)),
			new MGROUP_ROTATE(i+STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES, RotGrp+4, 6, _V(0,DRADARPIVOT,0), _V(1,0,0), (float)(90*RAD)),
			90*RAD, "DRADAR", i);
	}

	strcpy(name, "Dockyard");
	docks.Init(this, ini, name, STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES+DRADARS, "DOCKYARD", -1);

	strcpy(name, "Airport");
	airport.Init(this, ini, name, STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES+DRADARS+DOCKYARDS, "AIRPORT", -1);
	crew=airport.GetEntrance()->GetCrew();

	controlRoom=launchTunnel.GetRoom(1); //Tower

	//Initialize Orbiter extensions
	orbiterExtensionsResult=OrbiterExtensions::Init(this);

	if ((orbiterExtensionsVersion=OrbiterExtensions::GetVersion())<0) orbiterExtensionsVersion=0.0;

	taxiways.Init(this, ini, "TAXIWAYS", OFFSET);

	runways.Init(this, ini, "RUNWAYS", OFFSET);

	//DEBUG
	disx=0.0;
	disy=0.0;
	disz=0.0;
	stp=10.0;
	mnr=0;
}
// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void AscensionUltra::DefineAnimations ()
{
	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].DefineAnimations();
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].DefineAnimations();
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].DefineAnimations();
	launchTunnel.DefineAnimations();
	for(int i=0;i<DRADARS;i++) dradar[i].DefineAnimations();
	vertical.DefineAnimations();
	verticalSmall.DefineAnimations();
	docks.DefineAnimations();
	airport.DefineAnimations();
}

void AscensionUltra::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
	// draw the default HUD
	VESSEL2::clbkDrawHUD (mode, hps, hDC);

	// TODO: draw vessel status	
}

// Set vessel class parameters
void AscensionUltra::clbkSetClassCaps (FILEHANDLE cfg)
{
	// Get INI file location
	char *className=GetClassName(), *cn, configDir[1024];
	strcpy(cn=new char[strlen(className)+1], className);
	_strlwr(cn);
	
	//configuration file is also INI file, read orbiter.cfg first to get custom config paths
	FILEHANDLE f=oapiOpenFile(ORBITERCONFIG, FILE_IN);
	if (!oapiReadItem_string(f, CONFIGDIRTAG, configDir)) strcpy(configDir, CONFIGDIRDEFAULT);
	if (!oapiReadItem_bool(f, SCNSAVE, scnsave)) scnsave=true; //Scenario saving default is true
	oapiCloseFile(f, FILE_IN);
	//try standard path
	ini=new char[strlen(cn)+strlen(configDir)+strlen(CONFIGPATH)];
	sprintf(ini, CONFIGPATH, configDir, cn);
	FILE *test=fopen(ini, "r");
	if (test==NULL)
	{
		//try alternate vessel config path
		delete [] ini;
		ini=new char[strlen(cn)+strlen(configDir)+strlen(CONFIG2PATH)];
		sprintf(ini, CONFIG2PATH, configDir, cn);
		test=fopen(ini, "r");
		if (test==NULL)
		{
			//vessel configuration not found - this is a severe error, as it should be there
			delete [] ini;
			ini=new char[strlen(cn)*2+strlen(configDir)*2+strlen(CONFIGEXCEPTION)];
			sprintf(ini, CONFIGEXCEPTION, configDir, cn, configDir, cn);
			oapiWriteLog(ini);
			exit(-1); //Bail out
		}
	}
	fclose(test); //We don't really need the handle
	delete [] cn;

	// *************** physical parameters **********************

	VESSEL2::SetEmptyMass (EMPTY_MASS);
	VECTOR3 r[2] = {{0,0,6}, {0,0,-4}};
	SetSize (5000);
	SetVisibilityLimit (7.5e-4, 1.5e-3);
	SetAlbedoRGB (_V(0.13,0.20,0.13));
	SetTouchdownPoints (_V(0,0,10), _V(-3.5,0,-3), _V(3.5,0,-3));
	SetCOG_elev(0.001);
	EnableTransponder (true);
	bool render_cockpit = true;

	// ******************** NAV radios **************************

	InitNavRadios (4);

	// ************************* mesh ***************************
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Island1"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Base_Signs"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Admin"), &OFFSET), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Billboards"), &OFFSET), MESHVIS_ALWAYS);
	meshHangar = oapiLoadMeshGlobal ("AscensionUltra\\AU_TA-NW");
	meshWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_TA-WO");
	meshLeaseLight = oapiLoadMeshGlobal ("AscensionUltra\\AU_LH-NW");
	meshLeaseLightWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_LH-WO");
	meshLeaseHeavy = oapiLoadMeshGlobal ("AscensionUltra\\AU_HH-NW", OnLeaseHeavyLoad);
	meshLeaseHeavyWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_HH-WO", OnLeaseHeavyLoad);
	meshDRadar = oapiLoadMeshGlobal ("cssc\\d-radar");
	
	double curvoffTA[TURNAROUNDHANGARS]={-0.08,-0.12,-0.17};
	double curvoffLL[LEASELIGHTHANGARS]={0, -0.01, -0.02, -0.03, -0.04, -0.05};
	double curvoffHL[LEASEHEAVYHANGARS]={-0.02, -0.04, -0.06};

	InitSubObjects();

	for(int i=0;i<TURNAROUNDHANGARS;i++)
	{
		VECTOR3 off=OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoffTA[i],0);
		SetMeshVisibilityMode (AddMesh (meshHangar, &off), MESHVIS_ALWAYS);
		turnAround[i].SetPosition(off);
	}
	for(int i=0;i<LEASELIGHTHANGARS;i++)
	{
		VECTOR3 off=OFFSET+LL1OFFSET+LL1MATRIXOFFSET*i+_V(0,curvoffLL[i],0);
		SetMeshVisibilityMode (AddMesh (meshLeaseLight, &off), MESHVIS_ALWAYS);
		leaseLight[i].SetPosition(off);
	}
	for(int i=0;i<LEASEHEAVYHANGARS;i++)
	{
		VECTOR3 off=OFFSET+LH1OFFSET+LH1MATRIXOFFSET*i+_V(0,curvoffHL[i],0);
		SetMeshVisibilityMode (AddMesh (meshLeaseHeavy, &off), MESHVIS_ALWAYS);
		leaseHeavy[i].SetPosition(off);
	}
	SetMeshVisibilityMode (AddMesh (meshLaunch = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_NW"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshVertical = oapiLoadMeshGlobal ("AscensionUltra\\AU_VLC_NW"), &(OFFSET+VLC1OFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshVerticalSmall = oapiLoadMeshGlobal ("AscensionUltra\\AU_VLC2_NW"), &(OFFSET+VLC2OFFSET)), MESHVIS_ALWAYS);
	for(int i=0;i<DRADARS;i++)
	{
		VECTOR3 off=OFFSET+DRADAROFFSET+DRADARMATRIXOFFSET*i;
		SetMeshVisibilityMode (AddMesh (meshDRadar, &off), MESHVIS_ALWAYS);
		dradar[i].SetPosition(off+_V(0,DRADARPIVOT,0)); //Dish position
	}
	SetMeshVisibilityMode (AddMesh (meshDocks = oapiLoadMeshGlobal ("AscensionUltra\\AU_Docks"), &(OFFSET+DOCKSOFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshAirport = oapiLoadMeshGlobal ("AscensionUltra\\AU_Airport_NW"), &(OFFSET+AIRPORTOFFSET)), MESHVIS_ALWAYS);	
	for(int i=0;i<TURNAROUNDHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshWindow, &(OFFSET+TA1OFFSET+TA1MATRIXOFFSET*i+_V(0,curvoffTA[i],0))), MESHVIS_ALWAYS);
	for(int i=0;i<LEASELIGHTHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshLeaseLightWindow, &(OFFSET+LL1OFFSET+LL1MATRIXOFFSET*i+_V(0,curvoffLL[i],0))), MESHVIS_ALWAYS);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) SetMeshVisibilityMode (AddMesh (meshLeaseHeavyWindow, &(OFFSET+LH1OFFSET+LH1MATRIXOFFSET*i+_V(0,curvoffHL[i],0))), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshLaunchWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_LFMC_WO"), &(OFFSET+LFMCOFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshVerticalWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_VLC_WO"), &(OFFSET+VLC1OFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshVerticalSmallWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_VLC2_WO"), &(OFFSET+VLC2OFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshAirportWindow = oapiLoadMeshGlobal ("AscensionUltra\\AU_Airport_WO"), &(OFFSET+AIRPORTOFFSET)), MESHVIS_ALWAYS);
	SetMeshVisibilityMode (AddMesh (meshTopo = oapiLoadMeshGlobal ("AscensionUltra\\AU_Flora"), &OFFSET), MESHVIS_ALWAYS);
	
	launchTunnel.SetPosition(OFFSET+LFMCOFFSET);
	vertical.SetPosition(OFFSET+VLC1OFFSET);
	verticalSmall.SetPosition(OFFSET+VLC2OFFSET);
	docks.SetPosition(OFFSET+DOCKSOFFSET);
	airport.SetPosition(OFFSET+AIRPORTOFFSET);

	DefineAnimations();

	int index=0;

	for(int i=0;i<TURNAROUNDHANGARS;i++) index=turnAround[i].InitActionAreas(crew, index);
	for(int i=0;i<LEASELIGHTHANGARS;i++) index=leaseLight[i].InitActionAreas(crew, index);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) index=leaseHeavy[i].InitActionAreas(crew, index);
	vertical.InitActionAreas(crew, index);
	verticalSmall.InitActionAreas(crew, index);
}

// Read status from scenario file
void AscensionUltra::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {		
		if (!_strnicmp (line, "HANGAR", 6)) {
			sscanf (line+6, "%d", &cur_TurnAround);
		} else if (cur_TurnAround>=0 && cur_TurnAround<TURNAROUNDHANGARS) {
			if (!turnAround[cur_TurnAround].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "LEASE", 5)) {
			sscanf (line+5, "%X", &cur_Lease);
		} else if (cur_Lease>=0 && cur_Lease<LEASELIGHTHANGARS) {
			if (!leaseLight[cur_Lease].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (cur_Lease>=LEASELIGHTHANGARS && cur_Lease<LEASELIGHTHANGARS+LEASEHEAVYHANGARS) {
			if (!leaseHeavy[cur_Lease-LEASELIGHTHANGARS].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "LAUNCHTUNNEL", 12)) {
			sscanf (line+12, "%X", &cur_LaunchTunnel);
		} else if (cur_LaunchTunnel>=0 && cur_LaunchTunnel<1) {
			if (!launchTunnel.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "DRADAR", 6)) {
			sscanf (line+6, "%X", &cur_DopplerRadar);
		} else if (cur_DopplerRadar>=0 && cur_DopplerRadar<2) {
			if (!dradar[cur_DopplerRadar].clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "AIRPORT", 7)) {
			sscanf (line+7, "%X", &cur_Airport);
		} else if (cur_Airport>=0 && cur_Airport<1) {
			if (!airport.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "DOCKYARD", 8)) {
			sscanf (line+8, "%X", &cur_Docks);
		} else if (cur_Docks>=0 && cur_Docks<1) {
			if (!docks.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (!_strnicmp (line, "VERTICALLAUNCH", 14)) {
			sscanf (line+14, "%X", &cur_Vertical);
		} else if (cur_Vertical>=0 && cur_Vertical<1) {
			if (!vertical.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else if (cur_Vertical>=1 && cur_Vertical<2) {
			if (!verticalSmall.clbkLoadStateEx(line)) ParseScenarioLineEx (line, vs);
		} else {
            ParseScenarioLineEx (line, vs);
			// unrecognised option - pass to Orbiter's generic parser
        }
    }
}

// Write status to scenario file
void AscensionUltra::clbkSaveState (FILEHANDLE scn)
{
	// Write default vessel parameters
	VESSEL2::clbkSaveState (scn);
	if (!scnsave) return;

	char cbuf[256];
	int i;

	// Write custom parameters
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		sprintf (cbuf, "%d", i);
		oapiWriteScenario_string (scn, "HANGAR", cbuf);
		turnAround[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%d", i);
	oapiWriteScenario_string (scn, "HANGAR", cbuf);

	for(i=0;i<LEASEHEAVYHANGARS+LEASELIGHTHANGARS;i++)
	{
		sprintf (cbuf, "%X", i);
		oapiWriteScenario_string (scn, "LEASE", cbuf);
		if (i<LEASELIGHTHANGARS) leaseLight[i].clbkSaveState(scn);
		else leaseHeavy[i-LEASELIGHTHANGARS].clbkSaveState(scn);
	}	
	sprintf (cbuf, "%X", i);
	oapiWriteScenario_string (scn, "LEASE", cbuf);

	oapiWriteScenario_string (scn, "LAUNCHTUNNEL", "0");
	launchTunnel.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "LAUNCHTUNNEL", "1");

	for(i=0;i<DRADARS;i++)
	{
		sprintf (cbuf, "%X", i);
		oapiWriteScenario_string (scn, "DRADAR", cbuf);
		dradar[i].clbkSaveState(scn);
	}
	sprintf (cbuf, "%X", i);
	oapiWriteScenario_string (scn, "DRADAR", cbuf);

	oapiWriteScenario_string (scn, "VERTICALLAUNCH", "0");
	vertical.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "VERTICALLAUNCH", "1");
	verticalSmall.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "VERTICALLAUNCH", "2");
		
	sprintf (cbuf, "%X", i);
	oapiWriteScenario_string (scn, "VERTICALLAUNCH", cbuf);

	oapiWriteScenario_string (scn, "AIRPORT", "0");
	airport.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "AIRPORT", "1");	

	oapiWriteScenario_string (scn, "DOCKYARD", "0");
	docks.clbkSaveState(scn);
	oapiWriteScenario_string (scn, "DOCKYARD", "1");
}

// Finalise vessel creation
void AscensionUltra::clbkPostCreation ()
{
	SetEmptyMass (EMPTY_MASS);

	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].clbkPostCreation();
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].clbkPostCreation();
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].clbkPostCreation();
	launchTunnel.clbkPostCreation();
	for(int i=0;i<DRADARS;i++) dradar[i].clbkPostCreation();
	vertical.clbkPostCreation();
	verticalSmall.clbkPostCreation();
	docks.clbkPostCreation();
	airport.clbkPostCreation();
}

// Respond to playback event
bool AscensionUltra::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!_strnicmp (event_type, "HANGAR", 6))
	{
		//Hangar event
		int hangar=(int)(event_type+6)[0]-0x30;
		if (hangar>=0 && hangar<TURNAROUNDHANGARS) return turnAround[hangar].clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	if (!_strnicmp (event_type, "LEASE", 5))
	{
		//Hangar event
		int hangar=(int)(event_type+5)[0]-0x30;
		if (hangar>=0 && hangar<LEASELIGHTHANGARS) return leaseLight[hangar].clbkPlaybackEvent(simt, event_t, event_type+6, event);
		if (hangar>=LEASELIGHTHANGARS && hangar<LEASEHEAVYHANGARS) return leaseHeavy[hangar-LEASELIGHTHANGARS].clbkPlaybackEvent(simt, event_t, event_type+6, event);
	}	
	if (!_strnicmp (event_type, "LAUNCHTUNNEL", 12))
	{
		//Tunnel event
		return launchTunnel.clbkPlaybackEvent(simt, event_t, event_type+12, event);
	}
	if (!_strnicmp (event_type, "DRADAR", 6))
	{
		//Tunnel event
		int radar=(int)(event_type+6)[0]-0x30;
		if (radar>=0 && radar<DRADARS) return dradar[radar].clbkPlaybackEvent(simt, event_t, event_type+6, event);
	}
	if (!_strnicmp (event_type, "VERTICALLAUNCH", 14))
	{
		//Hangar event
		int hangar=(int)(event_type+14)[0]-0x30;
		if (hangar==0) return vertical.clbkPlaybackEvent(simt, event_t, event_type+15, event);
		if (hangar==1) return verticalSmall.clbkPlaybackEvent(simt, event_t, event_type+15, event);
	}	
	if (!_strnicmp (event_type, "AIRPORT", 7))
	{
		//Airport event
		return airport.clbkPlaybackEvent(simt, event_t, event_type+7, event);
	}
	if (!_strnicmp (event_type, "DOCKYARD", 8))
	{
		//Airport event
		return docks.clbkPlaybackEvent(simt, event_t, event_type+8, event);
	}
	return false;
}

// Create DG visual
void AscensionUltra::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].clbkVisualCreated(vis, refcount);
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].clbkVisualCreated(vis, refcount);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].clbkVisualCreated(vis, refcount);
	launchTunnel.clbkVisualCreated(vis, refcount);
	for(int i=0;i<DRADARS;i++) dradar[i].clbkVisualCreated(vis, refcount);
	vertical.clbkVisualCreated(vis, refcount);
	verticalSmall.clbkVisualCreated(vis, refcount);
	docks.clbkVisualCreated(vis, refcount);
	airport.clbkVisualCreated(vis, refcount);
}

// Destroy DG visual
void AscensionUltra::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	visual = NULL;
}

// --------------------------------------------------------------
// Frame update
// --------------------------------------------------------------
void AscensionUltra::clbkPostStep (double simt, double simdt, double mjd)
{
	//Check for internal view in order to fix rendering order via MESHVIS_EXTPASS
	//This could decrease performance, so we disable it later again
	OBJHANDLE obj=oapiGetFocusObject();
	bool mode=oapiCameraInternal() && obj==GetHandle();
	if (mode != oldCameraMode)
	{
		oldCameraMode=mode;
		int i=STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES+DRADARS+DOCKYARDS+AIRPORTS;
		int k=GetMeshVisibilityMode(i)>MESHVIS_NEVER?GetMeshCount():i;
		WORD vismode=MESHVIS_ALWAYS | (mode?MESHVIS_EXTPASS:0);
		for(int i=0;i<k;i++) SetMeshVisibilityMode(i, vismode);
	}
		
	//Call post steps of all sub-elements
	for(int i=0;i<TURNAROUNDHANGARS;i++) turnAround[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<LEASELIGHTHANGARS;i++) leaseLight[i].clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<LEASEHEAVYHANGARS;i++) leaseHeavy[i].clbkPostStep(simt, simdt, mjd);
	launchTunnel.clbkPostStep(simt, simdt, mjd);
	for(int i=0;i<DRADARS;i++)
	{
		if (obj!=dradar[i].GetTarget()) dradar[i].SetTarget(obj);
		dradar[i].clbkPostStep(simt, simdt, mjd);
	}
	vertical.clbkPostStep(simt, simdt, mjd);
	verticalSmall.clbkPostStep(simt, simdt, mjd);
	docks.clbkPostStep(simt, simdt, mjd);
	airport.clbkPostStep(simt, simdt, mjd);

	//Check all virtual docks
	VECTOR3 global, local;
	std::map<Room*, VESSEL*> toBeDeleted;
	for(std::map<Room*, VESSEL*>::iterator i=roomVessel.begin();i!=roomVessel.end();i++)
	{	
		//Check ground contact
		if (!i->second->GroundContact())
		{
			toBeDeleted[i->first]=i->second;
			continue;
		}

		//Check vincinity
		i->second->GetGlobalPos(global);
		Global2Local(global, local);
		if (!i->first->GetHangar()->CheckVincinity(&local)) toBeDeleted[i->first]=i->second;
	}
	for(std::map<Room*, VESSEL*>::iterator i=toBeDeleted.begin();i!=toBeDeleted.end();i++)
	{
		roomVessel.erase(i->first);
		vesselRoom.erase(i->second);
		OrbiterExtensions::SetDockState(i->second, NULL);
		i->first->SetDock(NULL);
	}

	//Detect activated action area, iterate through sub-items for processing, break on first processor
	int action=crew->DetectActionAreaActivated();
	if (action>-1) for(int i=0;i<TURNAROUNDHANGARS;i++) if (turnAround[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) for(int i=0;i<LEASELIGHTHANGARS;i++) if (leaseLight[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) for(int i=0;i<LEASEHEAVYHANGARS;i++) if (leaseHeavy[i].ActionAreaActivated(action)) {action=-1;break;}
	if (action>-1) vertical.ActionAreaActivated(action);
	if (action>-1) verticalSmall.ActionAreaActivated(action);

	//DEBUG relative position
	if (coords)
	{
		VECTOR3 global, local;
		oapiGetFocusInterface()->GetGlobalPos(global);
		Global2Local(global, local);
		local-=OFFSET+LFMCOFFSET;
		sprintf(oapiDebugString(),"MAP coordinates (w.r.t LFMC): %f , %f , %f", -local.x, local.y, local.z);
	}
	
	if (orbiterExtensionsResult<0) sprintf(oapiDebugString(), "WARNING! Orbiter extensions not loaded. Error %d", orbiterExtensionsResult);
	else crew->WarnUserUMMUNotInstalled("Ascension Ultra");
}

bool AscensionUltra::clbkLoadGenericCockpit ()
{
	SwitchView(controlRoom);	
	oapiSetDefNavDisplay (1);
	oapiSetDefRCSDisplay (1);
	campos = CAM_GENERIC;
	return true;
}

// Process buffered key events
int AscensionUltra::clbkConsumeBufferedKey (DWORD key, bool down, char *kstate)
{
	//char inputBoxBuffer[81], inputBoxTitle[81];
	
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (KEYMOD_SHIFT (kstate)) {
	} else if (KEYMOD_CONTROL (kstate)) {
		switch (key) {
			case OAPI_KEY_1: break; //TODO: CTRL+1 key
			default: break; //TODO: CTRL keys
		}
	} else {
		switch (key) {
		//DEBUG		
		case OAPI_KEY_1:
			MoveGroup(mnr, _V(stp, 0, 0));
			disx+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_2:
			MoveGroup(mnr, _V(-stp, 0, 0));
			disx-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_3:
			MoveGroup(mnr, _V(0, stp, 0));
			disy+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_4:
			MoveGroup(mnr, _V(0, -stp, 0));
			disy-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_5:
			MoveGroup(mnr, _V(0, 0, stp));
			disz+=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_6:
			MoveGroup(mnr, _V(0, 0, -stp));
			disz-=stp;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_7:
			stp/=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_8:
			stp*=10.0;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_9:
			if (mnr>0) mnr--;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_0:
			mnr++;
			sprintf(oapiDebugString(), "[%d]x%f y%f z%f d%f", mnr, disx, disy, disz, stp);
			return 1;
		case OAPI_KEY_C:
			coords=!coords;
			return 1;
		case OAPI_KEY_W:
			{
				
				int i=STATICMESHES+TURNAROUNDHANGARS+LEASELIGHTHANGARS+LEASEHEAVYHANGARS+LAUNCHTUNNELS+VERTICALLAUNCHES+DRADARS+DOCKYARDS+AIRPORTS;
				int k=GetMeshCount()-STATICTRANSPARENCIES;
				int mode=GetMeshVisibilityMode(i) > MESHVIS_NEVER?MESHVIS_NEVER:(MESHVIS_ALWAYS | MESHVIS_EXTPASS);
				for(;i<k;i++) SetMeshVisibilityMode(i, mode);
				sprintf(oapiDebugString(), "Window status: %X", mode);
			}
			return 1;
		}
	}
	return 0;
}

void AscensionUltra::MoveGroup(int mesh, VECTOR3 v)
{
	//Transfer mesh groups	
	MESHGROUP_TRANSFORM mt;
	mt.nmesh=mesh;
	mt.transform=mt.TRANSLATE;
	mt.P.transparam.shift=v;
	int k=oapiMeshGroupCount(GetMesh(visual, mesh));
	for(mt.ngrp=0;mt.ngrp<k;mt.ngrp++) MeshgroupTransform(visual, mt);	
}

int AscensionUltra::GetHangars(int type)
{
	int count=0;
	if ((type & HANGARTYPETA)>0) count+=TURNAROUNDHANGARS;
	if ((type & HANGARTYPELL)>0) count+=LEASELIGHTHANGARS;	
	if ((type & HANGARTYPELH)>0) count+=LEASEHEAVYHANGARS;
	if ((type & HANGARTYPELFMC)>0) count+=1;
	if ((type & HANGARTYPEVLC)>0) count+=2;
	if ((type & HANGARTYPEPORT)>0) count+=1;
	return count;
}

Hangar *AscensionUltra::GetHangar(int type, int index)
{
	if (index<0) return NULL;
	if ((type & HANGARTYPETA)>0)
	{
		if (index<TURNAROUNDHANGARS) return turnAround+index;
		else index-=TURNAROUNDHANGARS;
	}
	if ((type & HANGARTYPELL)>0)
	{
		if (index<LEASELIGHTHANGARS) return leaseLight+index;
		else index-=LEASELIGHTHANGARS;
	}	
	if ((type & HANGARTYPELH)>0)
	{
		if (index<LEASEHEAVYHANGARS) return leaseHeavy+index;
		else index-=LEASEHEAVYHANGARS;
	}
	if ((type & HANGARTYPELFMC)>0)
	{
		if (index<1) return &launchTunnel;
		else index-=1;
	}
	if ((type & HANGARTYPEVLC)>0)
	{
		if (index==0) return &vertical;
		else if (index==1) return &verticalSmall;
		else index-=2;
	}
	if ((type & HANGARTYPEPORT)>0)
	{
		if (index<1) return &airport;
		else index-=1;
	}
	if ((type & HANGARTYPEDOCKS)>0)
	{
		if (index<1) return &docks;
		else index-=1;
	}
	return NULL;
}

Taxiways *AscensionUltra::GetTaxiways(){return &taxiways;}

Runways *AscensionUltra::GetRunways(){return &runways;}

Room *AscensionUltra::GetControlRoom(){return controlRoom;}
void AscensionUltra::SwitchView(Room *room)
{
	SetCameraOffset (room->GetHangar()->GetPosition()+room->GetCameraPosition());
	SetCameraDefaultDirection(room->GetViewDirection());
	if (oapiGetFocusObject()==GetHandle()) oapiCameraSetCockpitDir(0,0);
	controlRoom=room;
}

int AscensionUltra::GetPersons()
{
	int i, rooms, j, persons=0;
		
	for(i=0;i<TURNAROUNDHANGARS;i++)
	{
		rooms=turnAround[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=turnAround[i].GetRoom(j)->GetPersons();
	}

	for(i=0;i<LEASELIGHTHANGARS;i++)
	{
		rooms=leaseLight[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=leaseLight[i].GetRoom(j)->GetPersons();
	}

	for(i=0;i<LEASEHEAVYHANGARS;i++)
	{
		rooms=leaseHeavy[i].GetRooms();
		for(j=0;j<rooms;j++) persons+=leaseHeavy[i].GetRoom(j)->GetPersons();
	}

	rooms=launchTunnel.GetRooms();
	for(j=0;j<rooms;j++) persons+=launchTunnel.GetRoom(j)->GetPersons();	

	rooms=vertical.GetRooms();
	for(j=0;j<rooms;j++) persons+=vertical.GetRoom(j)->GetPersons();
	rooms=verticalSmall.GetRooms();
	for(j=0;j<rooms;j++) persons+=verticalSmall.GetRoom(j)->GetPersons();

	rooms=docks.GetRooms();
	for(j=0;j<rooms;j++) persons+=docks.GetRoom(j)->GetPersons();

	rooms=airport.GetRooms();
	for(j=0;j<rooms;j++) persons+=airport.GetRoom(j)->GetPersons();
	
	return ++persons;	//First entry is always the ADD PERSON entry
}

Room* AscensionUltra::GetPersonLocationFromHangar(int &index, Hangar *hangar)
{	
	int rooms=hangar->GetRooms();
	for(int j=0;j<rooms;j++)
	{
		Room *room=hangar->GetRoom(j);
		if (room->GetPersons()>index) return room;
		index-=room->GetPersons();
	}
	return NULL;
}

Room *AscensionUltra::GetPersonLocation(int &index)
{
	if (index==0) return airport.GetEntrance();
	index--;

	Room *room;
			
	for(int i=0;i<TURNAROUNDHANGARS;i++) if ((room = GetPersonLocationFromHangar(index, &turnAround[i]))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &launchTunnel))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &vertical))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &verticalSmall))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &docks))!=NULL) return room;
	if ((room = GetPersonLocationFromHangar(index, &airport))!=NULL) return room;
	
	index=0;
	return NULL;	
}

Person AscensionUltra::GetPerson(int index, Room *room)
{
	if (room==NULL) room=GetPersonLocation(index);
	return Person(room, index);
}

// Changes person properties according to flags (see header-file for PERSON_xxx macros)
// Returns values:
// -7 .. internal failure
// -3 .. no more slots available on location change (or ERROR_DOCKED_SHIP_IS_FULL on transfer)
// <0 .. negative results from UMmuSDK.h
//  0 .. success of EVA or remove
// >0 .. new index of modified person
int AscensionUltra::ChangePerson(int index, int flags, ...)
{	
	int result=0;
	va_list args;
	va_start(args, flags);
	Room *room=NULL;
	if ((flags & PERSON_FILTER) > 0) room=va_arg(args, Room*);
	flags = flags & ~PERSON_FILTER;
	Person person=GetPerson(index, room);
	UMMUCREWMANAGMENT *crew=person.Location->GetCrew();

	switch (flags)
	{
	case PERSON_EVA:
		{
			VESSEL *vessel=person.Location->GetDock();
			if (vessel!=NULL)
			{
				CreateDock(_V(0,0,0), _V(0,1,0), _V(0,1,0));
				OrbiterExtensions::SetDockState(this, vessel->GetHandle());
			}
			result=crew->EvaCrewMember(person.Name);
			if (result==ERROR_NO_ONE_ON_BOARD ||
				(result==EVA_OK && vessel!=NULL) ||
				(result==TRANSFER_TO_DOCKED_SHIP_OK && vessel==NULL)) result=ERROR_CHANGE_FAIL;
			if (result>EVA_OK) result=EVA_OK;
			if (vessel!=NULL)
			{
				OrbiterExtensions::SetDockState(this, NULL);
				ClearDockDefinitions();
			}
			break;
		}
	case PERSON_DELETE:
		crew->RemoveCrewMember(person.Name);		
		break;
	default:
		//Set new location if necessary
		if ((flags & PERSON_LOCATION)>0)
		{
			person.Location=va_arg(args, Room*);
			if (person.Location!=NULL)
				if (person.Location->GetCrew()->GetCrewTotalNumber()>=person.Location->GetMaxPersons())
					person.Location=NULL;			
			if (person.Location==NULL)
			{
				va_end(args);
				return ERROR_DOCKED_SHIP_IS_FULL;
			}			
		}

		//Prepare removal of crew member - heap variables to preserve strings
		char *name=new char[strlen(person.Name)+1];
		char *miscId=new char[strlen(person.MiscId)+1];
		strcpy(name, person.Name);
		strcpy(miscId, person.MiscId);
		person.Name=name;
		person.MiscId=miscId;

		//Remove crew member prior to adding new crew member with changes properties - important to
		//stay withing max. crew member amount
		crew->RemoveCrewMember(person.Name);

		//Set optionally changed crew and recreate add person if necessary
		if (crew!=person.Location->GetCrew())
		{
			if (crew==this->crew) crew->AddCrewMember("John Doe", 20, 60, 75, "Crew");
			crew=person.Location->GetCrew();
		}
		
		if ((flags & PERSON_NAME)>0) person.Name=va_arg(args, char*);
		if ((flags & PERSON_MISCID)>0) person.MiscId=va_arg(args, char*);
		if ((flags & PERSON_AGE)>0) person.Age=atoi(va_arg(args, char*));
		if ((flags & PERSON_PULS)>0) person.Puls=atoi(va_arg(args, char*));
		if ((flags & PERSON_WEIGHT)>0) person.Weight=atoi(va_arg(args, char*));		

		crew->AddCrewMember(person.Name, person.Age, person.Puls, person.Weight, person.MiscId);
		
		//Get index of newly added person
		index=GetPersons();
		for(result=0;result<index;result++)
		{
			Person p=GetPerson(result);
			if (p.Location->GetCrew()!=crew) continue;
			if (strcmp(p.Name, person.Name)==0) break;
		}
		if (result==index) result=ERROR_CHANGE_FAIL;
		
		delete [] name;
		delete [] miscId;
		break;
	}
	va_end(args);
	return result;
}

Hangar *AscensionUltra::GetNearestHangar(int type, VESSEL *vessel)
{
	//Check Orbiter extensions version
	if (orbiterExtensionsVersion<0.1) return NULL;

	//Check vessel landed
	if (!vessel->GroundContact()) return NULL;

	VECTOR3 global, local;
	vessel->GetGlobalPos(global);
	Global2Local(global, local);
	global=local-OFFSET;
	
	if (global.x<-6000 || global.x>0 || global.z<0 || global.z>1300) return NULL; //Check base vincinity
	
	if (launchTunnel.CheckVincinity(&local)) return &launchTunnel;	
	if (airport.CheckVincinity(&local)) return &airport;
	if (docks.CheckVincinity(&local)) return &docks;
	for(int i=0;i<TURNAROUNDHANGARS;i++) if (turnAround[i].CheckVincinity(&local)) return &turnAround[i];
	for(int i=0;i<LEASELIGHTHANGARS;i++) if (leaseLight[i].CheckVincinity(&local)) return &leaseLight[i];
	for(int i=0;i<LEASEHEAVYHANGARS;i++) if (leaseHeavy[i].CheckVincinity(&local)) return &leaseHeavy[i];
	if (vertical.CheckVincinity(&local)) return &vertical;
	if (verticalSmall.CheckVincinity(&local)) return &verticalSmall;
	
	return NULL;
}

int AscensionUltra::GetChecklists(int type, VESSEL *vessel)
{
	int count=0;
	int hangars=GetHangars(type);
	for(int i=0;i<hangars;i++) count+=GetChecklists(GetHangar(type, i),vessel);
	return count;
}

Checklist *AscensionUltra::GetChecklist(int type, int index, VESSEL *vessel)
{
	int hangars=GetHangars(type);
	for(int i=0;i<hangars;i++)
	{
		Hangar *hangar=GetHangar(type, i);
		int lists=GetChecklists(hangar,vessel);
		if (index<lists) return GetChecklist(hangar, index, vessel);
		index-=lists;
		if (index<0) break;
	}
	return NULL;
}

int AscensionUltra::GetChecklists(Hangar *hangar, VESSEL *vessel)
{
	int lists=hangar->GetChecklists();
	if (vessel==NULL) return lists;

	int count=0;
	OBJHANDLE filter=vessel->GetHandle();
	for(int i=0;i<lists;i++)
		if (hangar->GetChecklist(i)->GetSubject()==filter) count++;
	return count;
}

Checklist *AscensionUltra::GetChecklist(Hangar *hangar, int index, VESSEL *vessel)
{
	if (vessel==NULL) return hangar->GetChecklist(index);
	
	int lists=hangar->GetChecklists();
	OBJHANDLE filter=vessel->GetHandle();
	for(int i=0;i<lists;i++)
	{
		Checklist *list=hangar->GetChecklist(i);
		if (list->GetSubject()==filter)
		{
			if (index==0) return list;
			else index--;
			if (index<0) break;
		}
	}
	return NULL;
}

void AscensionUltra::RegisterEventHandler(void (*handler)(BaseVessel::EventHandler::Arguments args, void *context), void *context){eventHandlers[context]=handler;}

void AscensionUltra::UnregisterEventHandler(void (*handler)(BaseVessel::EventHandler::Arguments args, void *context), void *context){eventHandlers.erase(context);}

void AscensionUltra::SendEvent(BaseVessel::EventHandler::Arguments args)
{
	for(std::map<void*, void (*)(BaseVessel::EventHandler::Arguments args, void *context)>::iterator i=eventHandlers.begin(); i!=eventHandlers.end(); i++) i->second(args, i->first);
}

void AscensionUltra::DockVessel(Room *room, VESSEL *vessel)
{
	//Check Orbiter extensions version
	if (orbiterExtensionsVersion<0.1) return;

	VESSEL *oldVessel;
	Room *oldRoom;
	if (roomVessel.find(room)==roomVessel.end()) oldVessel=NULL;
	else oldVessel=roomVessel[room];
	if (vesselRoom.find(vessel)==vesselRoom.end()) oldRoom=NULL;
	else oldRoom=vesselRoom[vessel];

	if (room==NULL && oldRoom!=NULL)
	{
		//Undock vessel
		oldRoom->SetDock(NULL);
		roomVessel.erase(oldRoom);
		vesselRoom.erase(vessel);
		OrbiterExtensions::SetDockState(vessel, NULL);
	}

	if (vessel==NULL && oldVessel!=NULL)
	{
		//Undock room
		room->SetDock(NULL);
		roomVessel.erase(room);
		vesselRoom.erase(oldVessel);
		OrbiterExtensions::SetDockState(oldVessel, NULL);
	}

	if (vessel!=NULL && room!=NULL)
	{
		if (oldRoom!=NULL)
		{
			//Undock old room occupation
			oldRoom->SetDock(NULL);
			vesselRoom.erase(vessel);
			roomVessel.erase(oldRoom);			
		}
		if (oldVessel!=NULL)
		{
			//Undock old vessel occupation
			room->SetDock(NULL);
			roomVessel.erase(room);
			vesselRoom.erase(oldVessel);
			OrbiterExtensions::SetDockState(oldVessel, NULL);
		}
		//Dock vessel to room
		room->SetDock(vessel);
		roomVessel[room]=vessel;
		vesselRoom[vessel]=room;
		OrbiterExtensions::SetDockState(vessel, GetHandle());
	}
}

// Formats and talks a message.
// ${x..} tags reference parameters. The first character defines the parameter typ as follows:
//   [vV] is the name of the subject vessel
//   [aA] is the name of the AU vessel
//   [sS] is the talker definition for the subject vessel
//   [bB] is the talker definition for the AU vessel (the base)
//   [rR] is an arbitrary acknowledgment string, taken from talker definition
//   [wW] is "speaking" a sound (aka wave) file given in the tag's remaining characters as relative path to Orbiter's root
//   [tT] switches to text-to-speech only mode - text will only be spoken
//   [dD] switches to display only mode - text will only be displayed, possible XML-tags will NOT be filtered
//   [cC] switches to combined mode - text will be both spoken and displayed as is, just XML-tags will be filtered for display
//   [0-9] together with the tag's remaining characters defines a custom argument by number
void AscensionUltra::Talk(LPCWSTR message, OBJHANDLE subject, int argc, ...)
{
	if (!talker) return;

	//Note: this trick here only works with C++11, because the restriction of in-function structure usage in templates was lifted there
	struct ReplaceItem
	{
		int start;
		int width;
		int length;
		WCHAR *text;
	} replacement;
	std::vector<ReplaceItem> replacements;

	replacement.start=0;
	replacement.width=-2; //Works as state in parser: -2=idle, -1='$'read, >=0=characters in tag

	BaseVessel::Talker::Voice *voice=&voiceATC;

	//Create argument index
	std::vector<LPCWSTR> arguments, *p;
	va_list args;
	va_start(args, argc);
	for(int i=0;i<argc;i++)	arguments.push_back(va_arg(args, LPCWSTR));
	va_end(args);

	//Parse tags
	int k=wcslen(message);
	int change=0;
	for(int i=0;i<k;i++)
	{
		switch(replacement.width)
		{
		case -2: //IDLE
			replacement.start=i;
			if (message[i]==L'$') replacement.width++;
			break;
		case -1: //'$'READ
			if (message[i]!=L'{')
			{
				replacement.width--;
				replacement.start=i;
			}
			replacement.width++;
			break;
		default:
			if (replacement.width++<0 || message[i]!=L'}') continue;
			else
			{
				int arg=0; //Wrong formated tags will default to argument 0
				swscanf(message+replacement.start+2, L"%d}", &arg);
				char *name=NULL;
				if (arg<0/* || arg>argc*/)
				{
					//Create an empty string to replace the wrong argument reference
					replacement.text=new WCHAR[(replacement.length=0)+1];
					replacement.text[0]=0x00;
				}
				else switch(arg)
				{
				case 0:
					name=oapiGetVesselInterface(subject)->GetName();
					voice=&voiceVessel;
					goto convert;
				case 1:
					name=GetName();
					voice=&voiceATC;
		 convert:	replacement.length=strlen(name);
					replacement.text=new WCHAR[replacement.length+1];
					mbstowcs(replacement.text, name, replacement.length+1);
					replacement.length=wcslen(replacement.text);
					break;
				case 2:
					voice=&voiceVessel;
					goto select;
				case 3:
					voice=&voiceATC;
			select:	replacement.length=wcslen(voice->Definition);
					replacement.text=new WCHAR[replacement.length+1];
					wcscpy(replacement.text, voice->Definition);
					break;
				case 4:
					p=&voice->Acknowledgement;
					arg=rand()%p->size(); //Random number between 0 and amount of defined acknowledgments minus 1
					goto copy;
				default: //Get argument from argument list
					p=&arguments;					
			copy:   replacement.length=wcslen((*p)[arg]);
					replacement.text=new WCHAR[replacement.length+1];
					wcscpy(replacement.text, (*p)[arg]);					
					break;
				}
				replacement.width+=2; //add the "${" header to width				
				change+=replacement.length-replacement.width; //Calculate how the length of the expanded string will change
				replacements.push_back(replacement);
				replacement.start=0;
				replacement.width=-2;
			}
			break;
		}
	}
	//We don't care for unclosed tags, so last replacement is ignored.

	//Do the replacing
	WCHAR *text=new WCHAR[k+change+1];
	k=change=0;
	for(int i=0;i<(int)replacements.size();i++)
	{
		ReplaceItem r=replacements[i];
		int l=r.start-change;				//Remaining original text length up to the tag begin
		wcsncpy(text+k, message+change, l);	//Copy original text
		wcscpy(text+k+l, r.text);			//Copy replacement text
		k+=r.length+l;						//Advance destination pointer beyond replacement text
		change=r.start+r.width;				//Advance source pointer beyond tag end
		delete [] r.text;
	}
	wcscpy(text+k, message+change);		//Copy the remaining source text
	replacements.clear();

	TalkerEntry entry;
	entry.message=text;
	entry.display=NULL; //TODO: do proper display text setting
	entry.size=voice->Size;
	entry.color=voice->Color;
	entry.flags=NULL;
	talker(entry, GetHandle(), subject);
	
	delete [] text;
}

void AscensionUltra::SetTalker(void (*talker)(const TalkerEntry &, const OBJHANDLE, const OBJHANDLE)){this->talker=talker;}
