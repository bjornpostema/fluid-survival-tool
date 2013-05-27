#include "Calculator.h"
#include "DFPN2.h"
#include "TimedDiagram.h"
#include "../includes/opencv/cv.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/time.h>

extern "C" {
#include "../includes/matheval.h"
}

#include "../controller/GUIController.h"
#include "Formula.h"
#include "ModelChecker.h"

namespace model {

Calculator::Calculator(QString rawFileName, QString rawPlaceName, GUIController *newGuic)
{
    guic = newGuic;
    model = 0;
    placeName = QString();
    fileName  = QString();

    if (setFile(rawFileName)) {
        setPlace(rawPlaceName);
    }
}

Calculator::~Calculator()
{
    delete model;
}


bool Calculator::setPlace(QString rawPlaceName) {
    if (!rawPlaceName.isEmpty()) {
        placeName = rawPlaceName;
        guic->addText(QString("Place name is set to: ").append(placeName).toStdString());
        return true;
    } else {
        guic->addText("Place name is not set.");
        return false;
    }
}

bool Calculator::setFile(QString rawFileName) {
    if (!rawFileName.isEmpty()) {
        fileName = rawFileName;
        guic->addText(QString("File name set to: ").append(fileName).toStdString());
    } else {
        guic->addText("File name is not set.");
        return false;
    }

    model = ReadModel(QString2Char(fileName), guic);
    if (model == NULL) {
        guic->addText(QString("Model could not be read or parsed.").toStdString());
        return false;
    } else {
        guic->addText(QString("Model is read and parsed.").toStdString());
        return true;
    }
}

const char* Calculator::QString2Char(QString rawQString) {
        QByteArray qba  = rawQString.toAscii();
        const char *resPlaceName;
        resPlaceName = qba.data();
        return resPlaceName;
}

/**
 * @brief Calculator::showSTD Shows the STD of the currently loaded model.
 * @param rawFileName To set the save file name location to [rawFileName]_rd.jpg
 * @return Gives true when the STD can be shown else false.
 */
bool Calculator::showSTD(QString rawFileName) {
    if (model == 0 || fileName == 0) {
        return false;
    }

//    unsigned int pIndex, disPIndex;
//    for (int i = 0; i < model->N_places; i++) {
//        //if (model->places[i].type != PT_FLUID) continue;
//        if (model->places[i].type == PT_FLUID && strncmp(model->places[i].id, placeName, strlen(placeName)) == 0)  pIndex = i;;
//        if (model->places[i].type == PT_DISCRETE && strncmp(model->places[i].id, "Input1On", strlen("Input1On")) == 0)  disPIndex = i;;
//    }

    unsigned int tIndex = 0;
    for (int i = 0; i < model->N_transitions; i++) {
            if (model->places[i].type != TT_DETERMINISTIC) continue;
            if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

            tIndex = i;
    }


    double time = model->transitions[tIndex].time;
    std::cout << "time: " <<  time << std::endl;
//    std::cout << "pIndex: " <<  pIndex << std::endl;
//    std::cout << "disPIndex: " <<  disPIndex << std::endl;

    FunctionVars *fv = new FunctionVars;
    //fv = (FunctionVars *) malloc(sizeof(FunctionVars));
    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

    fv->mu = time + 2;

    model->MaxTime = time + 50.0;
    Marking* initialMarking = createInitialMarking(model);
    long mtime, seconds, useconds;

    TimedDiagram::getInstance()->setModel(model);

    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);

    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    std::cout << "Number of regions: " << TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    std::cout << "Time to generate STD: " << mtime << "ms" << std::endl;

    guic->addText(QString("Time to generate STD: %1 ms").arg(mtime).toStdString());

    TimedDiagram::getInstance()->scale = 20;
    std::cout << "Writing the debug region diagram...." << std::endl;
    std::stringstream ss;
    ss << rawFileName.toStdString() << "_rd";
    TimedDiagram::getInstance()->saveDiagram(ss.str());
    cv::Mat flipped;
    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    cv::imshow("STD Diagram Plot", flipped);
    //cv::waitKey(0);
    delete fv;
    return true;
}

bool Calculator::showProbFunc() {
    if (model == 0 || placeName == 0) {
        return false;
    }

    FunctionVars *fv = new FunctionVars;

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    model->MaxTime = 100.0;
    Marking* initialMarking = createInitialMarking(model);
    long mtime, seconds, useconds;

    TimedDiagram::getInstance()->setModel(model);
    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);
    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    std::cout << "Time to generate STD: " << mtime<< "ms" << std::endl;

