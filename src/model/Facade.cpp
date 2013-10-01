/**
 * @file Facade.cpp
 * @author B.F. Postema
 * @brief The Facade following the facade pattern.
 * The facade pattern gives a class to access the subsystem of the models. The controllers send requests to the facade.
 */

#include "Facade.h"
#include "../includes/opencv/cv.h"
extern "C" {
#include "../includes/matheval.h"
}

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
    timeval start, end;
    long mtime_full,mtime_std, seconds, useconds;
    int regionAmount;
    gettimeofday(&start, NULL);

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

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

    model->MaxTime = maxTime;
    Marking* initialMarking = createInitialMarking(model);
    seconds = 0, useconds = 0;

    TimedDiagram::getInstance()->setModel(model);

    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);

    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime_std = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    regionAmount = TimedDiagram::getInstance()->getNumberOfRegions();
    //std::cout << "Number of regions: " << TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    //std::cout << "Time to generate STD: " << mtime_std << "ms" << std::endl;

    TimedDiagram::getInstance()->scale = imageScale;
    //std::cout << "Writing the debug region diagram...." << std::endl;
    std::stringstream ss;
    ss << "./output/" << rawFileName.toStdString() << "_std";
    TimedDiagram::getInstance()->saveDiagram(ss.str());
    cv::Mat flipped;
    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    cv::imshow("STD Diagram Plot", flipped);
    freeMarking(initialMarking);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime_full = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    guic->addText(QString("Number of regions: %1").arg(regionAmount).toStdString());
    guic->addText(QString("Time to generate STD: %1 ms").arg(mtime_std).toStdString());
    guic->addText(QString("Total execution time: %1 ms").arg(mtime_full).toStdString());

    return true;
}

bool Facade::showProbFunc(double cStart, double cEnd, double cStep, double tStep, double maxTime) {
    timeval start, end;
    long mtime_full,mtime_std,mtime_measures, seconds, useconds;
    int regionAmount;
    gettimeofday(&start, NULL);

    if (model == 0 || placeName == 0) {
        return false;
    }

    unsigned int pIndex = -1;
    double amount = 0;

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    model->MaxTime = maxTime;

    Marking* initialMarking = createInitialMarking(model);
    seconds = 0; useconds = 0;

    TimedDiagram::getInstance()->setModel(model);
    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);
    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime_std = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    regionAmount = TimedDiagram::getInstance()->getNumberOfRegions();
    //std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    //std::cout << "Time to generate STD: " << mtime_std<< "ms" << std::endl;

    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic);

    if (!modelChecker->setVariables()) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    //std::cout << "starting measure computation..." << std::endl;

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
    oFile.open(QString2Char(QString("./output/%1_3d.dat").arg(placeName)), std::ios::out);

    seconds = 0; useconds = 0;
    timeval t0, t1;
    gettimeofday(&t0, NULL);
    for (double t = .02; t <= model->MaxTime + .01; t += tStep){
        for (amount = cStart+.05; amount <= cEnd; amount += cStep){

            double p = modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(t, pIndex, amount),0.00);

            oFile << " "<< p;
        }
        oFile << std::endl;
    }

    gettimeofday(&t1, NULL);

    seconds  += t1.tv_sec  - t0.tv_sec;
    useconds += t1.tv_usec - t0.tv_usec;

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }
    mtime_measures = ((seconds * 1000 + useconds/1000.0) + 0.5);
    //std::cout << "total time computing measures:  " << mtime_measures << "ms" << std::endl;

    oFile.close();
    FILE* gnuplotPipe;
    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
    {
////      GNUPlot debugging...
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

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime_full = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    guic->addText(QString("Number of regions: %1").arg(regionAmount).toStdString());
    guic->addText(QString("Time to generate STD: %1 ms").arg(mtime_std).toStdString());
    guic->addText(QString("Total time computing measures: %1 ms").arg(mtime_measures).toStdString());
    guic->addText(QString("Total execution time: %1 ms").arg(mtime_full).toStdString());

    return true;
}

