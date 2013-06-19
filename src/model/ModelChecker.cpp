/*
 * ModelChecker.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: hamed
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

	if (psi1_poly != NULL) psi1_poly->print();
	if (psi2_poly != NULL) psi2_poly->print();

//	cout << "PotentialSatSet: ";
//	potentialSatSet->print();
//	psi1_poly->print();
//	std::cout<< "------------------------------" << std::endl;
//	psi2_poly->print();

	if (!debugImage.empty()){
		if (psi1_poly != NULL) geometryHelper->drawPolygon(debugImage, psi1_poly, scale, cv::Scalar(0, 0, 255));
		if (psi2_poly != NULL) geometryHelper->drawPolygon(debugImage, psi2_poly, scale, cv::Scalar(255, 255, 0));
		cv::Mat flipped;
		cv::flip(debugImage, flipped, 0);
		cv::imshow("test", flipped);
		cv::waitKey(0);
	}


	//potentialSatSet->print();

	// First interval set.
	if (psi2_poly != NULL){
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

            satSet = satSet->unionWith(geometryHelper->getIntersectionIntervals(psi2_poly, seg));
		}
	}
//	satSet->print();

	if (psi1_poly != NULL){
		IntervalSet* psi1_Int = new IntervalSet();
		//Second interval set.
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

			psi1_Int = psi1_Int->unionWith(geometryHelper->getIntersectionIntervals(psi1_poly, seg));
			cout << "psi1_Int: ";
//			psi1_Int->print();
			//In general case here the psi1_poly should be reformed.
		}

		if (psi2_poly != NULL){
			IntervalSet* psi12_Int = geometryHelper->getIntersectionIntervals(psi1_poly, psi2_poly);
			if (psi12_Int != NULL){
//				psi12_Int->print();
                satSet = satSet->unionWith(psi1_Int->intersect(psi12_Int));
//				satSet->print();
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
    char *argument = this->model->transitions[gTransitionId(this->model)].df_argument;

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
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
            return false;
        }
        a = atof(argFinder);
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addError(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(this->model)+1).arg(this->model->transitions[gTransitionId(this->model)].id).toStdString());
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
//	satSet->print();

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

//	satSet->print();

	IntervalSet* iSet = new IntervalSet();
    iSet->intervals.push_back(Interval(dtrmRegion->time, (state1 == ENTIRE)? dtrmRegion->nextDtrmEvent->time : t1));
	iSet = iSet->minus(satSet);

//	iSet->print();


    for (unsigned int i =0; i < dtrmRegion->nextRegions->size(); i++)
        satSet = satSet->unionWith(visitStocRegion(dtrmRegion->nextRegions->at(i), psi1, psi2, iSet, t, bound));

	return satSet;

}

bool ModelChecker::until(IntervalSet *&satSet, Formula* psi1, Formula* psi2, Interval bound) {
	// TODO: Currently I have assumed that the given bound starts at 0 ie. T_1 = 0.
    // TODO: Traverse through the formulas retrieving the polygons. Then in the end retrieve the intervalset.
//	if (satSet != NULL)
//		satSet->clear();
//	else
	satSet = new IntervalSet();

    double t = ttc; // - std->getTrEnabledTime();
	std::cout << "t" << t << std::endl;
	Line timeLine(0, t);
	Point p1, p2;

	if (!debugImage.empty())
		geometryHelper->drawVerticalLine(debugImage, bound.end, UP, scale, cv::Scalar(0, 0, 0));


//	satSet->print();
	//iterating over all deterministic events.
	// the last event regards the maximum time reached. so should not be considered.
	for (unsigned int i = 0; i < std->dtrmEventList.size() - 1; i++){
		std::cout << i << std::endl;
        if (std->dtrmEventList[i]->time <= ttc && std->dtrmEventList[i]->nextDtrmEvent->time > ttc){
			satSet = satSet->unionWith(visitDtrmRegion(std->dtrmEventList[i], psi1, psi2,t, bound));
//			satSet->print();
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
    std::cout << "IntervalSet result TT : ";
    res->print(std::cout);
    std::cout << std::endl;
    return true;
}

bool ModelChecker::iSetNeg(IntervalSet *&res, IntervalSet* iset1) {
    std::cout << "IntervalSet 1 : ";
    iset1->print(std::cout);
    std::cout << std::endl;
    if(!iSetTT(res)) return false;
    res->minus(iset1);
    std::cout << "IntervalSet result NEG : ";
    res->print(std::cout);
    std::cout << std::endl;
    return true;
}

bool ModelChecker::iSetAnd(IntervalSet *&res, IntervalSet* iset1, IntervalSet* iset2) {
    std::cout << "IntervalSet 1 : ";
    iset1->print(std::cout);
    std::cout << std::endl;
    std::cout << "IntervalSet 2 : ";
    iset2->print(std::cout);
    std::cout << std::endl;
    res = iset1->intersect(iset2);
    std::cout << "IntervalSet result AND : ";
    res->print(std::cout);
    std::cout << std::endl;
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
//        std::cout << iSet->intervals[i].end << " : " << scdfExp(iSet->intervals[i].end - shift) << std::endl;
//        std::cout << iSet->intervals[i].start << " : " << scdfExp(iSet->intervals[i].start - shift) << std::endl;
//        std::cout << " Total : " << scdfExp(iSet->intervals[i].end - shift) - scdfExp(iSet->intervals[i].start - shift) << std::endl;
//        std::cout << " Distribution : " << this->distr << std::endl;
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
//    std::cout << prob << std::endl;
    return prob;
}

bool ModelChecker::parseFML(Formula *&fullFML, QString rawFormula) {
    /*
     * Parse the STL formula
     * Use Flex + Bison
     * Flex (Lexer) : Scans the lines and transforms it into tokens.
     * Bison (Parser) : Creates the datastructure of the AST and gives initial values to the different type of formulas.
     */
    yy_scan_string(rawFormula.toAscii().data());
    yyparse();
    yylex_destroy();
    reset_lexer();
    if (getColError() == -1) {
        guic->addText("The formula is succesfully parsed.");
    } else {
        guic->addError(QString("Syntax error for character %1 in formula %2: The formula could not be parsed.").arg(getColError()).arg(rawFormula).toStdString());
        return false;
    }
    //fullFML = parser_results();
    double prob = 0.2;
    Interval bound(0,10);
    fullFML = new ProbFormula(new UntilFormula(new AtomContFormula(0,0,"reservoir",5.00),new AtomDisFormula(0,0,"pumpon",0),bound),0,prob,LEQ);
