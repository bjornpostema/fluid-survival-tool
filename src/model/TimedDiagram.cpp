/*
* TimedDiagram.cpp
*
*  Created on: 28 okt. 2011
*      Author: GhasemiehH
*/

#include "TimedDiagram.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include "DFPN2.h"

namespace model {

TimedDiagram* TimedDiagram::instance = NULL;

TimedDiagram::TimedDiagram() {
	currentTime = 0;
}

void TimedDiagram::clear(){
	dtrmEventList.clear();
	regionList.clear();
	//instance == NULL;
}

void TimedDiagram::setModel(Model* model){
	clear();
	this->model = model;
}

TimedDiagram* TimedDiagram::getInstance() {
	if (instance == NULL)
		instance = new TimedDiagram();

	return instance;
}

void TimedDiagram::generateDiagram(Marking* initialMarking) {

	/*
	* TODO:
	* Generating the deterministic (under the \f$ t = s \f$ line) part of diagram.
	*/

	SegmentizeDtrmRegion(initialMarking);


	/**
	* Generating the stochastic (top of \f$ t = s \f$ line) part of diagram.
	*/
	//TODO: consider the marking after the last dtrEvent.
	double prePoint = 0;
	gTrEnabledTime = 0;
	for (int j = 0; (unsigned)j < dtrmEventList.size() - 1; j++){
		std::cout << dtrmEventList[j]->time << std::endl;
	}
	// ignore the the last event.
	for (int j = 0; (unsigned)j < dtrmEventList.size() - 1; j++){

		if (!isGTransitionEnabled(model, dtrmEventList[j]->postRegionMarking)){
            prePoint = dtrmEventList[j]->nextDtrmEvent->time;
            gTrEnabledTime = dtrmEventList[j]->nextDtrmEvent->time;
			continue;
		}


		//in case that two consequent event are happening in the same time approximately.
		if (j > 0 && IS_ZERO(dtrmEventList[j]->time - dtrmEventList[j-1]->time))
			continue;

		//moving to the frame with origin at (gTrEnabledTime, gTrEnabledTime)
        Point startPoint(prePoint - gTrEnabledTime, prePoint - gTrEnabledTime);	//******//
        Point endPoint(dtrmEventList[j]->nextDtrmEvent->time - gTrEnabledTime, dtrmEventList[j]->nextDtrmEvent->time - gTrEnabledTime);
        Marking* gtFiredMarking = copyMarking(model, dtrmEventList[j]->postRegionMarking);

		checkEnabled(model, gtFiredMarking);
		setActFluidRate(model, gtFiredMarking, 0);
		/**
		 * The reason for (- startPoint.Y) is that we should shift the time to the frame with origin at (startPoint.Y, startPoint.Y).
		 * (This is how the algorithm is designed, there are other ways too!!)
		 */
		gtFiredMarking = advanceMarking(model, gtFiredMarking, 1, -startPoint.Y, gtFiredMarking->fluidPlaceDeriv,gtFiredMarking->enabling);

		//TODO [IMPORTANT]: check whether the g-transition is enabled.
		fireGeneralTransition(model, gtFiredMarking);

        Segment* tsLine = new Segment(startPoint, endPoint);
		StochasticEvent* initEvent = new StochasticEvent(tsLine, TRANSITION);
		initEvent->id = gTransitionId(model);
		initEvent->preRegionMarking = dtrmEventList[j]->postRegionMarking;
        initEvent->preDtrmEvent = dtrmEventList[j];
		segmentizeStochasticRegion(gtFiredMarking, initEvent, prePoint);

        prePoint = dtrmEventList[j]->nextDtrmEvent->time;
	}

	//std::ofstream regionfile("regions.out");

	//for (int j = 0; j < dtrmEventList.size(); j++){
	//	regionfile << "dtrm region(" << j << "): " << std::endl;
    //	regionfile << "time: " << dtrmEve                   //std::cout << sPdfInt(s1, fv) << "-" << sPdfInt(s2, fv) << std::endl;
    //                    std::cout << sPdfInt(s1, fv) << "-" << sPdfInt(s2, fv) << " ";
    //                    std::cout << fabs(sPdfInt(s1, fv) - sPdfInt(s2, fv)) << " ";
    //                    std::cout << "prob" << prob << std::endl;ntList[j]->time << "-type: " << dtrmEventList[j]->eventType << "-id: " << dtrmEventList[j]->id << std::endl;
	//}

	//regionfile << "-------------------------------------------------" << std::endl;

	//for (int i = 0; i < regionList.size(); i++){
	//	regionfile << "stoch region(" << i << "): " << std::endl;
	//	regionList[i]->print(regionfile);


	//	regionfile << "-------------------------------------------------" << std::endl;
	//}


}

void TimedDiagram::SegmentizeDtrmRegion(Marking* initialMarking){

	//TODO: (IMPORTANT) This function does not take care of multiple events at the same time.
	// Do it later!!!!

	Marking *marking = copyMarking(model, initialMarking);

	double crntTime = 0;
	int *enabled;
	double firstT;

	int firstEventID;
	EventType firstEventType;

	double *clock0, *fluid0, *drift;
    DtrmEvent* crntEvent = 0;
    DtrmEvent* preEvent = 0;

    crntEvent = new DtrmEvent(FIRST_NULL_EVENT);
    crntEvent->time = 0;
    crntEvent->id = -1;
    crntEvent->preRegionMarking = marking;
    crntEvent->postRegionMarking = marking;
    dtrmEventList.push_back(crntEvent);
    preEvent = crntEvent;

	while (crntTime < model->MaxTime){


		checkEnabled(model, marking);
		setActFluidRate(model, marking, crntTime);
		
		Marking* preMarking = copyMarking(model, marking);


		enabled = marking->enabling;
		clock0 = marking->clock0;
		fluid0 = marking->fluid0;
		drift = marking->fluidPlaceDeriv;

		firstT = INF;

		int hasImmediateEnabled;

		// count the number (and put in cache) the number of enabled transitions
		hasImmediateEnabled = 0;
		for (int i = 0; i < model->N_transitions; i++) {
			if (enabled[i] != 1)
				continue;

			if ((model->transitions[i].type == TT_IMMEDIATE)) {
				firstT = 0.0;
				firstEventID = i;
				hasImmediateEnabled = 1;
				firstEventType = TRANSITION;
				break;
			} else if ((model->transitions[i].type == TT_DETERMINISTIC) && 
				(model->transitions[i].time - clock0[model->transitions[i].idInMarking] < firstT)){
					firstT = model->transitions[i].time - clock0[model->transitions[i].idInMarking];
					firstEventID = i;
					firstEventType = TRANSITION;
			}
		}

		if (hasImmediateEnabled) {
			//TODO: Take care of immediate transitions!!
			std::cout << "An immediate transition is enabled." << std::endl;
			fireTransition(model, marking, firstEventID);
			DtrmEvent* e = new DtrmEvent(firstEventType);
			e->id = firstEventID;
			e->preRegionMarking = preMarking;
			e->postRegionMarking = marking;
			dtrmEventList.push_back(e);
			continue;
		}

		for (int i = 0; i < model->N_places; i++) {
			if (model->places[i].type == PT_FLUID) {
				
				//if drift is close to zero nothing changes.
				if (IS_ZERO(drift[model->places[i].idInMarking]))
					continue;

				double f = fluid0[model->places[i].idInMarking];
				double d = drift[model->places[i].idInMarking];
				double fluidtime;

				if (d > ZERO_PREC) 
					fluidtime = (model->places[i].f_bound - f) / d;
				else if (d < -ZERO_PREC) 
					fluidtime = - (f) / d;

				if (fluidtime < firstT){
					firstT = fluidtime;
					firstEventID = i;
					if (d > ZERO_PREC) 
						firstEventType = PLACE_UPPER_BOUNDRY;
					else if (d < -ZERO_PREC)
						firstEventType = PLACE_LOWER_BOUNDRY;
				}
			}
		}

		//if there is no event before max time reached, we are done.
		if (crntTime + firstT > model->MaxTime){
            crntEvent = new DtrmEvent(MAX_TIME_REACHED);
			crntEvent->time = model->MaxTime;
			crntEvent->id = -1;
			crntEvent->preRegionMarking = marking;
			crntEvent->postRegionMarking = NULL;
            dtrmEventList.push_back(crntEvent);
            if (preEvent != NULL)
                preEvent->nextDtrmEvent = crntEvent;
            crntEvent->nextDtrmEvent = 0;

			break;
		}


		marking = advanceMarking(model, marking, 0, firstT, drift, enabled);
		crntTime += firstT;

		if (firstEventType == TRANSITION)
			fireTransition(model, marking, firstEventID);

        crntEvent= new DtrmEvent(firstEventType);
		crntEvent->time = crntTime;
		crntEvent->id = firstEventID;
		crntEvent->preRegionMarking = preMarking;
		crntEvent->postRegionMarking = marking;
		dtrmEventList.push_back(crntEvent);
        if (preEvent != NULL)
            preEvent->nextDtrmEvent = crntEvent;
        preEvent = crntEvent;
	}
}
void TimedDiagram::segmentizeStochasticRegion(Marking* marking, StochasticEvent* eventSeg, double timeBias) {


	Segment* eventLine = eventSeg->timeSegment;
    std::cout << "P2 : " << eventLine->p2.X << " P1: "<< eventLine->p1.X << std::endl;
	if (eventLine->p2.Y > model->MaxTime || eventLine->p2.X > model->MaxTime) {
		std::cout << ("\n Maximum time reached") << std::endl;
		return;
	}

	double start = eventLine->p1.X ;
	double end = eventLine->p2.X;

	int cntFirst;
	int *enabled;
	int *enabledTransitionCache = NULL; //TODO: it should be static? after each event a transition could be enabled or disabled.



	if (enabledTransitionCache == NULL)
		enabledTransitionCache = new int[model->N_transitions];


	checkEnabled(model, marking);
	setActFluidRate(model, marking, start /*- timeBias*/);

	double* clock0, *clock1, *fluid0, *fluid1, *drift;

	enabled = marking->enabling;
	clock0 = marking->clock0;
	clock1 = marking->clock1;
	fluid0 = marking->fluid0;
	fluid1 = marking->fluid1;
	drift = marking->fluidPlaceDeriv;
	cntFirst = 0;

	double firstT = 1.0e100;

	int hasImmediateEnabled;

	std::vector<StochasticEvent*>* potentialEvents = new std::vector<StochasticEvent*>();

	cntFirst = 0;
	firstT = 1.0e100;

	// count the number (and put in cache) the number of enabled transitions
	hasImmediateEnabled = 0;
	for (int i = 0; i < model->N_transitions; i++) {
		if (enabled[i] != 1)
			continue;

		if ((model->transitions[i].type == TT_IMMEDIATE)) {
			if (firstT > 0.0) {
                firstT = 0.0;Region
				enabledTransitionCache[0] = i;
				cntFirst = 1;
				hasImmediateEnabled = 1;
			} else {
				enabledTransitionCache[cntFirst] = i;
				cntFirst++;
			}
		} else if ((model->transitions[i].type == TT_DETERMINISTIC)
			&& (hasImmediateEnabled == 0)) {
				enabledTransitionCache[cntFirst] = i;
				cntFirst++;
		}
	}

	if (hasImmediateEnabled) {
		//TODO: Take care of immediate transitions!!
		std::cout << "An immediate transition is enabled." << std::endl;

	} else {
		for (int i = 0; i < cntFirst; i++) {
			int trId = enabledTransitionCache[i];
			/**
			* A deterministic transition equation is determined by following:
			* \f$ \Delta t = T_D - (as + b), \Delta t = t - (\alpha s + \beta + [timeBias]) \f$
			* \f$ (as + b) \f$ --> is the current clock value of transition.
			* \f$ (\alpha s + \beta) \f$ --> is the event line equation passed as the argument.
			* \f$ t \f$ --> is the current time (vertical axis of the diagram).
			* Final equation would be :
			*  	\f$ t = (\alpha - a)s + (T_D + \beta - b + [timeBias]).
			* A timeBias should be subtracted from \f$ t \f$ in case this function is not called (in generate diagram) with the start from origin.
			*/
			double a = clock1[model->transitions[trId].idInMarking];
			double b = clock0[model->transitions[trId].idInMarking];

			//TODO:(IMPORTANT) fix the end of each new segment with coinciding the eventLine.
			Segment * detTransSeg = new Segment(eventLine->a - a,
				model->transitions[trId].time + eventLine->b - b /*+ timeBias*/,
				start, end);
			StochasticEvent* detTransEvent = new StochasticEvent(detTransSeg, TRANSITION);
			detTransEvent->id = enabledTransitionCache[i];
			detTransEvent->preRegionMarking = marking;
            detTransSeg->print();
			potentialEvents->push_back(detTransEvent);

		}
		for (int i = 0; i < model->N_places; i++) {
			if (model->places[i].type == PT_FLUID) {

				//if drift is close to zero nothing changes.
				if (IS_ZERO(drift[model->places[i].idInMarking]))
					continue;


				/**
				* A fluid place equation is determined by following:
				* \f$ B - (as +b) = d\Delta t , \Delta t = t - (\alpha s + \beta) \f$
				* \f$ (as + b) \f$ --> is the current fluid level in the place.
				* \f$ (\alpha s + \beta) \f$ --> is the event line equation passed as the argument.
				* \f$ t \f$ --> is the current time (vertical axis of the diagram).
				* Final equation would be :
				* For d > 0 : \f$ t = (\alpha - \frac{a}{d})s + (\frac{-b + B}{d} + \beta).
				* For d < 0	: \f$ t = (\alpha + \frac{a}{|d|})s + (\frac{b}{|d|} + \beta). 
				*
				* Note: If the drift is negative the B is the lower bound and vice versa. 
				* and all d terms should be absolute.
				* ***********************************************************************************************************************
				* IMPORTANT NOTE:																										*
				* the s value used above is in the frame which its origin is at the point (gTrEnabledTime, gTrEnabledTime).				*
				* so in order to obtain the equation in the main frame we shoud replace all s and t should be minused by gTrEnabledTime.*
				* so the new equations would be:																						*
				* For d > 0 : \f$ t = (\alpha - \frac{a}{d})s + (\frac{-b + B}{d} + \beta) +											*
				*					  gTrEnabledTime(1 - \alpha + \frac{a}{d}).															*
				* For d < 0	: \f$ t = (\alpha + \frac{a}{|d|})s + (\frac{b}{|d|} + \beta) +												*
				*                     gTrEnabledTime(1 - \alpha - \frac{a}{|d|}).														*
				*************************************************************************************************************************
				*/

				double a = fluid1[model->places[i].idInMarking];
				double b = fluid0[model->places[i].idInMarking];
				double d = drift[model->places[i].idInMarking];

				StochasticEvent* placeEvent = new StochasticEvent();
				Segment* placeSeg;
				if (d > ZERO_PREC) {

					placeSeg = new Segment(eventLine->a - a / d,
						(model->places[i].f_bound - b) / d + eventLine->b,
						//+ gTrEnabledTime*(1- eventLine->a + a / d),
						/*+ timeBias*/
						start, end);
					placeEvent->eventType = PLACE_UPPER_BOUNDRY;
				} else if (d < -ZERO_PREC) {

					std::cout << a << ", " << std::abs(d) + eventLine->b << ", " << start << ", " <<  end << std::endl;
					placeSeg = new Segment(eventLine->a + a / std::abs(d),
						b / std::abs(d) + eventLine->b,
						//+ gTrEnabledTime*(1- eventLine->a - a /abs(d))/*+ timeBias*/,
                        start, end);
                    placeSeg->print();
					placeEvent->eventType = PLACE_LOWER_BOUNDRY;
				}


				//regarding some 
				if (eventSeg->eventType ==  placeEvent->eventType && eventSeg->id == i){
					drift[model->places[i].idInMarking] = 0;
					continue;
				}

				placeEvent->timeSegment = placeSeg;
				
				placeEvent->id = i;
				placeEvent->preRegionMarking= marking;
				potentialEvents->push_back(placeEvent);
				cntFirst++;
			}
		}

		if (cntFirst > 0) { // if there is at least an event that can happen

            eventLine->print();
			std::vector<StochasticEvent*> *nextEvents = new std::vector<StochasticEvent*>();
			computeNextEvents(potentialEvents, eventLine, nextEvents);

			potentialEvents->clear();
			delete potentialEvents;

            //STDRegion* region = new STDRegion(nextEvents, eventLine);
			//region->marking = marking;
			//region->timeBias = timeBias;

			//regionList.push_back(region);

            createAddRegions(nextEvents, eventSeg, marking);


            for (int i = 0; (unsigned)i < nextEvents->size(); i++) {

				//if (IS_ZERO(nextEvents->at(i)->timeSegment->p1.X - nextEvents->at(i)->timeSegment->p2.X))
				//	continue;

				Marking *newMarking;
				//TODO: Here we should check for multiple possible events.

				double t1 = nextEvents->at(i)->timeSegment->a - eventLine->a;
				double t0 = nextEvents->at(i)->timeSegment->b - eventLine->b /*- timeBias*/;


				//in case that no time advancment is needed!
				//if (IS_ZERO(t1) && IS_ZERO(t0))
				//	continue;


				newMarking = advanceMarking(model, marking, t1, t0, drift, enabled);

				if (nextEvents->at(i)->eventType == TRANSITION)
					fireTransition(model, newMarking, nextEvents->at(i)->id);

				nextEvents->at(i)->postRegionMarking = newMarking;

				segmentizeStochasticRegion(newMarking, nextEvents->at(i), timeBias);
			}
		} else {

			Segment* maxSeg = new Segment(0, model->MaxTime, eventLine->p1.X, eventLine->p2.X);
			StochasticEvent* maxTimeReached = new StochasticEvent(maxSeg, MAX_TIME_REACHED);
			maxTimeReached->preRegionMarking = marking;
			maxTimeReached->postRegionMarking = marking;
			maxTimeReached->id = -1;
			std::vector<StochasticEvent*> *maxTimeEvents = new std::vector<StochasticEvent*>();
			maxTimeEvents->push_back(maxTimeReached);
            Region* region = new Region(maxTimeEvents, eventLine);
			region->marking = marking;
			region->timeBias = timeBias;

            if (eventSeg->preRegion != 0)
                eventSeg->preRegion->successors->push_back(region);
            else //if we have entered from the deterministic are to this new region.
                eventSeg->preDtrmEvent->nextRegions->push_back(region);

			regionList.push_back(region);
		}

	}

}

void TimedDiagram::computeNextEvents(std::vector<StochasticEvent*> * potentialEvents, Segment *uSegment, std::vector<StochasticEvent*> *nextEvents) {

	std::sort(potentialEvents->begin(), potentialEvents->end(),
		StochasticEvent::greaterSlopeFirst);
	Segment* workingSeg = new Segment(uSegment->p1, uSegment->p2);
	minLines(potentialEvents, workingSeg, nextEvents);
}

void TimedDiagram::minLines(std::vector<StochasticEvent*> * potentialEvents, Segment *uSegment, std::vector<StochasticEvent*> * nextEvents){

	double start = uSegment->p1.X;
    double end = uSegment->p2.X;

	double minStart = INF;
	int minIndex;
    for (int i = 0; (unsigned)i < potentialEvents->size(); i++) {
        std::cout << "a: " << potentialEvents->at(i)->timeSegment->a << std::endl;
		if (potentialEvents->at(i)->timeSegment->getY(start) < minStart) {
			minStart = potentialEvents->at(i)->timeSegment->getY(start);
			minIndex = i;
		}
	}

	int crntIndex = minIndex;

	Point p1, p2;
	int nextIndex = crntIndex;

	p1.X = start;
	p1.Y = potentialEvents->at(crntIndex)->timeSegment->getY(start);
	while ((unsigned)crntIndex < potentialEvents->size()) {

		p2 = potentialEvents->at(crntIndex)->timeSegment->p2;  //this is the first point that crntIndex line is intersecting with.

		for (int i = crntIndex + 1; (unsigned)i < potentialEvents->size(); i++) {
			Point p;
			if (potentialEvents->at(crntIndex)->timeSegment->intersect(*potentialEvents->at(i)->timeSegment, p))
				if (p.Y <= p2.Y) {
					p2 = p;
					nextIndex = i;
				}
		}


		Segment* newSegment = new Segment(potentialEvents->at(crntIndex)->timeSegment->a, 
			potentialEvents->at(crntIndex)->timeSegment->b, p1.X, p2.X);

		StochasticEvent* sEvent = new StochasticEvent(potentialEvents->at(crntIndex));
		sEvent->timeSegment = newSegment;

		nextEvents->push_back(sEvent);

        std::cout << "minIndex:"  << minIndex << std::endl;
        std::cout << "crntIndex:" << crntIndex << std::endl;
        std::cout << "potentialEvents->size():" << potentialEvents->size()<< std::endl;
        uSegment->print();
		//if there is no intersection we are done.
		if (crntIndex == nextIndex) {
			Point uPoint;

			//if we have an intersection with the underlying segment, we should consider it.
            potentialEvents->at(crntIndex)->timeSegment->print();
			if (potentialEvents->at(crntIndex)->timeSegment->intersect(*uSegment, uPoint) && uPoint != uSegment->p2 && uPoint != uSegment->p1){
				nextEvents->back()->timeSegment->p2 = uPoint;
				uSegment->p1 = uPoint;
				potentialEvents->erase(potentialEvents->begin() + crntIndex);
				//I think potentialEvents->at(crntIndex) should be omitted.
                uSegment->print();
                std::cout << "potentialEvents->size():" << potentialEvents->size()<< std::endl;
				minLines(potentialEvents, uSegment, nextEvents);
			}

			break; 
		}

		crntIndex = nextIndex;
		p1 = p2;
	}
}

void TimedDiagram::createAddRegions(std::vector<StochasticEvent*> * eventList, StochasticEvent * preEvent, Marking* marking){
	int index1 = 0; 
	int index2 = 0;
    Segment* lowerBoundray = preEvent->timeSegment;
	Point p1 = lowerBoundray->p1;
	Point p2 = lowerBoundray->p2;
	for (int i = 0; (unsigned)i < eventList->size(); i++){
		index2 = i;
		if (eventList->at(i)->timeSegment->p2.Y == lowerBoundray->getY(eventList->at(i)->timeSegment->p2.X) ||
			eventList->at(i)->timeSegment->p2.X == lowerBoundray->p2.X)
		{
			//in case we are here according to the second condition.
			p2.X = eventList->at(i)->timeSegment->p2.X;
			p2.Y = lowerBoundray->getY(eventList->at(i)->timeSegment->p2.X);

			std::vector<StochasticEvent*>* events = new std::vector<StochasticEvent*>();
			for (int j = index1; j <= index2; j++)
				events->push_back(eventList->at(j));

			Segment* lb = new Segment(p1, p2);
            Region* region = new Region(events, lb);
			region->marking = marking;

			regionList.push_back(region);

			//if we have entered from an stochastic region to this new region.
            if (preEvent->preRegion != 0)
                preEvent->preRegion->successors->push_back(region);
            else //if we have entered from the deterministic are to this new region.
                preEvent->preDtrmEvent->nextRegions->push_back(region);

			for (int j = index1; j <= index2; j++)
				eventList->at(j)->preRegion = region;

			p1 = p2;
			index1 = index2 + 1;
		}
	}
}

double TimedDiagram::calProbAtTime(double time, double (*sPdfInt)(double), bool (*isPropHolds)(Model*, Marking*, double t0, double t1, double& , double&, unsigned int _pIndex, double _amount), unsigned int _pIndex, double _amount){
	//TODO: by sorting region in more genuine way this function could be more effecient.

    double s1, s2;
	double prob = 0;
	Point p1, p2;
	
	if (time > gTrEnabledTime) {
		//stochastic part : only if at the given time g-transition could have been enabled.
		/**
		* IMPORTANT note: we have computed everything for the stochastic part in the frame with origin at (gTrEnabledTime, gTrEnabledTime)
		* So we should move to that frame.
		*/
		double sFrameTime = time - gTrEnabledTime; 

		Segment timeSeg(0, sFrameTime , 0, model->MaxTime - gTrEnabledTime);
		for (int i = 0; (unsigned)i < regionList.size(); i++){
			//s1 and s2 are the validity interval for which the probability holds, returned by the function iPropHolds.
			//std::cout << "i=" << i << "-";
			if (regionList[i]->intersect(timeSeg, p1, p2)){
				s1 = p1.X; s2 = p2.X;
				double t0 = sFrameTime - regionList[i]->lowerBoundry->b /*- regionList[i]->timeBias*/;
				double t1 = - regionList[i]->lowerBoundry->a;
                if (isPropHolds(model, regionList[i]->marking, t0, t1, s1, s2, _pIndex, _amount)){
                    //if (s1 != 0 && s2 != 0) {
//                    std::cout << "stochastic -" << " time: " << time <<" s1: " << s1 << " s2: " << s2 << " ";
//                    std::cout << prob+sPdfInt(s2, fv) - sPdfInt(s1, fv) << " = " << prob << "+" << sPdfInt(s1,fv) << "-" << sPdfInt(s2,fv) << " = " << sPdfInt(s2, fv) - sPdfInt(s1, fv) << std::endl;
                    prob += sPdfInt(s2) - sPdfInt(s1);
                    //std::cout << sPdfInt(s1, fv) << "-" << sPdfInt(s2, fv) << std::endl;
//                    std::cout << sPdfInt(s1, fv) << "-" << sPdfInt(s2, fv) << " ";
//                    std::cout << fabs(sPdfInt(s1, fv) - sPdfInt(s2, fv)) << " ";
                    //std::cout << "prob" << prob << std::endl;
                    //}
				}
			}
		}
	}

	//determinestic part after g-transition firing
	int cc;
	for (cc = 0; (unsigned)cc < dtrmEventList.size() && time > dtrmEventList[cc]->time ; cc++);
	s1 = time > gTrEnabledTime ? time : 0;
    s2 = INFINITY; //model->MaxTime; // THIS SEEMS WRONG. THIS SHOULD BE INFINITY TO INTEGRATE OVER.
//    std::cout << "deterministic -" << " time: " << time << " s1: " << s1 << " s2: " << s2 << " ";
//    std::cout << prob + sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2, fv) - sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1, fv) << " = " << prob << " + " << sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1,fv) << "-" << sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2,fv) << " = " << sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2, fv) - sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1, fv) << std::endl;

    double t;
	if (cc == 0)
		t = time;
	else 
		t = time - dtrmEventList[cc - 1]->time;
    if (isPropHolds(model, dtrmEventList[cc]->preRegionMarking, t , 0, s1, s2, _pIndex, _amount)) {
        //if (s1 != 0 && s2 != 0) {
            prob += sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2) - sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1);
        //}
    }

	return prob;
}

