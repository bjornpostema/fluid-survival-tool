#include "Facade.h"
#include "DFPN2.h"
#include "TimedDiagram.h"
#include "../includes/opencv/cv.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/time.h>

extern "C" {
#include "../includes/matheval.h"
}

#include "Formula.h"
#include "ModelChecker.h"

namespace model {

Facade::Facade(QString rawFileName, QString rawPlaceName, Logger *newGuic)
{
    guic = newGuic;
    model = 0;
    placeName = QString();
    fileName  = QString();

    if (setFile(rawFileName)) {
        setPlace(rawPlaceName);
    }
}

Facade::Facade(QString rawFileName, Logger *newGuic)
{
    guic = newGuic;
    model = 0;
    placeName = QString();
    fileName  = QString();

    setFile(rawFileName);
}

Facade::~Facade()
{
    delete model;
}


bool Facade::setPlace(QString rawPlaceName) {
    if (!rawPlaceName.isEmpty()) {
        placeName = rawPlaceName;
        guic->addText(QString("Place name is set to: ").append(placeName).toStdString());
        return true;
    } else {
        guic->addText("Place name was already set.");
        return false;
    }
}

bool Facade::setFile(QString rawFileName) {
    if (!rawFileName.isEmpty()) {
        fileName = rawFileName;
        guic->addText(QString("File name set to: ").append(fileName).toStdString());
    } else {
        guic->addText("File name was already set.");
        return false;
    }

    model = ReadModel(QString2Char(fileName), guic);
    if (model == NULL) {
        guic->addError(QString("Model could not be read or parsed.").toStdString());
        return false;
    } else {
        guic->addText(QString("Model is read and parsed.").toStdString());
        return true;
    }
}

const char* Facade::QString2Char(QString rawQString) {
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
bool Facade::showSTD(QString rawFileName, double maxTime, int imageScale) {
    if (model == 0 || fileName == 0) {
        return false;
    }

    unsigned int tIndex = 0;
    for (int i = 0; i < model->N_transitions; i++) {
        if (model->places[i].type != PT_DISCRETE) continue;
        if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

        tIndex = i;
    }

    double time = model->transitions[tIndex].time;
    //    unsigned int pIndex, disPIndex;
    //    for (int i = 0; i < model->N_places; i++) {
    //        //if (model->places[i].type != PT_FLUID) continue;
    //        if (model->places[i].type == PT_FLUID && strncmp(model->places[i].id, placeName, strlen(placeName)) == 0)  pIndex = i;;
    //        if (model->places[i].type == PT_DISCRETE && strncmp(model->places[i].id, "Input1On", strlen("Input1On")) == 0)  disPIndex = i;;
    //    }

    //    unsigned int tIndex = 0;
    //    for (int i = 0; i < model->N_transitions; i++) {
    //            if (model->places[i].type != TT_DETERMINISTIC) continue;
    //            if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

    //            tIndex = i;
    //    }

    //    double time = model->transitions[tIndex].time;
    //    std::cout << "time: " <<  time << std::endl;
    //    std::cout << "pIndex: " <<  pIndex << std::endl;
    //    std::cout << "disPIndex: " <<  disPIndex << std::endl;

//    FunctionVars *fv = new FunctionVars;
    //fv = (FunctionVars *) malloc(sizeof(FunctionVars));
    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

    model->MaxTime = maxTime;
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

    TimedDiagram::getInstance()->scale = imageScale;
    std::cout << "Writing the debug region diagram...." << std::endl;
    std::stringstream ss;
    ss << "./output/" << rawFileName.toStdString() << "_std";
    TimedDiagram::getInstance()->saveDiagram(ss.str());
    cv::Mat flipped;
    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    cv::imshow("STD Diagram Plot", flipped);
    //cv::waitKey(0);
//    delete fv;
    freeMarking(initialMarking);
    return true;
}

bool Facade::showProbFunc(double cStart, double cEnd, double cStep, double tStep, double maxTime) {
    if (model == 0 || placeName == 0) {
        return false;
    }

    unsigned int pIndex = -1;
    double amount = 0;

//    unsigned int tIndex = 0;
//    for (int i = 0; i < model->N_transitions; i++) {
//        if (model->places[i].type != TT_DETERMINISTIC) continue;
//        if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

//        tIndex = i;
//    }

//    double time = model->transitions[tIndex].time;

//    FunctionVars *fv = new FunctionVars;

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    model->MaxTime = maxTime;
    //model->MaxTime = 50 + time;
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
    //  TimedDiagram::getInstance()->scale = 20;
    //    std::cout << "Writing the debug region diagram...." << std::endl;
    //    std::stringstream ss;
    //    ss << "temp" << "_rd";
    //    TimedDiagram::getInstance()->saveDiagram(ss.str());
    //    cv::Mat flipped;
    //    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    //    cv::imshow("STD Diagram Plot", flipped);

    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic);

    if (!modelChecker->setVariables()) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    std::cout << "starting measure computation..." << std::endl;
    //Model* model = TimedDiagram::getInstance()->model;
    for (int i = 0; pIndex == (unsigned)-1 && i < model->N_places; i++) {
        if (model->places[i].type != PT_FLUID) continue;
        if (strncmp(model->places[i].id,QString2Char(placeName), strlen(model->places[i].id)) != 0) continue;
        pIndex = i;
    }

    if (pIndex == (unsigned)-1) {
        guic->addError("The place name is set to an invalid place name. This functionality only works for fluid place names.");
//        delete fv;
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    std::ofstream oFile;
    oFile.open(QString2Char(QString("./output/%1_3d.dat").arg(placeName)), std::ios::out);

    mtime = 0; seconds = 0; useconds = 0;
    timeval t0, t1;
//    char *argFinder;
//    switch (model->transitions[gTransitionId(model)].df_distr)
//    {
//    case Exp:
//    case Gamma: fv->lambda = atof(model->transitions[gTransitionId(model)].df_argument);
//        break;
//    case Uni:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->a = atof(argFinder);
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->b = atof(argFinder);
//        break;
//    case Gen:
//        fv->f = evaluator_create (model->transitions[gTransitionId(model)].df_argument);
//        break;
//    case FoldedNorm:
//    case Norm:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->mu = atof(argFinder);
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->sigma = atof(argFinder);
//        break;
//    case Dtrm:
//        fv->dtrm = atof(model->transitions[gTransitionId(model)].df_argument);
//        break;
//    }
//    bool warn = false; // t += .5 en fv->amount += .2
    for (double t = .02; t <= model->MaxTime + .01; t += tStep){
        for (amount = cStart+.05; amount <= cEnd; amount += cStep){
//            if (fv->amount <= model->places[fv->pIndex].f_bound) {
            //std::cout << "t = " << t << "--amount = " << amount << std::endl;
            gettimeofday(&t0, NULL);

            double p = modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(t, pIndex, amount),0.00);
//            std::cout << "Tijd : " << t << " Amount : " << amount << " Prob : " << p << std::endl;
            //double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
//            switch (model->transitions[gTransitionId(model)].df_distr)
//            {
//            case Exp: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfExp, propertyTest, fv);
//                break;
//            case Uni: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfUni, propertyTest, fv);
//                break;
//            case Gen: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfGen, propertyTest, fv);
//                break;
//            case Gamma: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfGamma, propertyTest, fv);
//                break;
//            case Norm: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfNormal, propertyTest, fv);
//                break;
//            case FoldedNorm: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfFoldedNormal, propertyTest, fv);
//                break;
//            case Dtrm: p = TimedDiagram::getInstance()->calProbAtTime(t, scdfDtrm, propertyTest, fv);
//                break;
//            }
            gettimeofday(&t1, NULL);

            seconds  += t1.tv_sec  - t0.tv_sec;
            useconds += t1.tv_usec - t0.tv_usec;
            //if (p > 1) p = 1;
            oFile << " "<< p;
            //std::cout << " "<< p;
//            } else {
//                oFile << " " << 0;
//                warn = true;
//            }
        }
        oFile << std::endl;
        //std::cout << std::endl;
    }

//    if (warn) guic->addText("Warning: The constant is greater than the fluid bound of that place.");

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }
    mtime = ((seconds * 1000 + useconds/1000.0) + 0.5);
    std::cout << "total time computing measures:  " << mtime << "ms" << std::endl;