//    /*
//     * Some intermediate datastructures that should work as test cases.
//     */
//    double n1 = 1, c2 = 0.1, c3 = 0.2;
//    double t1 = 0, t2 = 10;
//    double ttc = 2.00, prob = 0.2;
//    Interval bound(0,10);

//    if (QString::compare(rawFormula, QString("Pr<=0.2 (P(1) == 1)")) == STR_EQUAL) {
//        // P(1)=1
//        fullFML = new ProbFormula(new AtomDisFormula(0,0,"Input1On",n1),0,prob,LEQ);
//        guic->addText("Checking demo formula 1 : Pr&lt;=0.2 (P(1) == 1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(6) <= 0.1)")) == STR_EQUAL) {
//        // P(8)<=0.1
//        fullFML = new ProbFormula(new AtomContFormula(0,0,"soft1",c2),0,prob,LEQ);
//        guic->addText("Checking demo formula 2 : Pr&lt;=0.2 (P(6) &lt;= 0.1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(6) <= 0.2 AND P(6) <= 0.1)")) == STR_EQUAL) {
//        // P(1)=1 ^ P(8)<=0.1
//        fullFML = new ProbFormula (new AndFormula(new AtomContFormula(0,0,"soft1", c3),new AtomContFormula(0,0,"soft1",c2)),0,prob,LEQ);
//        guic->addText("Checking demo formula 3 : Pr&lt;=0.2 (P(6) &lt;= 0.2 AND P(6) &lt;= 0.1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 ~(P(6) == 0.1)")) == STR_EQUAL) {
//        // ~P(8)<=0.1
//        fullFML = new ProbFormula (new NegFormula(new AtomContFormula(0,0,"soft1",c2),0),0,prob,LEQ);
//        guic->addText("Checking demo formula 4 : Pr&lt;=0.2 ~(P(6) == 0.1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 ~(P(1) == 1)")) == STR_EQUAL) {
//        // ~P(1)=1
//        fullFML = new ProbFormula (new NegFormula(new AtomDisFormula(0,0,"Input1On", n1),0),0,prob,LEQ);
//        guic->addText("Checking demo formula 5 : Pr&lt;=0.2 ~(P(1) == 1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (tt)")) == STR_EQUAL) {
//        // tt
//        fullFML = new ProbFormula(new TrueFormula(0,0),0,prob,LEQ);
//        guic->addText("Checking demo formula 6 : Pr&lt;=0.2 (tt)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(1)=1 U[0,10] P(8)=0.1)")) == STR_EQUAL) {
//        // P(1)=1 U[0,10] P(8)=0.1
//        fullFML = new ProbFormula(new UntilFormula(new AtomDisFormula(0,0,"Input1On", n1),new AtomContFormula(0,0,"stor2",c3),bound),0,prob,LEQ);
//        guic->addText("Checking demo formula 7 : Pr&lt;=0.2 (P(1)=1 U[0,10] P(8)=0.1)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 ((P(1)=1 AND P(8)<=0.1) U[0,10] P(9)=0.2)")) == STR_EQUAL) {
//        // (P(1)=1 ^ P(8)<=0.1) U[0,10] P(9)=0.2
//        fullFML = new ProbFormula(new UntilFormula(new AndFormula(new AtomDisFormula(0,0,"Input1On", n1),new AtomContFormula(0,0,"soft1",c2)),new AtomContFormula(0,0,"stor2",c3),bound),0,prob,LEQ);
//        guic->addText("Checking demo formula 8 : Pr&lt;=0.2 ((P(1)=1 AND P(8)&lt;=0.1) U[0,10] P(9)=0.2)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(2) <= 5)")) == STR_EQUAL) {
//        fullFML = new ProbFormula(new AtomContFormula(0,0,"reservoir",5.00),0,prob,LEQ);
//        guic->addText("Checking demo formula 1 : Pr&lt;=0.2 (P(2) &lt;= 5)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(0) == 0)")) == STR_EQUAL) {
//        fullFML = new ProbFormula(new AtomDisFormula(0,0,"pumpOn",0),0,prob,LEQ);
//        guic->addText("Checking demo formula 2 : Pr&lt;=0.2 (P(0) == 0)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (~P(0) == 0)")) == STR_EQUAL) {
//        fullFML = new ProbFormula(new NegFormula(new AtomDisFormula(0,0,"pumpOn",0),0),0,prob,LEQ);
//        guic->addText("Checking demo formula 3 : Pr&lt;=0.2 (~ P(0) == 0)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(0) == 0 AND P(2) <= 5)")) == STR_EQUAL) {
//        fullFML = new ProbFormula(new AndFormula(new AtomDisFormula(0,0,"pumpOn",0),new AtomContFormula(0,0,"reservoir",5.00)),0,prob,LEQ);
//        guic->addText("Checking demo formula 4 : Pr&lt;=0.2 (P(0) == 0 AND P(2) &lt;= 5)");
//    } else if (QString::compare(rawFormula, QString("Pr<=0.2 (P(0) == 0 U[0,10] P(2) <= 5)")) == STR_EQUAL) {
//        fullFML = new ProbFormula(new UntilFormula(new AtomContFormula(0,0,"reservoir",5.00),new AtomDisFormula(0,0,"pumpOn",0),bound),0,prob,LEQ);
//        guic->addText("Checking demo formula 5 : Pr&lt;=0.2 (P(0) == 0 U[0,10] P(2) &lt;= 5)");
//    } else {
        //guic->addText("Checking the input formula.");