void TimedDiagram::saveDiagram(std::string filename){

	const int row = model->MaxTime * scale;
	const int col = model->MaxTime * scale;

	debugImage = cv::Mat(row, col, CV_8UC3, cv::Scalar::all(255));


	for (int i = 0; (unsigned)i < regionList.size(); i++){
		drawSegmet(debugImage, *regionList[i]->leftBoundry, scale);
		drawSegmet(debugImage, *regionList[i]->rightBoundry, scale);
		drawSegmet(debugImage, *regionList[i]->lowerBoundry, scale);
		for (int j = 0; j < regionList[i]->eventSegments->size(); j++)
			drawSegmet(debugImage, *regionList[i]->eventSegments->at(j)->timeSegment, scale);

		cv::Point p(scale*(regionList[i]->lowerBoundry->p1.X + regionList[i]->lowerBoundry->p2.X)/2,
			scale*(regionList[i]->lowerBoundry->p1.Y + regionList[i]->lowerBoundry->p2.Y)/2);
//		char rn[10];
//		sprintf(rn, "%d", i);
//		cv::putText(debugImage, std::string(rn), p, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255));
	}

	for (int i = 0; (unsigned)i < dtrmEventList.size(); i++){
		cv::line(debugImage, cv::Point((dtrmEventList[i]->time/* - gTrEnabledTime*/) * scale, (dtrmEventList[i]->time /* - gTrEnabledTime*/)* scale),
			cv::Point(col, (dtrmEventList[i]->time /* - gTrEnabledTime*/)* scale), cv::Scalar(0, 255, 0), 1);
	}

	cv::line(debugImage, cv::Point(0, 0), cv::Point(row, col), cv::Scalar(255, 0, 0), 2);

	cv::Mat flipped;
	cv::flip(debugImage, flipped, 0);
	cv::imwrite(filename + ".jpg", flipped);

	//cv::Mat im2show;
	//cv::resize(diagram, im2show, cv::Size(1000, 1000));
	//cv::imshow("regions", im2show);
	//cv::waitKey();
}

void TimedDiagram::drawSegmet(cv::Mat & image, Segment& seg, const int scale){
	cv::Point p1(seg.p1.X * scale, seg.p1.Y * scale);
	cv::Point p2(seg.p2.X * scale, seg.p2.Y * scale);

	cv::line(image, p1, p2, cv::Scalar(0, 255, 0), 1);
}

}
