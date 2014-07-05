// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

#pragma once
#include "orbitersdk.h"
#include <map>
#include <vector>

class Hangar;
class AscensionUltra;

class Checklist
{
public:
	Checklist(void);
	~Checklist(void);
	virtual int GetType(void);
	virtual int GetState(void);
	virtual bool SetEvent(int event);
	virtual void SetSubject(OBJHANDLE subject);
	virtual OBJHANDLE GetSubject(void);
	virtual void Init(VESSEL *owner, Hangar *hangar, const char *event_prefix, int state=0);
	virtual void PostStep (double simt, double simdt, double mjd);
	virtual Hangar *GetHangar(void);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetATCText(int fromState, int toState, LPCWSTR text);

private:	
	char *event_prefix;
	int state;
	std::map<int, std::map<int, std::vector<LPCWSTR>>> atc;

protected:
	virtual void Talk(LPCWSTR text); //Override this for custom parameter calls
	void RecordEvent(int event);
	void SetState(int state);
	VECTOR3 GetNosePoint();
	AscensionUltra *owner;
	Hangar *hangar;
	OBJHANDLE subject;
};