    oFile.close();
    FILE* gnuplotPipe;
    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
    {
//        std::cout<<cStart<<(cStart+(cEnd-cStart)*0.25)<<(cStart+(cEnd-cStart)*0.50)<<(cStart+(cEnd-cStart)*0.75)<<cEnd<<(model->MaxTime*0.2)<<(model->MaxTime*0.4)<<(model->MaxTime*0.6)<<(model->MaxTime*0.8)<<model->MaxTime <<std::endl;
//        std::cout<<(int)cStart<<std::endl<<(int)(cStart+(cEnd-cStart)*0.25)<<std::endl<<(int)((cEnd-cStart)/cStep)*0.25<<std::endl<<(int)(cStart+(cEnd-cStart)*0.50)<<std::endl<<(int)((cEnd-cStart)/cStep)*0.50<<std::endl<<(int)(cStart+(cEnd-cStart)*0.75)<<std::endl<<(int)((cEnd-cStart)/cStep)*0.75<<std::endl<<(int)cEnd<<std::endl<<(int)((cEnd-cStart)/cStep)<<std::endl<<std::endl<<(int)(model->MaxTime*0.2)<<std::endl<<(int)(model->MaxTime*0.2/tStep)<<std::endl<<(int)(model->MaxTime*0.4)<<std::endl<<(int)(model->MaxTime*0.4/tStep)<<std::endl<<(int)(model->MaxTime*0.6)<<std::endl<<(int)(model->MaxTime*0.6/tStep)<<std::endl<<(int)(model->MaxTime*0.8)<<std::endl<<(int)(model->MaxTime*0.8/tStep)<<std::endl<<(int)(model->MaxTime)<<std::endl<<(int)(model->MaxTime/tStep)<<std::endl;
//        printf("test");
//        char buffer[400];
//        sprintf(buffer, "set pm3d \n unset surface\n "
//                        "set xlabel \"Constant\" \n"
//                        "set zlabel \"Probability\"\n"
//                        "set ylabel \"Time\"\n"
////                        "set xtics (\"0\" 0, \"2\" 10, \"4\" 20,\"6\" 30, \"8\" 40) \n "
////                        "set ytics (\"0\" 0, \"20\" 40, \"40\" 80,\"60\" 120, \"80\" 160, \"100\" 200) \n "
//                        "set xtics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d) \n "
//                        "set ytics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d, \"%d\" %d) \n "
//                        "set palette rgbformulae 33,13,10 \n",(int)cStart,(int)0,(int)(cStart+(cEnd-cStart)*0.25),(int)(((cEnd-cStart)/cStep)*0.25),(int)(cStart+(cEnd-cStart)*0.50),(int)(((cEnd-cStart)/cStep)*0.50),(int)(cStart+(cEnd-cStart)*0.75),(int)(((cEnd-cStart)/cStep)*0.75),(int)cEnd,(int)((cEnd-cStart)/cStep),(int)0,(int)0,(int)(model->MaxTime*0.2),(int)(model->MaxTime*0.2/tStep),(int)(model->MaxTime*0.4),(int)(model->MaxTime*0.4/tStep),(int)(model->MaxTime*0.6),(int)(model->MaxTime*0.6/tStep),(int)(model->MaxTime*0.8),(int)(model->MaxTime*0.8/tStep),(int)(model->MaxTime),(int)(model->MaxTime/tStep));

//        std::cout << buffer << std::endl;

        fprintf(gnuplotPipe,"set pm3d \n unset surface\n "
                "set xlabel \"Constant\" \n"
                "set zlabel \"Probability\"\n"
                "set ylabel \"Time\"\n"
//                "set xtics (\"0\" 0, \"2\" 10, \"4\" 20,\"6\" 30, \"8\" 40) \n "
//                "set ytics (\"0\" 0, \"20\" 40, \"40\" 80,\"60\" 120, \"80\" 160, \"100\" 200) \n "
                "set xtics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d) \n "
                "set ytics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d, \"%d\" %d) \n "
                "set palette rgbformulae 33,13,10 \n",(int)cStart,(int)0,(int)(cStart+(cEnd-cStart)*0.25),(int)(((cEnd-cStart)/cStep)*0.25),(int)(cStart+(cEnd-cStart)*0.50),(int)(((cEnd-cStart)/cStep)*0.50),(int)(cStart+(cEnd-cStart)*0.75),(int)(((cEnd-cStart)/cStep)*0.75),(int)cEnd,(int)((cEnd-cStart)/cStep),(int)0,(int)0,(int)(model->MaxTime*0.2),(int)(model->MaxTime*0.2/tStep),(int)(model->MaxTime*0.4),(int)(model->MaxTime*0.4/tStep),(int)(model->MaxTime*0.6),(int)(model->MaxTime*0.6/tStep),(int)(model->MaxTime*0.8),(int)(model->MaxTime*0.8/tStep),(int)(model->MaxTime),(int)(model->MaxTime/tStep));

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"splot \"%s\" matrix with lines\n",QString2Char(QString("./output/%1_3d.dat").arg(placeName)));
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"exit \n");
        fclose(gnuplotPipe);
    }
