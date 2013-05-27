#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "DFPN2.h"
#include <QString>
#include <QTextEdit>
#include "Logger.h"
#include "cmath"

#define STR_EQUAL 0

namespace model {

class Facade
{
public:
    Facade(QString rawFileName, QString rawPlaceName, Logger *guic);
    Facade(QString rawFileName, Logger *guic);
    virtual ~Facade();
    bool virtual setPlace(QString rawPlaceName);
    bool virtual setFile(QString rawFileName);
    bool virtual showSTD(QString rawFileName, double maxTime, int imageScale);
    bool virtual showProbFunc(double cStart, double cEnd, double cStep, double tStep, double maxTime);
    bool virtual showProbFunc(double c, double tStep, double maxTime);
    bool virtual modelCheck(QString rawFormula, QString rawCheckTime, double maxTime);
//    bool virtual tempUntilModelCheck();

    Model *model;
    QString fileName;
    QString placeName;
    Logger *guic;

private:
    const char* QString2Char(QString rawQString);
};

}

#endif // CALCULATOR_H