//	if (argc == 4){
//		std::cout << "Writing the debug region diagram...." << std::endl;
//		std::stringstream ss;
//		ss << argv[1] << "_rd";
//		TimedDiagram::getInstance()->saveDiagram(ss.str());
//	}

    std::cout << "starting measure computation..." << std::endl;
    fv->pIndex = -1;
    //Model* model = TimedDiagram::getInstance()->model;
    for (int i = 0; fv->pIndex == -1 && i < model->N_places; i++) {
        if (model->places[i].type != PT_FLUID) continue;
        if (strncmp(model->places[i].id,QString2Char(placeName), strlen(model->places[i].id)) != 0) continue;
        fv->pIndex = i;
    }

    if (fv->pIndex == -1) {
        guic->addText("The place name is set to an invalid place name. This functionality only works for fluid place names.");
        return false;
    }

    std::ofstream oFile;
    oFile.open(QString2Char(placeName), std::ios::out);

    mtime = 0; seconds = 0; useconds = 0;
    timeval t0, t1;
    char *argFinder;
    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Exp:
    case Gamma: fv->lambda = atoi(model->transitions[gTransitionId(model)].df_argument);
    break;
    case Uni:
        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
        if (argFinder == NULL) {
            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
            return false;
        }
        fv->a = atoi(argFinder);
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
            return false;
        }
        fv->b = atoi(argFinder);
    break;
    case Gen:
        fv->f = evaluator_create (model->transitions[gTransitionId(model)].df_argument);
    break;
    case Norm:
        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
        if (argFinder == NULL) {
            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
            return false;
        }
        fv->mu = atoi(argFinder);
        argFinder = strtok(NULL,",");
        if (argFinder == NULL) {
            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
            return false;
        }
        fv->sigma = atoi(argFinder);
    break;
    }
    for (double t = .02; t <= model->MaxTime + .01; t += .5){
        for (fv->amount = .05; fv->amount <= model->places[fv->pIndex].f_bound; fv->amount += .2){
            //std::cout << "t = " << t << "--amount = " << amount << std::endl;
            double p = -1.0;
            gettimeofday(&t0, NULL);
            //double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
            switch (model->transitions[gTransitionId(model)].df_distr)
            {
            case Exp: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfExp, propertyTest, fv);
            break;
            case Uni: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfUni, propertyTest, fv);
            break;
            case Gen: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfGen, propertyTest, fv);
            break;
            case Gamma: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfGamma, propertyTest, fv);
            break;
            case Norm: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfNormal, propertyTest, fv);
            }
            gettimeofday(&t1, NULL);

            seconds  += t1.tv_sec  - t0.tv_sec;
            useconds += t1.tv_usec - t0.tv_usec;

            oFile << " "<< p;
            //std::cout << " "<< p;
        }
        oFile << std::endl;
        //std::cout << std::endl;
    }

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(fv->f);
    break;
    }
    mtime = ((seconds * 1000 + useconds/1000.0) + 0.5);
    std::cout << "total time computing measures:  " << mtime << "ms" << std::endl;

    oFile.close();
    FILE* gnuplotPipe;
    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
    {
        fprintf(gnuplotPipe,"set pm3d \n unset surface\n "
                "set xlabel \"Storage Content [m^3]\" \n"
                "set zlabel \"Probability\"\n"
                "set ylabel \"time [hours]\"\n"
                "set xtics (\"0\" 0, \"2\" 10, \"4\" 20,\"6\" 30, \"8\" 40) \n "
                "set ytics (\"0\" 0, \"20\" 40, \"40\" 80,\"60\" 120, \"80\" 160, \"100\" 200) \n "
                "set palette rgbformulae 33,13,10 \n");

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"splot \"%s\" matrix with lines\n",QString2Char(placeName));
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"exit \n");
        fclose(gnuplotPipe);
    }
    delete fv;
    delete initialMarking;
}

bool Calculator::modelCheck(QString rawFormula, QString rawCheckTime) {
    /**
     * Parse the STL formula
     */

    FunctionVars *fv = new FunctionVars;

    int disPIndex = 0;
    double checkTime = rawCheckTime.toDouble();
    if (!(checkTime >= 0)) {
        guic->addText("The chosen time to check is invalid, please specify a new time.");
    }

    TimedDiagram::getInstance()->setModel(model);

    ModelChecker modelChecker(model, TimedDiagram::getInstance());

    std::cout << "starting measure computation..." << std::endl;
    /**
     * Find all the indeces of all the places in the formula and keep track of all these places.
     */
    fv->pIndex = -1;
    //Model* model = TimedDiagram::getInstance()->model;
    for (int i = 0; fv->pIndex == -1 && i < model->N_places; i++) {
        if (model->places[i].type != PT_FLUID) continue;
        if (strncmp(model->places[i].id,QString2Char(placeName), strlen(QString2Char(placeName))) != 0) continue;
        fv->pIndex = i;
    }

    if (fv->pIndex == -1) {
        guic->addText("The place name is set to an invalid place name. This functionality only works for fluid place names.");
        return false;
    }

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

    model->MaxTime = checkTime + 100.0;

    /**
     * ASSUME: STL formula is x<=0.1 U[0,t] n=1
     */
    AtomContFormula* psi1 = new AtomContFormula(fv->pIndex, 0.1);
    AtomDisFormula* psi2_dis = new AtomDisFormula(disPIndex, 1);
    AtomContFormula* psi2_cont = new AtomContFormula(fv->pIndex, 3);
    ADFormula* psi2 = new ADFormula(psi2_cont, psi2_dis);


    modelChecker.debugImage = TimedDiagram::getInstance()->debugImage;
    modelChecker.scale = TimedDiagram::getInstance()->scale;

    std::ofstream oFile("./results.out");
    std::ofstream oInt("./intervals.out");
    for (double T = 1; T < 11; T += 1){
        Interval bound(0, checkTime+T);
        IntervalSet* i1 = modelChecker.until(psi1, psi2, bound, checkTime);

        std::cout << "The satisfaction set of Until formula: " << std::endl;
        for (unsigned int i = 0; i < i1->intervals.size(); i++){
            i1->intervals[i].end = i1->intervals[i].end - checkTime;
            i1->intervals[i].start = i1->intervals[i].start - checkTime;
        }

        i1->print(oInt);

        oFile << modelChecker.calcProb(i1, spdfNormal, 0, fv) << std::endl;
    }

}

}