//    std::cout << fv->dtrm << std::endl;
//    delete fv;
    delete modelChecker;
    freeMarking(initialMarking);
    return true;
}

bool Facade::showProbFunc(double c, double tStep, double maxTime) {
    if (model == 0 || placeName == 0) {
        return false;
    }

    unsigned int pIndex = -1;
    double amount = 0;

//    unsigned int tIndex = 0;
//    for (int i = 0; i < model->N_transitions; i++) {
//        if (model->places[i].type != TT_DETERMINISTIC) continue;
//        if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

//        tIndex = i;
//    }

//    double time = model->transitions[tIndex].time;

//    FunctionVars *fv = new FunctionVars;

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    model->MaxTime = maxTime;
    //model->MaxTime = 50 + time;
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
    //  TimedDiagram::getInstance()->scale = 20;
    //    std::cout << "Writing the debug region diagram...." << std::endl;
    //    std::stringstream ss;
    //    ss << "temp" << "_rd";
    //    TimedDiagram::getInstance()->saveDiagram(ss.str());
    //    cv::Mat flipped;
    //    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    //    cv::imshow("STD Diagram Plot", flipped);

    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic);

    if (!modelChecker->setVariables()) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    std::cout << "starting measure computation..." << std::endl;
    //Model* model = TimedDiagram::getInstance()->model;
    for (int i = 0; pIndex == (unsigned)-1 && i < model->N_places; i++) {
        if (model->places[i].type != PT_FLUID) continue;
        if (strncmp(model->places[i].id,QString2Char(placeName), strlen(model->places[i].id)) != 0) continue;
        pIndex = i;
    }

    if (pIndex == (unsigned)-1) {
        guic->addError("The place name is set to an invalid place name. This functionality only works for fluid place names.");
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    std::ofstream oFile;
    oFile.open(QString2Char(QString("./output/%1_2d.dat").arg(placeName)), std::ios::out);

    mtime = 0; seconds = 0; useconds = 0;
    timeval t0, t1;
//    char *argFinder;
//    switch (model->transitions[gTransitionId(model)].df_distr)
//    {
//    case Exp:
//    case Gamma: fv->lambda = atof(model->transitions[gTransitionId(model)].df_argument);
//        break;
//    case Uni:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->a = atof(argFinder);
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->b = atof(argFinder);
//        break;
//    case Gen:
//        fv->f = evaluator_create (model->transitions[gTransitionId(model)].df_argument);
//        break;
//    case FoldedNorm:
//    case Norm:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->mu = atof(argFinder);
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            delete fv;
//            freeMarking(initialMarking);
//            return false;
//        }
//        fv->sigma = atof(argFinder);
//        break;
//    case Dtrm:
//        fv->dtrm = atof(model->transitions[gTransitionId(model)].df_argument);
//        break;
//    }
    amount = c;
    for (double t = 0.2; t <= model->MaxTime + .01; t += tStep){
        //if (c < model->places[fv->pIndex].f_bound) {
            //std::cout << "t = " << t << "--amount = " << amount << std::endl;
            gettimeofday(&t0, NULL);

            double p = modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(t, pIndex, amount),0.00);

//            //double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
//            switch (model->transitions[gTransitionId(model)].df_distr)
//            {
//            case Exp: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfExp, modelChecker->propertyXleqCTest);
//                break;
//            case Uni: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfUni, modelChecker->propertyXleqCTest);
//                break;
//            case Gen: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfGen, modelChecker->propertyXleqCTest);
//                break;
//            case Gamma: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfGamma, modelChecker->propertyXleqCTest);
//                break;
//            case Norm: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfNormal, modelChecker->propertyXleqCTest);
//                break;
//            case FoldedNorm: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfFoldedNormal, modelChecker->propertyXleqCTest);
//                break;
//            case Dtrm: p = TimedDiagram::getInstance()->calProbAtTime(t, modelChecker->scdfDtrm, modelChecker->propertyXleqCTest);
//            }
            gettimeofday(&t1, NULL);

            seconds  += t1.tv_sec  - t0.tv_sec;
            useconds += t1.tv_usec - t0.tv_usec;
            //if (p > 1) p = 1;
            oFile << " "<< p;
            //std::cout << " "<< p;
//        } else {
//            guic->addText("Warning: The constant is greater than the fluid bound of that place.");
//            oFile << " " << 0;
//        }

        oFile << std::endl;
        //std::cout << std::endl;
    }

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }
    mtime = ((seconds * 1000 + useconds/1000.0) + 0.5);
    std::cout << "total time computing measures:  " << mtime << "ms" << std::endl;

    oFile.close();
    FILE* gnuplotPipe;
    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
    {

        char buffer[400];
//                sprintf(buffer, "set title 'Place constant 2d-plot '\n "
//                        "set xlabel \"Time\" \n"
//                        "set ylabel \"Probability\"\n"
//        //                "set term gif\n"
//        //                "set output \"./output/%s_2d.gif\"\n"

//                        "set xtics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d, \"%d\" %d) \n "
//                        /*"set xrange[1:%f] \n "*//*,placeName.toAscii().data()*/,(int)0,(int)0,(int)(model->MaxTime*0.2),(int)(model->MaxTime*0.2/tStep),(int)(model->MaxTime*0.4),(int)(model->MaxTime*0.4/tStep),(int)(model->MaxTime*0.6),(int)(model->MaxTime*0.6/tStep),(int)(model->MaxTime*0.8),(int)(model->MaxTime*0.8/tStep),(int)(model->MaxTime),(int)(model->MaxTime/tStep)/*,model->MaxTime*/);

                std::cout << buffer << std::endl;
        fprintf(gnuplotPipe,"set title 'Place constant 2d-plot '\n "
                "set xlabel \"Time\" \n"
                "set ylabel \"Probability\"\n"
//                "set term gif\n"
//                "set output \"./output/%s_2d.gif\"\n"

                "set xtics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d, \"%d\" %d) \n "
                /*"set xrange[1:%f] \n "*//*,placeName.toAscii().data()*/,(int)0,(int)0,(int)(model->MaxTime*0.2),(int)(model->MaxTime*0.2/tStep),(int)(model->MaxTime*0.4),(int)(model->MaxTime*0.4/tStep),(int)(model->MaxTime*0.6),(int)(model->MaxTime*0.6/tStep),(int)(model->MaxTime*0.8),(int)(model->MaxTime*0.8/tStep),(int)(model->MaxTime),(int)(model->MaxTime/tStep)/*,model->MaxTime*/);

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"plot \"%s\" title \"probability\" with lp \n",QString2Char(QString("./output/%1_2d.dat").arg(placeName)));
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"exit \n");
        fclose(gnuplotPipe);
    }
    delete modelChecker;
    freeMarking(initialMarking);
    return true;
}