//    }
return true;
}

bool ModelChecker::traverseFML(bool &res, Formula *fullFML) {
    /*
     * TODO: Traverse through the STL formula
     * Strategies is post-order
     */
    IntervalSet *iSetLeft;
    IntervalSet *iSetRight;
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
    return true;
}

bool ModelChecker::traverseISetFML(IntervalSet *&res, Formula *fullFML) {
    /*
     * TODO: Traverse through the STL formula
     * Strategies is post-order
     */
    res = 0;
    IntervalSet *iSetLeft;
    IntervalSet *iSetRight;
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
            IntervalSet *iSetLeft;
            if(!traverseISetFML(iSetLeft,fullFML->getLeftChild())) return false;
            IntervalSet *iSetRight;
            if(!traverseISetFML(iSetRight,fullFML->getRightChild())) return false;
            if(!iSetAnd(res,iSetLeft,iSetRight)) return false;
        }
    break;
    case NEG:
        if (fullFML->getLeftChild() != 0) {
            IntervalSet *iSetLeft;
            if(!traverseISetFML(iSetLeft,fullFML->getLeftChild())) return false;
            if (!iSetNeg(res,iSetLeft)) return false;
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
    return res;
}

IntervalSet* ModelChecker::calcAtomDisISetAtTime(double time, int pIndex, double amount) {

    double s1, s2;
    IntervalSet *iSet = new IntervalSet();
    Point p1, p2;

//    std::cout << "RegionList size : " << TimedDiagram::getInstance()->regionList.size() << std::endl;
//    std::cout << "DtrmEventList size : " << TimedDiagram::getInstance()->dtrmEventList.size() << std::endl;

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
        //std::cout << "Interval : [" << I.end << "," << I.start << "]" << std::endl;
        iSet->intervals.push_back(I);
//        std::streambuf * buf;
//        buf = std::cout.rdbuf();
//        std::ostream out(buf);
//        iSet->print(out);
    }
    return iSet;
}

