/**
 * @file ModelChecker.cpp
 * @author B.F. Postema
 * @brief The model checker class for model checking HPNGs against STL.
 */

#include "ModelChecker.h"

using namespace std;

namespace model {

IntervalSet* ModelChecker::visitStocRegion(Region* region, Formula* psi1, Formula* psi2, IntervalSet* potentialSatSet, double t, Interval bound) {
	//TODO:[IMPORTANT assumption] t=0 and bound = [0, MAX_TIME]

	if (region == NULL) return NULL;
	//if line t is within the region


	//if line t is NOT within the region
	Polygon *psi1_poly = geometryHelper->createPolygon(region, psi1);
	Polygon *psi2_poly = geometryHelper->createPolygon(region, psi2);
	psi1_poly = geometryHelper->cropPolygon(psi1_poly, bound.end, DOWN);
	psi2_poly = geometryHelper->cropPolygon(psi2_poly, bound.end, DOWN);

    //if (psi1_poly != NULL) psi1_poly->print();
    //if (psi2_poly != NULL) psi2_poly->print();

	if (!debugImage.empty()){
		if (psi1_poly != NULL) geometryHelper->drawPolygon(debugImage, psi1_poly, scale, cv::Scalar(0, 0, 255));
		if (psi2_poly != NULL) geometryHelper->drawPolygon(debugImage, psi2_poly, scale, cv::Scalar(255, 255, 0));
		cv::Mat flipped;
		cv::flip(debugImage, flipped, 0);
		cv::imshow("test", flipped);
		cv::waitKey(0);
	}

	// First interval set.
	if (psi2_poly != NULL){
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

            satSet = satSet->unionWith(geometryHelper->getIntersectionIntervals(psi2_poly, seg));
            delete seg;
		}
	}

	if (psi1_poly != NULL){
		IntervalSet* psi1_Int = new IntervalSet();
		//Second interval set.
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

			psi1_Int = psi1_Int->unionWith(geometryHelper->getIntersectionIntervals(psi1_poly, seg));
			//In general case here the psi1_poly should be reformed.
            delete seg;
		}

		if (psi2_poly != NULL){
			IntervalSet* psi12_Int = geometryHelper->getIntersectionIntervals(psi1_poly, psi2_poly);
            if (psi12_Int != NULL){
                satSet = satSet->unionWith(psi1_Int->intersect(psi12_Int));
			}
		}

        //Third Set of intervals.
        for (unsigned int i = 0; i < region->successors->size(); i++){
            IntervalSet* potPsi1_int = geometryHelper->getIntersectionIntervals(psi1_poly, region->successors->at(i)->lowerBoundry);
            if (potPsi1_int != NULL){
                potPsi1_int = potPsi1_int->minus(satSet);
                potPsi1_int = potPsi1_int->intersect(psi1_Int);
                if (!potPsi1_int->isEmpty())
                     satSet = satSet->unionWith(visitStocRegion(region->successors->at(i), psi1, psi2, potPsi1_int, t, bound));
            }
        }
	}

	return satSet;

}