bool Facade::modelCheck(bool &res, QString rawFormula, QString rawCheckTime, double maxTime) {

    /*
     * TODO: Parse the STL formula
     * Use Flex + Lemon, YACC or Bison, rather than ANTLR or own implementation.
     * Flex (Lexer) : Scans the lines and transforms it into tokens.
     * Lemon (Parser) : Creates the datastructure of the AST and gives initial values to the different type of formulas.
     */

    if (model == 0) {
        return false;
    }

    /*
     * Traverse through the STL formula.
     * Traversal algorithm is based on a post-order strategy.
     */

    double checkTime = rawCheckTime.toDouble();
    if (!(checkTime >= 0)) {
        guic->addError("The chosen time to check is invalid, please specify a new time.");
    }

//    unsigned int tIndex = 0;
//    for (int i = 0; i < model->N_transitions; i++) {
//        if (model->places[i].type != TT_DETERMINISTIC) continue;
//        if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

//        tIndex = i;
//    }

//    double time = model->transitions[tIndex].time;

    InitializeModel(model);

//    guic->addText(QString("Tijd : %1").arg(time).toStdString());

    model->MaxTime = maxTime;

    Marking* initialMarking = createInitialMarking(model);

    TimedDiagram::getInstance()->setModel(model);

    TimedDiagram::getInstance()->generateDiagram(initialMarking);

    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic, checkTime);

    Formula *fullFML;
    if (!modelChecker->parseFML(fullFML, rawFormula)) {
        delete modelChecker;
        freeMarking(initialMarking);
//        delete fullFML;
        return false;
    };
    std::cout << "starting measure computation..." << std::endl;

    //InitializeModel(model);

    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    //model->MaxTime = checkTime + 100.0;

    if (!modelChecker->setVariables()) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        delete fullFML;
        return false;
    }

    if (!modelChecker->traverseFML(res, fullFML)) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        delete fullFML;
        return false;
    }
    res ? guic->addSuccess("Yes! The formula is satisfied.") : guic->addSuccess("No! The formula is not satisfied.");

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }

    delete modelChecker;
    freeMarking(initialMarking);
    delete fullFML;

    return true;
}