IntervalSet* ModelChecker::calcAtomContISetAtTime(double time, int pIndex, double amount) {
    //TODO: by sorting region in more genuine way this function could be more effecient.

    double s1, s2;
    //double prob = 0;
    IntervalSet *iSet = new IntervalSet();
    Point p1, p2;

//    std::cout << "RegionList size : " << TimedDiagram::getInstance()->regionList.size() << std::endl;
//    std::cout << "DtrmEventList size : " << TimedDiagram::getInstance()->dtrmEventList.size() << std::endl;

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
                    //prob += fabs(sPdfInt(s1, fv) - sPdfInt(s2, fv));
//                    std::cout << "Interval : ["<< s1 << "," << s2 << "]" << std::endl;
//                    iSet->unionWith(Interval(s1,s2));
                    Interval I(s1, s2);
//                    std::cout << "Interval : [" << I.end << "," << I.start << "]" << std::endl;
                    iSet->intervals.push_back(I);
//                    iSet->print(std::cout);
                }
            }
        }
    }

    //determinestic part after g-transition firing
    int cc;
    for (cc = 0; (unsigned)cc < TimedDiagram::getInstance()->dtrmEventList.size() && time > TimedDiagram::getInstance()->dtrmEventList[cc]->time ; cc++);
    s1 = time > TimedDiagram::getInstance()->getTrEnabledTime() ? time : 0;
    s2 = INFINITY;/*model->MaxTime;*/ // Rather infinity right?
//    std::cout << "s1: " << s1 << " s2: " << s2 << " ";
//    std::cout << prob + fabs(sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1,fv) - sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2,fv)) << " = " << prob << " + " << sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1,fv) << "-" << sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2,fv) << " = " << fabs(sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1,fv) - sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2,fv)) << std::endl;

    double t;
    if (cc == 0)
        t = time;
    else
        t = time - TimedDiagram::getInstance()->dtrmEventList[cc - 1]->time;
    if (this->propertyXleqCTest(model, TimedDiagram::getInstance()->dtrmEventList[cc]->preRegionMarking, t , 0, s1, s2, pIndex, amount)) {
        Interval I(time > TimedDiagram::getInstance()->getTrEnabledTime() ? s1 - TimedDiagram::getInstance()->getTrEnabledTime() : s1, time > TimedDiagram::getInstance()->getTrEnabledTime() ? s2 - TimedDiagram::getInstance()->getTrEnabledTime() : s2);
//        std::cout << "Interval : [" << I.end << "," << I.start << "]" << std::endl;
        iSet->intervals.push_back(I);
        //prob += fabs(sPdfInt(time > gTrEnabledTime ? s1 - gTrEnabledTime : s1, fv) - sPdfInt(time > gTrEnabledTime ? s2 - gTrEnabledTime : s2, fv));
//        std::streambuf * buf;
//        buf = std::cout.rdbuf();
//        std::ostream out(buf);
//        iSet->print(out);
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
//double ModelChecker::scdfUni(double s){
//    return s != INFINITY ? ((s-this->a)/(this->b-this->a)) : 1;
//}

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
//    return s != INFINITY ? ((s-fv->a)/(fv->b-fv->a)) : 1;
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
    //-.5*(1 - erf((s - fv->mu)/(fv->sigma*1.414213562)))
    return s != INFINITY ? .5*(1 + erf((s - this->mu)/(sqrt(2.0*this->sigma*this->sigma)))) : 1;
}

//double scdfTruncatedNormal(double s, FunctionVars* fv){
//    //-.5*(1 - erf((s - fv->mu)/(fv->sigma*1.414213562)))
//    return s != INFINITY ? (scdfNormal(((s-fv->mu)/fv->sigma),fv) - scdfNormal((fv->mu/fv->sigma),fv))/(1-scdfNormal((fv->mu/fv->sigma)),fv): 1;
//}

double ModelChecker::scdfFoldedNormal(double s){
    //-.5*(1 - erf((s - fv->mu)/(fv->sigma*1.414213562)))
    return s != INFINITY ? .5*(erf((s + this->mu)/(sqrt(2)*this->sigma)) + erf((s - this->mu)/(sqrt(2)*this->sigma))) : 1;
}

double ModelChecker::scdfDtrm(double s){
    //-.5*(1 - erf((s - fv->mu)/(fv->sigma*1.414213562)))
    return (s < this->dtrm) ? 0 : 1;
}

}
