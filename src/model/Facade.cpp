/**
 * @file Facade.cpp
 * @author B.F. Postema
 * @brief The Facade following the facade pattern.
 * The facade pattern gives a class to access the subsystem of the models. The controllers send requests to the facade.
 */

#include "Facade.h"
#include "opencv/cv.h"
extern "C" {
#include "matheval.h"
}

namespace model {
    Facade::Facade(QString rawFileName, QString rawPlaceName, Logger *newGuic, bool allowMultipleGeneralTransitions)
    {
        guic = newGuic;
        model = 0;
        placeName = QString();
        fileName  = QString();

        if (setFile(rawFileName, allowMultipleGeneralTransitions)) {
            setPlace(rawPlaceName);
        }
    }

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
        return setFile(rawFileName, false);
    }

    bool Facade::setFile(QString rawFileName, bool allowMultipleGeneralTransitions) {
        if (!rawFileName.isEmpty()) {
            fileName = rawFileName;
            guic->addText(QString("File name set to: ").append(fileName).toStdString());
        } else {
            guic->addText("File name was already set.");
            return false;
        }

        model = ReadModel(QString2Char(fileName), guic, allowMultipleGeneralTransitions);
        if (model == NULL) {
            guic->addError(QString("Model could not be read or parsed.").toStdString());
            return false;
        } else {
            guic->addText(QString("Model is read and parsed.").toStdString());
            return true;
        }
    }

    const char* Facade::QString2Char(QString rawQString) {
        QByteArray qba  = rawQString.toUtf8();
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

    /**
     * @brief Facade::showProbFunc Shows the 2D probability graph for a given constant of the currently loaded model.
     * @param c The constant
     * @param tStep The step size of the graph
     * @param maxTime The maximum time to calculate probabilities for.
     * @return true
     */
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
        for (double t = 0; t <= model->MaxTime + .01; t += tStep){

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

    /**
     * @brief Facade::showProbFunc Shows the 2D probability graph for a given constant of the currently loaded model, calculated using DES.
     * @param fluidPlaceMinimumConstant The constant
     * @param runsPerStep The number of simulation runs per step of the graph
     * @param simulationTimeStep The step size of the graph
     * @param simulationTimeMax The maximum time to calculate probabilities for.
     * @return true if completed successfully, false otherwise.
     */
    bool Facade::showDESProbFunc(double fluidPlaceMinimumConstant, int runsPerStep, double simulationTimeStep, double simulationTimeMax) {
        if(runsPerStep <= 0) {
            return false;
        }
        return _showDESProbFunc(fluidPlaceMinimumConstant, fluidPlaceMinimumConstant, 0, runsPerStep, simulationTimeStep, simulationTimeMax);
    }

    /**
     * @brief Facade::showProbFunc Shows the 2D probability graph for a given constant of the currently loaded model, calculated using DES.
     * @param fluidPlaceMinimumConstant The constant
     * @param runsPerStep The number of simulation runs per step of the graph
     * @param simulationTimeStep The step size of the graph
     * @param simulationTimeMax The maximum time to calculate probabilities for.
     * @return true if completed successfully, false otherwise.
     */
    bool Facade::showDESProbFunc(double fluidPlaceMinimumStart, double fluidPlaceMinimumEnd, double fluidPlaceMinimumStep, int runsPerStep, double simulationTimeStep, double simulationTimeMax) {
        // Check the input
        if(fluidPlaceMinimumStep <= 0) {
            return false;
        }
        if(fluidPlaceMinimumStart > fluidPlaceMinimumEnd) {
            return false;
        }
        if(runsPerStep <= 0) {
            return false;
        }
        return _showDESProbFunc(fluidPlaceMinimumStart, fluidPlaceMinimumEnd, fluidPlaceMinimumStep, runsPerStep, simulationTimeStep, simulationTimeMax);
    }

    /**
     * @brief Facade::showProbFunc Shows the 2D or 3D probability graph for a given constant of the currently loaded model, calculated using DES.
     * @param fluidPlaceMinimumConstant The constant
     * @param runsPerStep The number of simulation runs per step of the graph
     * @param simulationTimeStep The step size of the graph
     * @param simulationTimeMax The maximum time to calculate probabilities for.
     * @return true if completed successfully, false otherwise.
     */
    bool Facade::_showDESProbFunc(double fluidPlaceMinimumStart, double fluidPlaceMinimumEnd, double fluidPlaceMinimumStep, int runsPerStep, double simulationTimeStep, double simulationTimeMax) {
        // Should we do a new randomized run for each constant step?
        const bool runPerFluidPlaceMinimum = true;

        // The file used for reading random data from
        const std::string randomDataFile = "/dev/urandom";

        // The starting time for the simulation
        const double simulationTimeMin = 0;

        // Store the init time
        timeval timeInit;
        gettimeofday(&timeInit, NULL);

        // Check if the model is loaded and a place name is set
        if (model == 0 || placeName == 0) {
            return false;
        }

        // Will we be doing DES?
        bool isDES = runsPerStep > 0;

        // Initialize the model
        InitializeModel(model);

        // Set the maximum time
        model->MaxTime = simulationTimeMax;

        // Determine the index of the specified fluid place
        int fluidPlaceIndex;
        for (fluidPlaceIndex = 0; fluidPlaceIndex < model->N_places; fluidPlaceIndex++) {
            if (model->places[fluidPlaceIndex].type == PT_FLUID && strncmp(model->places[fluidPlaceIndex].id,QString2Char(placeName), strlen(model->places[fluidPlaceIndex].id)) == 0) {
                break;
            }
        }
        if (fluidPlaceIndex >= model->N_places) {
            guic->addError("The place name is set to an invalid place name. This functionality only works for fluid place names.");
            return false;
        }

        // Open the output file; this is were the plot data will be written to
        std::string outputFileName = std::string("./output/") + placeName.toStdString() + std::string(isDES ? "_des_" : "_") + std::string(fluidPlaceMinimumStep > 0 ? "3" : "2") + std::string("d.dat");
        std::ofstream outputFile;
        outputFile.open(outputFileName.c_str());

        // Determine the number of constant steps
        int fluidPlaceMinimumStepCount = 1;
        if(fluidPlaceMinimumStep > 0) {
            fluidPlaceMinimumStepCount += (fluidPlaceMinimumEnd - fluidPlaceMinimumStart) / fluidPlaceMinimumStep;
        }

        // Store the DES init time
        timeval timeInitDES;
        gettimeofday(&timeInitDES, NULL);

        // Some variables for use with DES
        FILE* randomDataFileHandle = NULL;
        struct generalTransitionData {
            int index;
            int df_distr;
            char* df_argument;
        };
        int generalTransitionCount = 0;
        generalTransitionData* generalTransitions = NULL;

        // DES-specific initialization
        if(isDES) {
            // Try to access the random data file
            randomDataFileHandle = fopen(randomDataFile.c_str(), "r");
            if(!randomDataFileHandle) {
                guic->addText("Failed to open random data file: " + randomDataFile);
                return false;
            }

            // Find all general transition and store their properties
            generalTransitionCount = model->N_generalTransitions;
            generalTransitions = new generalTransitionData[generalTransitionCount];
            for (int transitionIndex = 0, generalTransitionIndex = 0; transitionIndex < model->N_transitions && generalTransitionIndex < generalTransitionCount; transitionIndex++) {
                if (model->transitions[transitionIndex].type == TT_GENERAL) {
                    // Save the properties
                    generalTransitions[generalTransitionIndex].index = transitionIndex;
                    generalTransitions[generalTransitionIndex].df_distr = model->transitions[transitionIndex].df_distr;
                    generalTransitions[generalTransitionIndex].df_argument = model->transitions[transitionIndex].df_argument;

                    // We process the first transition differently
                    if(generalTransitionIndex == 0) {
                        // Set an deterministic distribution for the transition
                        model->transitions[transitionIndex].df_distr = Dtrm;
                        model->transitions[transitionIndex].idInMarking = 0;
                    } else {
                        // Make the transition fully deterministic
                        model->transitions[transitionIndex].type = TT_DETERMINISTIC;
                        model->transitions[transitionIndex].df_distr = 0;
                        model->transitions[transitionIndex].df_argument = NULL;
                        model->N_generalTransitions -= 1;
                        model->transitions[transitionIndex].idInMarking = model->N_determTransitions;
                        model->N_determTransitions += 1;
                    }

                    // Increase the count of processed general transitions
                    generalTransitionIndex++;
                }
            }
        } else {
            // We need at least 1 run for the analytical approach
            runsPerStep = 1;
        }

        // Create the initial marking
        Marking* initialMarking = createInitialMarking(model);

        // Clear the current list of markings, for memory leak countering
        clearAllocatedMarkings();

        // Set up the STD
        TimedDiagram::getInstance()->setModel(model);
        TimedDiagram::getInstance()->generateDiagram(initialMarking);
        ModelChecker *modelChecker = new ModelChecker(model, TimedDiagram::getInstance(), guic);

        // Check if setting the variables works correctly
        if (!modelChecker->setVariables()) {
            // Clean-up code
            delete modelChecker;
            freeMarking(initialMarking);
            return false;
        }

        // Store the starting time
        timeval timeStart;
        gettimeofday(&timeStart, NULL);

        // Calculate the probability for each step in the graph
        for (double simulationTime = simulationTimeMin; simulationTime <= simulationTimeMax + .01; simulationTime += simulationTimeStep) {
            // The probabilities to calculate
            double* probabilities = new double[fluidPlaceMinimumStepCount];

            // More simulation
            int fluidPlaceRun = 0;
            for(double fluidPlaceMinimumConstant = fluidPlaceMinimumStart; fluidPlaceMinimumConstant <= fluidPlaceMinimumEnd + .01; fluidPlaceMinimumConstant += fluidPlaceMinimumStep) {
                // Calculate the probability for a number of times
                for (int run = 0; run < runsPerStep; run++) {
                    // Loop over all general transitions and pick them a random value
                    if(isDES) for (int generalTransitionIndex = 0; generalTransitionIndex < generalTransitionCount; generalTransitionIndex++) {
                        // Read a random unsigned long from the handle
                        unsigned long x;
                        if(fread(&x, sizeof(x), 1, randomDataFileHandle) != 1) {
                            guic->addText("Failed to read (enough) random data from file: " + randomDataFile);
                            delete modelChecker;
                            freeMarking(initialMarking);
                            delete generalTransitions;
                            fclose(randomDataFileHandle);
                            return false;
                        }

                        // Calculate a uniform distrubuted double value
                        double randomValue = (double) x / ULONG_MAX;

                        // Apply the correct distribution
                        randomValue = modelChecker->calculateDistributionValue(generalTransitions[generalTransitionIndex].df_distr, generalTransitions[generalTransitionIndex].df_argument, randomValue);

                        // Check the generated value
                        if(randomValue < 0) {
                            guic->addError("Failed to generate a valid value for general transition.");
                            delete modelChecker;
                            freeMarking(initialMarking);
                            delete generalTransitions;
                            fclose(randomDataFileHandle);
                            return false;
                        }

                        // Save the value to the model
                        if(generalTransitionIndex == 0) {
                            char randomValueBuffer[255];
                            sprintf(randomValueBuffer, "%.10f", randomValue);
                            model->transitions[generalTransitions[generalTransitionIndex].index].df_argument = randomValueBuffer;
                        } else {
                            model->transitions[generalTransitions[generalTransitionIndex].index].time = randomValue;
                        }
                    }

                    // Regenerate the STD
                    TimedDiagram::getInstance()->clear();
                    TimedDiagram::getInstance()->generateDiagram(initialMarking);
                    if(!modelChecker->setVariables()) return false;

                    // Calculate the probabilities using the model methods
                    if(!runPerFluidPlaceMinimum && fluidPlaceMinimumStep > 0) {
                        int i = 0;
                        for(fluidPlaceMinimumConstant = fluidPlaceMinimumStart; fluidPlaceMinimumConstant <= fluidPlaceMinimumEnd + .01; fluidPlaceMinimumConstant += fluidPlaceMinimumStep) {
                            if(run == 0) probabilities[i] = 0;
                            probabilities[i] += modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(simulationTime, fluidPlaceIndex, fluidPlaceMinimumConstant), 0.00);
                            i++;
                        }
                    } else {
                        if(run == 0) probabilities[fluidPlaceRun] = 0;
                        probabilities[fluidPlaceRun] += modelChecker->calcProb(modelChecker->calcAtomContISetAtTime(simulationTime, fluidPlaceIndex, fluidPlaceMinimumConstant), 0.00);
                    }

                    // Free the newly generated markings
                    freeAllocatedMarkings();
                }
                fluidPlaceRun++;
                if(!runPerFluidPlaceMinimum || fluidPlaceMinimumStep <= 0) break;
            }

            // Calculate and output the averages
            for(int i = 0; i < fluidPlaceMinimumStepCount; i++) {
                if(i > 0) outputFile << " ";
                char probabilityBuffer[255];
                sprintf(probabilityBuffer, "%.10f", probabilities[i] / runsPerStep);
                outputFile << (std::string) probabilityBuffer;
            }

            // Output the calculated probability to the file
            outputFile << std::endl;

            // Delete the separate results
            delete probabilities;
        }

        // Store the ending time
        timeval timeEnd;
        gettimeofday(&timeEnd, NULL);

        // DES de-init
        if(isDES) {
            // Delete the general transition data
            delete generalTransitions;

            // Close random file handle
            fclose(randomDataFileHandle);
        }

        // Close the output file handle
        outputFile.close();

        // Free the memory used for the initial marking
        delete modelChecker;
        freeMarking(initialMarking);

        // Run GNUPlot to display the graph
        FILE* gnuplotPipe;
        if((gnuplotPipe = popen("gnuplot -persist", "w"))) {
            if(fluidPlaceMinimumStep > 0) {
                fprintf(gnuplotPipe,
                    "set title 'Place constant 3D-plot%s'\n"
                    "set pm3d\n"
                    "unset surface\n"
                    "set xlabel \"Constant\"\n"
                    "set ylabel \"Time\"\n"
                    "set zlabel \"Probability\"\n"
                    "set xtics (\"%.1f\" 0, \"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f)\n "
                    "set ytics (\"0\" 0, \"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f)\n"
                    "set palette rgbformulae 33, 13, 10\n",
                    isDES ? " DES" : "",
                    fluidPlaceMinimumStart,
                    fluidPlaceMinimumStart + (fluidPlaceMinimumEnd - fluidPlaceMinimumStart) * 0.25, ((fluidPlaceMinimumEnd - fluidPlaceMinimumStart) / fluidPlaceMinimumStep) * 0.25,
                    fluidPlaceMinimumStart + (fluidPlaceMinimumEnd - fluidPlaceMinimumStart) * 0.50, ((fluidPlaceMinimumEnd - fluidPlaceMinimumStart) / fluidPlaceMinimumStep) * 0.50,
                    fluidPlaceMinimumStart + (fluidPlaceMinimumEnd - fluidPlaceMinimumStart) * 0.75, ((fluidPlaceMinimumEnd - fluidPlaceMinimumStart) / fluidPlaceMinimumStep) * 0.75,
                    fluidPlaceMinimumEnd, (fluidPlaceMinimumEnd - fluidPlaceMinimumStart) / fluidPlaceMinimumStep,
                    simulationTimeMax * 0.2, simulationTimeMax * 0.2 / simulationTimeStep,
                    simulationTimeMax * 0.4, simulationTimeMax * 0.4 / simulationTimeStep,
                    simulationTimeMax * 0.6, simulationTimeMax * 0.6 / simulationTimeStep,
                    simulationTimeMax * 0.8, simulationTimeMax * 0.8 / simulationTimeStep,
                    simulationTimeMax, simulationTimeMax / simulationTimeStep
                );
            } else {
                fprintf(gnuplotPipe,
                    "set title 'Place constant 2D-plot%s'\n"
                    "set xlabel \"Time\"\n"
                    "set ylabel \"Probability\"\n"
                    "set xtics (\"0\" 0, \"%.1f\" %.1f, \"%.1f\" %.1f,\"%.1f\" %.1f, \"%.1f\" %.1f, \"%.1f\" %.1f)\n",
                    isDES ? " DES" : "",
                    simulationTimeMax * 0.2, simulationTimeMax * 0.2 / simulationTimeStep,
                    simulationTimeMax * 0.4, simulationTimeMax * 0.4 / simulationTimeStep,
                    simulationTimeMax * 0.6, simulationTimeMax * 0.6 / simulationTimeStep,
                    simulationTimeMax * 0.8, simulationTimeMax * 0.8 / simulationTimeStep,
                    simulationTimeMax, simulationTimeMax / simulationTimeStep
                );
            }
            fflush(gnuplotPipe);
            if(fluidPlaceMinimumStep > 0) {
                fprintf(gnuplotPipe, "splot \"%s\" matrix with lines\n", outputFileName.c_str());
            } else {
                fprintf(gnuplotPipe, "plot \"%s\" title \"probability\" with lp\n", outputFileName.c_str());
            }
            fflush(gnuplotPipe);
            fprintf(gnuplotPipe, "exit\n");
            fclose(gnuplotPipe);
        }

        // Print the total simulation time
        double timeDiffMs = (timeInitDES.tv_sec  - timeInit.tv_sec) * 1000 + (timeInitDES.tv_usec - timeInit.tv_usec) / 1000.0;
        guic->addText(QString("Basic initialisation time: %1 ms").arg(timeDiffMs).toStdString());
        if(isDES) {
            timeDiffMs = (timeStart.tv_sec  - timeInitDES.tv_sec) * 1000 + (timeStart.tv_usec - timeInitDES.tv_usec) / 1000.0;
            guic->addText(QString("DES initialisation time: %1 ms").arg(timeDiffMs).toStdString());
        }
        timeDiffMs = (timeEnd.tv_sec  - timeStart.tv_sec) * 1000 + (timeEnd.tv_usec - timeStart.tv_usec) / 1000.0;
        guic->addText(QString("Simulation time: %1 ms").arg(timeDiffMs).toStdString());
        timeDiffMs = (timeEnd.tv_sec  - timeInit.tv_sec) * 1000 + (timeEnd.tv_usec - timeInit.tv_usec) / 1000.0;
        guic->addText(QString("Total time: %1 ms").arg(timeDiffMs).toStdString());

        // Everything completed successful; return TRUE
        return true;
    }

    /**
     * @brief Facade::modelCheck
     * @param res
     * @param rawFormula
     * @param rawCheckTime
     * @param maxTime
     * @return
     */
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

}