//bool Facade::tempUntilModelCheck() {
//    if (model == 0 || placeName == 0) {
//        return false;
//    }

//    unsigned int pIndex, disPIndex;
//    ModelChecker modelChecker(model, TimedDiagram::getInstance());
//    for (int i = 0; i < model->N_places; i++) {
//        //if (model->places[i].type != PT_FLUID) continue;
//        if (model->places[i].type == PT_FLUID && strncmp(model->places[i].id, placeName.toAscii().data(), strlen(placeName.toAscii().data())) == 0)  pIndex = i;;
//        if (model->places[i].type == PT_DISCRETE && strncmp(model->places[i].id, "Input1On", strlen("Input1On")) == 0)  disPIndex = i;;
//    }

//    unsigned int tIndex = 0;
//    for (int i = 0; i < model->N_transitions; i++) {
//        if (model->places[i].type != TT_DETERMINISTIC) continue;
//        if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

//        tIndex = i;
//    }

//    double time = model->transitions[tIndex].time;
//    std::cout << "time: " <<  time << std::endl;
//    std::cout << "pIndex: " <<  pIndex << std::endl;
//    std::cout << "disPIndex: " <<  disPIndex << std::endl;


//    InitializeModel(model);
//    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

//    model->MaxTime = time + 50.0;
//    Marking* initialMarking = createInitialMarking(model);