bool ModelChecker::setVariables() {
    char *argFinder;
    int a;

    int distr = this->model->transitions[gTransitionId(this->model)].df_distr;

    //char *argument = this->model->transitions[gTransitionId(this->model)].df_argument;
    char argument[strlen(this->model->transitions[gTransitionId(this->model)].df_argument)];
    strcpy(argument, this->model->transitions[gTransitionId(this->model)].df_argument);
    //memcpy (argument, this->model->transitions[gTransitionId(this->model)].df_argument, sizeof (char *));
    switch (distr)
    {
    case Exp: this->setLambda(atof(argument));
        this->setDistr(Exp);
        break;
    case Gamma: this->setLambda(atof(argument));
        this->setDistr(Gamma);
        break;
    case Uni:
        argFinder = strtok(argument,",");
        if (argFinder == NULL) {
            guic->addError(QString("ATransition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        a = atof(argFinder);
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addError(QString("BTransition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        this->setAB(a,atof(argFinder));
        this->setDistr(Uni);
        break;
    case Gen:
        this->setF(evaluator_create (argument));
        this->setDistr(Gen);
        break;
    case FoldedNorm:
        argFinder = strtok(argument,",");
        if (argFinder == NULL) {
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        this->setMu(atof(argFinder));
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        this->setSigma(atof(argFinder));
        this->setDistr(FoldedNorm);
        break;
    case Norm:
        argFinder = strtok(argument,",");
        if (argFinder == NULL) {
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        this->setMu(atof(argFinder));
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        this->setSigma(atof(argFinder));
        this->setDistr(Norm);
        break;
    case Dtrm:
        this->setDtrm(atof(argument));
        this->setDistr(Dtrm);
        break;
    }

    return true;
}

ModelChecker::~ModelChecker() {
	// TODO Auto-generated destructor stub
}

IntervalSet* ModelChecker::visitDtrmRegion(DtrmEvent* dtrmRegion, Formula* psi1, Formula* psi2, double t, Interval bound) {
	if (dtrmRegion->time >= bound.end || dtrmRegion->time >= model->MaxTime || dtrmRegion == NULL)
		return satSet;

	Direction d1 = UP;
	Direction d2 = UP;
	double t1 = INF;
	double t2 = INF;

	RegionState state1 = geometryHelper->getTimeAndDirection(dtrmRegion, psi1, t1, d1);
	RegionState state2 = geometryHelper->getTimeAndDirection(dtrmRegion, psi2, t2, d2);

	if (!debugImage.empty()){
		geometryHelper->drawVerticalLine(debugImage, t1, d1, scale, cv::Scalar(0, 0, 255));
		geometryHelper->drawVerticalLine(debugImage, t2, d2, scale, cv::Scalar(255, 255, 0));
		cv::Mat flipped;
		cv::flip(debugImage, flipped, 0);
        cv::imshow("temp", flipped);
		cv::waitKey(0);

	}


	if (state2 != NONE && d2 == DOWN){
		satSet = satSet->unionWith(Interval(dtrmRegion->time, model->MaxTime));
		return satSet;
	}

	if (state1 != ENTIRE && d1 == UP)
		return NULL;

	if (state2 != NONE && t2 <= t1)
		satSet = satSet->unionWith(Interval(t2, model->MaxTime));
	else if(state1 == ENTIRE && (state2 != NONE && d2 == UP))
		satSet = satSet->unionWith(Interval(t2, model->MaxTime));
	else if(state1 == ENTIRE)
        satSet = satSet->unionWith(visitDtrmRegion(dtrmRegion->nextDtrmEvent, psi1, psi2, t, bound));

	IntervalSet* iSet = new IntervalSet();
    iSet->intervals.push_back(Interval(dtrmRegion->time, (state1 == ENTIRE)? dtrmRegion->nextDtrmEvent->time : t1));
	iSet = iSet->minus(satSet);

    for (unsigned int i =0; i < dtrmRegion->nextRegions->size(); i++)
        satSet = satSet->unionWith(visitStocRegion(dtrmRegion->nextRegions->at(i), psi1, psi2, iSet, t, bound));

	return satSet;

}

bool ModelChecker::until(IntervalSet *&res, Formula* psi1, Formula* psi2, Interval bound) {
	// TODO: Currently I have assumed that the given bound starts at 0 ie. T_1 = 0.
    // TODO: Traverse through the formulas retrieving the polygons. Then in the end retrieve the intervalset.

    satSet = new IntervalSet();

    double t = ttc; // - std->getTrEnabledTime();
    //std::cout << "t" << t << std::endl;
	Line timeLine(0, t);
	Point p1, p2;

	if (!debugImage.empty())
		geometryHelper->drawVerticalLine(debugImage, bound.end, UP, scale, cv::Scalar(0, 0, 0));

	//iterating over all deterministic events.
	// the last event regards the maximum time reached. so should not be considered.
	for (unsigned int i = 0; i < std->dtrmEventList.size() - 1; i++){
        //std::cout << i << std::endl;
        if (std->dtrmEventList[i]->time <= ttc && std->dtrmEventList[i]->nextDtrmEvent->time > ttc){
			satSet = satSet->unionWith(visitDtrmRegion(std->dtrmEventList[i], psi1, psi2,t, bound));
			break;
		}
	}

	//iterating over all stochastic regions.
	for (unsigned int i = 0; i < std->regionList.size(); i++){
		if (std->regionList[i]->intersect(timeLine, p1, p2)){
			IntervalSet* potSet = new IntervalSet();
			potSet->intervals.push_back(Interval(p1.X, p2.X));

			satSet = satSet->unionWith(visitStocRegion(std->regionList[i], psi1, psi2, potSet, t, bound));
		}
	}
    res = satSet;
    return true;
}

//IntervalSet* ModelChecker::tempUntil(Formula* psi1, Formula* psi2, Interval bound, double time) {
//    // TODO: Currently I have assumed that the given bound starts at 0 ie. T_1 = 0.

////	if (satSet != NULL)
////		satSet->clear();
////	else
//    satSet = new IntervalSet();

//    double t = time; // - std->getTrEnabledTime();
//    std::cout << "t" << t << std::endl;
//    Line timeLine(0, t);
//    Point p1, p2;

//    if (!debugImage.empty())
//        geometryHelper->drawVerticalLine(debugImage, bound.end, UP, scale, cv::Scalar(0, 0, 0));


////	satSet->print();
//    //iterating over all deterministic events.
//    // the last event regards the maximum time reached. so should not be considered.
//    for (unsigned int i = 0; i < std->dtrmEventList.size() - 1; i++){
//        std::cout << i << std::endl;
//        if (std->dtrmEventList[i]->time <= time && std->dtrmEventList[i]->nextDtrmEvent->time > time){
//            satSet = satSet->unionWith(visitDtrmRegion(std->dtrmEventList[i], psi1, psi2,t, bound));
////			satSet->print();
//            break;
//        }
//    }

//    //iterating over all stochastic regions.
//    for (unsigned int i = 0; i < std->regionList.size(); i++){
//        if (std->regionList[i]->intersect(timeLine, p1, p2)){
//            IntervalSet* potSet = new IntervalSet();
//            potSet->intervals.push_back(Interval(p1.X, p2.X));

//            satSet = satSet->unionWith(visitStocRegion(std->regionList[i], psi1, psi2, potSet, t, bound));
//        }
//    }



//    return satSet;
//}

bool ModelChecker::iSetTT(IntervalSet *&res) {
    res = new IntervalSet();
    Interval I(0, INFINITY);
    res->intervals.push_back(I);
    //std::cout << "IntervalSet result TT : ";
    //res->print(std::cout);
    //std::cout << std::endl;
    return true;
}

bool ModelChecker::iSetNeg(IntervalSet *&res, IntervalSet* iset1) {
    //std::cout << "IntervalSet 1 : ";
    //iset1->print(std::cout);
    //std::cout << std::endl;
    if(!iSetTT(res)) return false;
    res->minus(iset1);
    //std::cout << "IntervalSet result NEG : ";
    //res->print(std::cout);
    //std::cout << std::endl;
    return true;
}

bool ModelChecker::iSetAnd(IntervalSet *&res, IntervalSet* iset1, IntervalSet* iset2) {
    //std::cout << "IntervalSet 1 : ";
    //iset1->print(std::cout);
    //std::cout << std::endl;
    //std::cout << "IntervalSet 2 : ";
    //iset2->print(std::cout);
    //std::cout << std::endl;
    res = iset1->intersect(iset2);
    //std::cout << "IntervalSet result AND : ";
    //res->print(std::cout);
    //std::cout << std::endl;
    return true;
}

bool ModelChecker::iSetAtomDis(IntervalSet *&res, AtomDisFormula* psi1) {
    res = this->calcAtomDisISetAtTime(ttc, psi1->getPlaceIndex(), psi1->getN());
    return true;
}

bool ModelChecker::iSetAtomCont(IntervalSet *&res, AtomContFormula* psi1) {
    res = this->calcAtomContISetAtTime(ttc, psi1->getPlaceIndex(), psi1->getC());
    return true;
}

double ModelChecker::calcProb(IntervalSet* iSet, double shift) {
    double prob = 0;
    for(std::vector<Interval>::size_type i = 0; i != iSet->intervals.size(); i++) {
        switch (this->distr)
        {
        case Exp: prob += ( this->scdfExp(iSet->intervals[i].end - shift) - this->scdfExp(iSet->intervals[i].start - shift));
        break;
        case Uni: prob += ( this->scdfUni(iSet->intervals[i].end - shift) - this->scdfUni(iSet->intervals[i].start - shift));
        break;
        case Gen: prob += ( this->scdfGen(iSet->intervals[i].end - shift) - this->scdfGen(iSet->intervals[i].start - shift));
        break;
        case Gamma: prob += ( this->scdfGamma(iSet->intervals[i].end - shift) - this->scdfGamma(iSet->intervals[i].start - shift));
        break;
        case Norm: prob += ( this->scdfNormal(iSet->intervals[i].end - shift) - this->scdfNormal(iSet->intervals[i].start - shift));
        break;
        case FoldedNorm: prob += ( this->scdfFoldedNormal(iSet->intervals[i].end - shift) - this->scdfFoldedNormal(iSet->intervals[i].start - shift));
        break;
        case Dtrm: prob += ( this->scdfDtrm(iSet->intervals[i].end - shift) - this->scdfDtrm(iSet->intervals[i].start - shift));
        break;
        }
    }
    return prob;
}

bool ModelChecker::parseFML(Formula *&fullFML, QString rawFormula) {
    /*
     * Parse the STL formula
     * Use Flex + Bison
     * Flex (Lexer) : Scans the lines and transforms it into tokens.
     * Bison (Parser) : Creates the datastructure of the AST and gives initial values to the different type of formulas.
     */
    struct yy_buffer_state* bufferState;
    bufferState = yy_scan_string(rawFormula.toUtf8().data());
    yyparse();
    yy_delete_buffer(bufferState);
//    yylex_destroy();
    reset_lexer();

    if (getColError() == -1) {
        //guic->addText("The formula is succesfully parsed.");
    } else {
        guic->addError(QString("Syntax error for character %1 in formula %2: The formula could not be parsed.").arg(getColError()).arg(rawFormula).toStdString());
        return false;
    }
    fullFML = parser_results();

return true;
}

bool ModelChecker::traverseFML(bool &res, Formula *fullFML) {
    /*
     * TODO: Traverse through the STL formula
     * Strategies is post-order
     */
    IntervalSet *iSetLeft = new IntervalSet();
    IntervalSet *iSetRight = new IntervalSet();
    res = false;
    if (fullFML->getLeftChild() != 0) { if (!traverseISetFML(iSetLeft,fullFML->getLeftChild())) return false; }
    if (fullFML->getRightChild() != 0) { if (!traverseISetFML(iSetRight,fullFML->getRightChild())) return false; }

    /*
     * Actual actions of the formulas
     */
    if (fullFML->getType() == PROB) {
        double resProb = calcProb(iSetLeft,0.00);
        std::cout << "Probability:" << resProb << std::endl;
        guic->addText(QString("Probability : %1").arg(resProb).toStdString());
        switch (((ProbFormula*)fullFML)->getOp())
        {
        case LEQ :
            res = (resProb <= ((ProbFormula*)fullFML)->getProb());
            break;
        case LESSER :
            res = (resProb < ((ProbFormula*)fullFML)->getProb());
            break;
        case GREATER :
            res = (resProb > ((ProbFormula*)fullFML)->getProb());
            break;
        case GEQ :
            res = (resProb >= ((ProbFormula*)fullFML)->getProb());
            break;
        }
    }
    delete iSetLeft;
    delete iSetRight;
    return true;
}

bool ModelChecker::traverseISetFML(IntervalSet *&res, Formula *fullFML) {
    /*
     * TODO: Traverse through the STL formula
     * Strategies is post-order
     */
    res = 0;
    IntervalSet *iSetLeft = new IntervalSet();
    IntervalSet *iSetRight = new IntervalSet();
    if (fullFML->getLeftChild() != 0) { if (!traverseISetFML(iSetLeft, fullFML->getLeftChild())) return false; }
    if (fullFML->getRightChild() != 0) { if (!traverseISetFML(iSetRight,fullFML->getRightChild())) return false; }
    /*
     * Actual actions of the formulas
     */

    if (iSetLeft == 0 || iSetRight == 0) {
        return false;
    }

    switch (fullFML->getType())
    {
    case DISCRETE:
        /*
        * Since parser does not know anything about the model the placeIndeces need to be discovered.
        */
        if (((AtomDisFormula*)fullFML)->setPlaceIndex(model)) {
            if(!iSetAtomDis(res,(AtomDisFormula*)fullFML)) return false;
        } else {
            guic->addError(QString("There is an incorrect fluid place name called '%1' in your formula.").arg(((AtomDisFormula*)fullFML)->getPlaceName()).toStdString());
            res = 0;
        }
    break;
    case CONTINUOUS:
        /*
        * Since parser does not know anything about the model the placeIndeces need to be discovered.
        */
        if (((AtomContFormula*)fullFML)->setPlaceIndex(model)) {
            if (!iSetAtomCont(res,(AtomContFormula*)fullFML)) return false;
        } else {
            guic->addError(QString("There is an incorrect fluid place name called '%1' in your formula.").arg(((AtomContFormula*)fullFML)->getPlaceName()).toStdString());
            res = 0;
        }
    break;
    case TT: if(!iSetTT(res)) return false;
    break;
    case AND:
        if (fullFML->getLeftChild() != 0 && fullFML->getRightChild() != 0) {
            IntervalSet *iSetLeft = new IntervalSet();
            if(!traverseISetFML(iSetLeft,fullFML->getLeftChild())) return false;
            IntervalSet *iSetRight = new IntervalSet();
            if(!traverseISetFML(iSetRight,fullFML->getRightChild())) return false;
            if(!iSetAnd(res,iSetLeft,iSetRight)) return false;
            delete iSetLeft;
            delete iSetRight;
        }
    break;
    case NEG:
        if (fullFML->getLeftChild() != 0) {
            IntervalSet *iSetLeft = new IntervalSet();
            if(!traverseISetFML(iSetLeft,fullFML->getLeftChild())) return false;
            if (!iSetNeg(res,iSetLeft)) return false;
            delete iSetLeft;
        }
    break;
    case UNTIL:
        if (fullFML->getLeftChild() != 0 && fullFML->getRightChild() != 0) {
            Interval bound = ((UntilFormula*)fullFML)->getBound();
            /*
             * Start the Until procedure with polygons.
             */
            if(!until(res,fullFML->getLeftChild(),fullFML->getRightChild(),bound)) return false;
        }
    break;
    default:
        guic->addError("An unknown formula is detected.");
        res = 0;
    break;
    }

    delete iSetLeft;
    delete iSetRight;
    return res;
}

IntervalSet* ModelChecker::calcAtomDisISetAtTime(double time, int pIndex, double amount) {

    double s1, s2;
    IntervalSet *iSet = new IntervalSet();
    Point p1, p2;

    /* First iterate over all the regions in the stochastic area. */
    if (time > TimedDiagram::getInstance()->getTrEnabledTime()) {
        for (int i = 0; (unsigned)i < TimedDiagram::getInstance()->regionList.size(); i++) {
            double sFrameTime = time - TimedDiagram::getInstance()->getTrEnabledTime();

            Segment timeSeg(0, sFrameTime , 0, model->MaxTime - TimedDiagram::getInstance()->getTrEnabledTime());
            if (TimedDiagram::getInstance()->regionList[i]->intersect(timeSeg, p1, p2)) {
                s1 = p1.X; s2 = p2.X;
                if (TimedDiagram::getInstance()->regionList[i]->marking->tokens[model->places[pIndex].idInMarking] == amount) {
                    //std::cout << "Interval : ["<< s1 << "," << s2 << "]" << std::endl;
                    Interval I(s1, s2);
                    //std::cout << "Interval : [" << I.end << "," << I.start << "]" << std::endl;
                    iSet->intervals.push_back(I);
                    //iSet->print(std::cout);
                }
            }
        }
    }

    /* Second decide if the deterministic regions hold or not. */
    int cc;
    for (cc = 0; (unsigned)cc < TimedDiagram::getInstance()->dtrmEventList.size() && time > TimedDiagram::getInstance()->dtrmEventList[cc]->time ; cc++);
    s1 = time > TimedDiagram::getInstance()->getTrEnabledTime() ? time : 0;
    s2 = INFINITY;/*model->MaxTime;*/ // Rather infinity right?

    double t;
    if (cc == 0)
        t = time;
    else
        t = time - TimedDiagram::getInstance()->dtrmEventList[cc - 1]->time;
    if (TimedDiagram::getInstance()->dtrmEventList[cc]->preRegionMarking->tokens[model->places[pIndex].idInMarking] == amount) {
        Interval I(time > TimedDiagram::getInstance()->getTrEnabledTime() ? s1 - TimedDiagram::getInstance()->getTrEnabledTime() : s1, time > TimedDiagram::getInstance()->getTrEnabledTime() ? s2 - TimedDiagram::getInstance()->getTrEnabledTime() : s2);
        iSet->intervals.push_back(I);
    }
    return iSet;
}

IntervalSet* ModelChecker::calcAtomContISetAtTime(double time, int pIndex, double amount) {
    //TODO: by sorting region in more genuine way this function could be more effecient.

    double s1, s2;
    IntervalSet *iSet = new IntervalSet();
    Point p1, p2;

    if (time > TimedDiagram::getInstance()->getTrEnabledTime()) {
        //stochastic part : only if at the given time g-transition could have been enabled.
        /**
        * IMPORTANT note: we have computed everything for the stochastic part in the frame with origin at (gTrEnabledTime, gTrEnabledTime)
        * So we should move to that frame.
        */
        double sFrameTime = time - TimedDiagram::getInstance()->getTrEnabledTime();

        Segment timeSeg(0, sFrameTime , 0, model->MaxTime - TimedDiagram::getInstance()->getTrEnabledTime());
        for (int i = 0; (unsigned)i < TimedDiagram::getInstance()->regionList.size(); i++){
            //s1 and s2 are the validity interval for which the probability holds, returned by the function iPropHolds.
            if (TimedDiagram::getInstance()->regionList[i]->intersect(timeSeg, p1, p2)){
                s1 = p1.X; s2 = p2.X;
                double t0 = sFrameTime - TimedDiagram::getInstance()->regionList[i]->lowerBoundry->b /*- regionList[i]->timeBias*/;
                double t1 = - TimedDiagram::getInstance()->regionList[i]->lowerBoundry->a;
                if (this->propertyXleqCTest(model, TimedDiagram::getInstance()->regionList[i]->marking, t0, t1, s1, s2, pIndex, amount)){
                    Interval I(s1, s2);
                    iSet->intervals.push_back(I);
                }
            }
        }
    }

    //determinestic part after g-transition firing
    int cc;
    for (cc = 0; (unsigned)cc < TimedDiagram::getInstance()->dtrmEventList.size() && time > TimedDiagram::getInstance()->dtrmEventList[cc]->time ; cc++);
    s1 = time > TimedDiagram::getInstance()->getTrEnabledTime() ? time : 0;
    s2 = INFINITY;/*model->MaxTime;*/ // Rather infinity right?

    double t;
    if (cc == 0)
        t = time;
    else
        t = time - TimedDiagram::getInstance()->dtrmEventList[cc - 1]->time;
    if (this->propertyXleqCTest(model, TimedDiagram::getInstance()->dtrmEventList[cc]->preRegionMarking, t , 0, s1, s2, pIndex, amount)) {
        Interval I(time > TimedDiagram::getInstance()->getTrEnabledTime() ? s1 - TimedDiagram::getInstance()->getTrEnabledTime() : s1, time > TimedDiagram::getInstance()->getTrEnabledTime() ? s2 - TimedDiagram::getInstance()->getTrEnabledTime() : s2);
        iSet->intervals.push_back(I);
    }
    return iSet;
}

bool ModelChecker::propertyXleqCTest(Model* model, Marking* marking, double t0, double t1, double &s1, double &s2, int pIndex, double amount) {
    /**
     * fluid level in a place is (as+b) + (t1s+t0)d. t1s+t0 is time that has passed after entering this region.
     */
    double b = marking->fluid0[model->places[pIndex].idInMarking] + t0*marking->fluidPlaceDeriv[model->places[pIndex].idInMarking];
    double a = marking->fluid1[model->places[pIndex].idInMarking] + t1*marking->fluidPlaceDeriv[model->places[pIndex].idInMarking];
    if (IS_ZERO(a)) {
        if (b <= amount)
            return true;
        else
            return false;
    }
    double p = (amount-b)/a;
    if (p < s2 && p > s1){
        if (a < -ZERO_PREC) s1 = p;
        if (a > +ZERO_PREC) s2 = p;
        return true;
    } else if (p > s2){
        if (a * s2 + b < amount) return true;
    } else if (p < s1){
        if (a * s1 + b < amount) return true;
    }

    return false;
}

/*
 * The exponential probability density function.
 */
double ModelChecker::scdfExp(double s){
    return s != INFINITY ? (1 - exp(-this->lambda*s)) : 1;
}

/*
 * The uniform probability density function.
 */
double ModelChecker::scdfUni(double s){
    double res;
    if (s < this->a) {
        res = 0;
    } else if (s >= this->a && s < this->b) {
        res = ((s-this->a)/(this->b-this->a));
    } else if (s >= this->b || s == INFINITY){
        res = 1;
    }
    return res;
}

/* The general probability density function defined by user.
The math library supports the following functionalities:
 *
 * -> e (e), log2(e) (log2e), log10(e) (log10e), ln(2) (ln2), ln(10) (ln10), pi (pi),
 * pi / 2 (pi_2), pi / 4 (pi_4), 1 / pi (1_pi), 2 / pi (2_pi), 2 / sqrt(pi) (2_sqrtpi),
 * sqrt(2) (sqrt) and sqrt(1 / 2) (sqrt1_2).
 * -> exponential (exp), logarithmic (log), square root (sqrt), sine (sin), cosine (cos),
 * tangent (tan), cotangent (cot), secant (sec), cosecant (csc), inverse sine (asin),
 * inverse cosine (acos), inverse tangent (atan), inverse cotangent (acot),
 * inverse secant (asec), inverse cosecant (acsc), hyperbolic sine (sinh),
 * cosine (cosh), hyperbolic tangent (tanh), hyperbolic cotangent (coth),
 * hyperbolic secant (sech), hyperbolic cosecant (csch), hyperbolic inverse sine (asinh),
 * hyperbolic inverse cosine (acosh), hyperbolic inverse tangent (atanh),
 * hyperbolic inverse cotangent (acoth), hyperbolic inverse secant (asech),
 * hyperbolic inverse cosecant (acsch), absolute value (abs),
 * Heaviside step function (step) with value 1 defined for x = 0,
 * Dirac delta function with infinity (delta) and not-a-number (nandelta)
 * values defined for x = 0, and error function (erf).
 * -> unary minus ('-'),addition ('+'), subtraction ('+'), multiplication ('*'),
 * division multiplication ('/') and exponentiation ('^')
 * -> Parenthesis ('(' and ')') could be used to change priority order
 *
 * For more details visit: http://www.gnu.org/software/libmatheval/manual/libmatheval.html
 */
double ModelChecker::scdfGen(double s){
    char *names[] = { "s" };
    double values[] = { s };

    return s != INFINITY ? evaluator_evaluate (this->f, sizeof(names)/sizeof(names[0]), names, values) : 1;
}

int ModelChecker::scdfGammaFact(int n){
    if (n == 0)
        return 1;
    else
        return n*scdfGammaFact(n-1);
}

double ModelChecker::scdfGamma(double s){
    double res = 0;
    if (s != INFINITY) {
        double sum = 0;
        for (int i = 0; i < this->K; i++){
            sum += (1/scdfGammaFact(i))*pow((s/this->lambda), i)*exp(-s/this->lambda);
        }
        res = 1 - sum;
    } else {
        res = 1;
    }
    return res;
}

double ModelChecker::scdfNormal(double s){
    return s != INFINITY ? .5*(1 + erf((s - this->mu)/(sqrt(2.0*this->sigma*this->sigma)))) : 1;
}

//double scdfTruncatedNormal(double s, FunctionVars* fv){
//    return s != INFINITY ? (scdfNormal(((s-fv->mu)/fv->sigma),fv) - scdfNormal((fv->mu/fv->sigma),fv))/(1-scdfNormal((fv->mu/fv->sigma)),fv): 1;
//}

double ModelChecker::scdfFoldedNormal(double s){
    return s != INFINITY ? .5*(erf((s + this->mu)/(sqrt(2)*this->sigma)) + erf((s - this->mu)/(sqrt(2)*this->sigma))) : 1;
}

double ModelChecker::scdfDtrm(double s){
    return (s < this->dtrm) ? 0 : 1;
}

double ModelChecker::calculateDistributionValue(int distr, char* argument, double randomValue) {
    switch(distr) {
        case Exp:
            return (-1 * log(randomValue)) / atof(argument);

        case Uni:
            {
                char *argCopy = new char[strlen(argument)];
                strcpy(argCopy, argument);
                char* argFinder = strtok(argCopy, ",");
                if(argFinder == NULL) {
                    guic->addError("Invalid first argument for uniform distribution.");
                    return -1;
                }
                double a = atof(argFinder);
                argFinder = strtok(NULL, ",");
                if(argFinder == NULL) {
                    guic->addError("Invalid second argument for uniform distribution.");
                    return -1;
                }
                double b = atof(argFinder);
                delete argCopy;
                return a + (b - a) * randomValue;
            }

        case Dtrm:
            return atof(argument);

        // TODO: add Gen, Gamma, Norm and FoldedNorm distributions

        default:
            guic->addError("There is a non-supported distribution for DES in your model.");
            return -1;
    }
}

}
