#ifndef EVENT_H_
#define EVENT_H_

#include "DFPN2.h"
#include "Line.h"

enum EventType { TRANSITION, PLACE_LOWER_BOUNDRY, PLACE_UPPER_BOUNDRY, MAX_TIME_REACHED};


/**
 * Stochastic Event. sd
 */
struct StochasticEvent{
	/**
	 * Describes the equation and the validity s-interval of next potential event, dependent on \f$ s \f$.
	 */
	Segment* timeSegment;

	EventType eventType;

	/**
	 * Transition or place ID;
	 */
	int id;

	/**
	 * marking of region before this event happening.
	 */
	Marking* preRegionMarking;

	/**
	 * marking of region before this event happening.
	 */
	Marking* postRegionMarking;

	static bool greaterSlopeFirst(StochasticEvent* e1, StochasticEvent* e2){return e1->timeSegment->a > e2->timeSegment->a;};

	StochasticEvent(Segment *line, EventType type) : timeSegment(line), eventType(type){};
	StochasticEvent(StochasticEvent* se){
		timeSegment = se->timeSegment;
		eventType = se->eventType;
		id = se->id;
		preRegionMarking = se->preRegionMarking;
		postRegionMarking = se->postRegionMarking;
	};

	StochasticEvent(){};

};

/**
 * Deterministic Event.
 */
struct DtrmEvent{
	EventType eventType;

	/**
	 * Transition or place ID;
	 */
	int id;

	/**
	 * Occurance time.
	 */
	double time;

	/**
	 * marking of region before this event happening.
	 */
	Marking* preRegionMarking;

	/**
	 * marking of region before this event happening.
	 */
	Marking* postRegionMarking;

	DtrmEvent(EventType type) : eventType(type){};
};

//bool lineGreaterSlopeFirst(Line & l1, Line & l2);
#endif