bool Facade::showProbFunc(double c, double tStep, double maxTime) {
    timeval start, end;
    long mtime_full,mtime_std,mtime_measures,seconds,useconds;
    int regionAmount;
    gettimeofday(&start, NULL);

    if (model == 0 || placeName == 0) {
        return false;
    }

    unsigned int pIndex = -1;
    double amount = 0;

    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.
    model->MaxTime = maxTime;

    Marking* initialMarking = createInitialMarking(model);

    TimedDiagram::getInstance()->setModel(model);
    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);
    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime_std = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    regionAmount = TimedDiagram::getInstance()->getNumberOfRegions();
    //std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    //std::cout << "Time to generate STD: " << mtime_std<< "ms" << std::endl;

    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic);

    if (!modelChecker->setVariables()) {
        // Clean-up code
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    }

    //std::cout << "starting measure computation..." << std::endl;
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

    seconds = 0; useconds = 0;
    timeval t0, t1;
    amount = c;
    gettimeofday(&t0, NULL);
    for (double t = 0.2; t <= model->MaxTime + .01; t += tStep){

            double p = modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(t, pIndex, amount),0.00);

            oFile << " "<< p;

        oFile << std::endl;
    }

    gettimeofday(&t1, NULL);

    seconds  += t1.tv_sec  - t0.tv_sec;
    useconds += t1.tv_usec - t0.tv_usec;

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }

    mtime_measures = ((seconds * 1000 + useconds/1000.0) + 0.5);
    //std::cout << "total time computing measures:  " << mtime_measures << "ms" << std::endl;

    oFile.close();
    FILE* gnuplotPipe;
    if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
    {

        char buffer[400];
////              GNUPlot debugging...
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
                "set xtics (\"%d\" %d, \"%d\" %d, \"%d\" %d,\"%d\" %d, \"%d\" %d, \"%d\" %d) \n "
                ,(int)0,(int)0,(int)(model->MaxTime*0.2),(int)(model->MaxTime*0.2/tStep),(int)(model->MaxTime*0.4),(int)(model->MaxTime*0.4/tStep),(int)(model->MaxTime*0.6),(int)(model->MaxTime*0.6/tStep),(int)(model->MaxTime*0.8),(int)(model->MaxTime*0.8/tStep),(int)(model->MaxTime),(int)(model->MaxTime/tStep)/*,model->MaxTime*/);

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"plot \"%s\" title \"probability\" with lp \n",QString2Char(QString("./output/%1_2d.dat").arg(placeName)));
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe,"exit \n");
        fclose(gnuplotPipe);
    }
    delete modelChecker;
    freeMarking(initialMarking);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime_full = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    guic->addText(QString("Number of regions: %1").arg(regionAmount).toStdString());
    guic->addText(QString("Time to generate STD: %1 ms").arg(mtime_std).toStdString());
    guic->addText(QString("Total time computing measures: %1 ms").arg(mtime_measures).toStdString());
    guic->addText(QString("Total execution time: %1 ms").arg(mtime_full).toStdString());
    return true;
}

bool Facade::modelCheck(bool &res, QString rawFormula, QString rawCheckTime, double maxTime) {
    /* Simulation implemented for stress-test purposes. */
    int simruns = 1; /* default should be 1, so the algorithm is performed once */
    timeval start, end;
    long mtime_full = 0, mtime_std = 0, mtime_measures = 0, seconds = 0, useconds = 0;
    int regionAmount = 0;
    //double resProb;
    for (int i=0;i<simruns;i++) {
    //resProb = 0.0;
    gettimeofday(&start, NULL);

    /*
     * Parse the STL formula
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

    InitializeModel(model);

    model->MaxTime = maxTime;

    Marking* initialMarking = createInitialMarking(model);

    TimedDiagram::getInstance()->setModel(model);

    seconds = 0; useconds = 0;

    timeval gt1, gt2;
    gettimeofday(&gt1, NULL);
    TimedDiagram::getInstance()->generateDiagram(initialMarking);
    gettimeofday(&gt2, NULL);
    seconds  = gt2.tv_sec  - gt1.tv_sec;
    useconds = gt2.tv_usec - gt1.tv_usec;
    mtime_std += ((seconds) * 1000 + useconds/1000.0) + 0.5;

    regionAmount = TimedDiagram::getInstance()->getNumberOfRegions();
    //std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
    //std::cout << "Time to generate STD: " << mtime_std<< "ms" << std::endl;

    seconds = 0; useconds = 0;
    timeval t0, t1;
    gettimeofday(&t0, NULL);
    ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic, checkTime);

    Formula *fullFML;
    if (!modelChecker->parseFML(fullFML, rawFormula)) {
        delete modelChecker;
        freeMarking(initialMarking);
        return false;
    };
    //std::cout << "starting measure computation..." << std::endl;

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

    //std::cout << "total time computing measures:  " << mtime_measures << "ms" << std::endl;

    switch (model->transitions[gTransitionId(model)].df_distr)
    {
    case Gen: evaluator_destroy(modelChecker->getF());
        break;
    }

    delete modelChecker;
    freeMarking(initialMarking);
    delete fullFML;

    gettimeofday(&t1, NULL);

    seconds  += t1.tv_sec  - t0.tv_sec;
    useconds += t1.tv_usec - t0.tv_usec;

    mtime_measures += ((seconds * 1000 + useconds/1000.0) + 0.5);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime_full += ((seconds) * 1000 + useconds/1000.0) + 0.5;
    }

    mtime_std = mtime_std /*/ simruns*/;
    mtime_measures = mtime_measures /*/ simruns*/;
    mtime_full = mtime_full /*/ simruns*/;

    //guic->addText(QString("Probability : %1").arg(resProb).toStdString());
    res ? guic->addSuccess("Yes! The formula is satisfied.") : guic->addSuccess("No! The formula is not satisfied.");
    guic->addText(QString("Number of regions: %1").arg(regionAmount).toStdString());
    guic->addText(QString("Time to generate STD: %1 ms").arg(mtime_std).toStdString());
    guic->addText(QString("Total time computing measures: %1 ms").arg(mtime_measures).toStdString());
    guic->addText(QString("Total execution time: %1 ms").arg(mtime_full).toStdString());

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
