#pragma once
#include "orbitersdk.h"
#include <map>
#include <vector>
#include "BeaconPath.h"

struct Route
{
	BeaconPath *Path;
	bool Inversed;
	bool On;
	bool Strobing;
	int Priority;
};

class Routes
{
public:
	~Routes(void);
	void Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed, int priority=0);
	void Clear();

	void Init(double size, double fallOff, double period, double duration, double propagate);

	// Finalize priorities - this will show enumeration sorted according to priority settings
	void PriorityFinalize();

	// Turn all routes on and off
	virtual void Switch(bool on);

	// Switch strobing of all routes on and off
	virtual void Strobe(bool on);

	// Switch strobing of specified route on and off
	virtual void Switch(const char *start, const char *end, bool on);

	// Check if specified route is on or off
	virtual bool On(const char *start, const char *end);

	// Check if one of the routes from/to the given start/end point is on or off
	virtual bool On(const char *point, bool isEnd=false);

	// Switch strobing of specified route on and off
	virtual void Strobe(const char *start, const char *end, bool on);

	// Check if specified route is strobing
	virtual bool Strobing(const char *start, const char *end);

	// Check if one of the routes from/to the given start/end point is strobing
	virtual bool Strobing(const char *point, bool isEnd=false);

	/* Return count of all:
	   no parameters        .. available start points
	   true                 .. available end points
	   false, name of point .. end points of routes with the given start point
	   true, name of point  .. start points of routes with the given end point */
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);

	/* Return name of taxi-route point with given index in the list of all:
	   no additional params .. available start points
	   true                 .. available end points
	   false, name of point .. end points of routes with the given start point
	   true, name of point  .. start points of routes with the given end point */
	virtual char *GetPoint(int index, bool isEnd=false, char *fromPoint=NULL);
	
private:	
	std::map<const char *, std::map<const char *, Route *> > links, reverse;
	const char **starts, **ends;
	std::map<const char *, const char ** > endsForStarts, startsForEnds;
	std::map<const char *, Route *> *GetEnds(char const*start);
	std::map<const char *, Route *> *GetStarts(char const*end);
	Route *Routes::GetLink(char const*start, char const*end);
	void Strobe(Route *link, bool on);
	double size, fallOff, period, duration, propagate;
	bool prioritiesFinalized;
	void ClearPriorities();
	void LinkPriorityFinalize(std::map<const char *, std::map<const char *, Route *>> &links, const char **&starts, std::map<const char *, const char ** > &endsForStarts);
};