//    long mtime, seconds, useconds;

//    char *argFinder;
//    int a;

//    switch (model->transitions[gTransitionId(model)].df_distr)
//    {
//    case Exp: modelChecker.setLambda(atoi(model->transitions[gTransitionId(model)].df_argument));
//        modelChecker.setDistr(Exp);
//        break;
//    case Gamma: modelChecker.setLambda(atoi(model->transitions[gTransitionId(model)].df_argument));
//        modelChecker.setDistr(Gamma);
//        break;
//    case Uni:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            freeMarking(initialMarking);
//            return false;
//        }
//        a = atoi(argFinder);
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for uni{a,b}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            freeMarking(initialMarking);
//            return false;
//        }
//        modelChecker.setAB(a,atoi(argFinder));
//        modelChecker.setDistr(Uni);
//        break;
//    case Gen:
//        modelChecker.setF(evaluator_create (model->transitions[gTransitionId(model)].df_argument));
//        modelChecker.setDistr(Gen);
//        break;
//    case Norm:
//        argFinder = strtok(model->transitions[gTransitionId(model)].df_argument,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            freeMarking(initialMarking);
//            return false;
//        }
//        modelChecker.setMu(atoi(argFinder));
//        argFinder = strtok(NULL,",");
//        if (argFinder == NULL) {
//            guic->addText(QString("Transition #%1 : %2 has an invalid cumulative distribution function (cdf) for norm{mu,sigma}, since this requires two arguments.").arg(gTransitionId(model)+1).arg(model->transitions[gTransitionId(model)].id).toStdString());
//            freeMarking(initialMarking);
//            return false;
//        }
//        modelChecker.setSigma(atoi(argFinder));
//        modelChecker.setDistr(Norm);
//        break;
//    }

//    TimedDiagram::getInstance()->setModel(model);

//    timeval gt1, gt2;
//    gettimeofday(&gt1, NULL);
//    TimedDiagram::getInstance()->generateDiagram(initialMarking);
//    gettimeofday(&gt2, NULL);

//    seconds  = gt2.tv_sec  - gt1.tv_sec;
//    useconds = gt2.tv_usec - gt1.tv_usec;
//    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

//    std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
//    std::cout << "Time to generate STD: " << mtime<< "ms" << std::endl;


//    TimedDiagram::getInstance()->scale = 20;
////    if (argc == 4){
////        std::cout << "Writing the debug region diagram...." << std::endl;
////        std::stringstream ss;
////        ss << argv[1] << "_rd";
////        TimedDiagram::getInstance()->saveDiagram(ss.str());
////        cv::Mat flipped;
////        cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
////        cv::imshow("test", flipped);
////        cv::waitKey(0);
////    }
//    AtomContFormula* psi1 = new AtomContFormula(0,0,pIndex, 0.1);

//    AtomDisFormula* psi2_dis = new AtomDisFormula(0,0,disPIndex, 1);
//    AtomContFormula* psi2_cont = new AtomContFormula(0,0,pIndex, 3);
//    ADFormula* psi2 = new ADFormula(0,0,psi2_cont, psi2_dis);


//    modelChecker.debugImage = TimedDiagram::getInstance()->debugImage;
//    modelChecker.scale = TimedDiagram::getInstance()->scale;

//    std::ofstream oFile("./results.out");
//    std::ofstream oInt("./intervals.out");
//    for (double T = 1; T < 11; T += 1){
//        Interval bound(0, time+T);
//        IntervalSet* i1 = modelChecker.tempUntil(psi1, psi2, bound, time);

//        std::cout << "The satisfaction set of Until formula: " << std::endl;
//        for (unsigned int i = 0; i < i1->intervals.size(); i++){
//            i1->intervals[i].end = i1->intervals[i].end - time;
//            i1->intervals[i].start = i1->intervals[i].start - time;
//        }

//        std::stringstream out;
//        i1->print(out);
//        i1->print(oInt << std::endl);

//        std::string s = out.str();
//        QString qs = QString::fromAscii(s.data(), s.size());
//        guic->addText(QString("Interval: %1 - Probability: %2").arg(qs).arg(modelChecker.calcProb(i1, 0)).toStdString());

//        oFile << modelChecker.calcProb(i1, 0) << std::endl;
//    }

//    switch (model->transitions[gTransitionId(model)].df_distr)
//    {
//    case Gen: evaluator_destroy(modelChecker.getF());
//        break;
//    }

//    FILE* gnuplotPipe;
//    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
//    {
//        fprintf(gnuplotPipe,"set title 'Model Checking plot '\n "
//                "set xlabel \"Time Bound\" \n"
//                "set ylabel \"Probability\"\n"
//                "set xrange[1:10] \n ");

//        fflush(gnuplotPipe);
//        fprintf(gnuplotPipe,"plot \"%s\" title \"probability\" with lp \n",QString2Char(QString("./results.out")));
//        fflush(gnuplotPipe);
//        fprintf(gnuplotPipe,"exit \n");
//        fclose(gnuplotPipe);
//    }

//    freeMarking(initialMarking);

//    return true;
//}


}
