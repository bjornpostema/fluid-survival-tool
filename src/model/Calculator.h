#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "DFPN2.h"
#include "../controller/GUIController.h"
#include <QString>
#include <QTextEdit>

namespace model {

class Calculator
{
public:
    Calculator(QString rawFileName, QString rawPlaceName, GUIController *guic);
    virtual ~Calculator();
    bool virtual setPlace(QString rawPlaceName);
    bool virtual setFile(QString rawFileName);
    bool virtual showSTD(QString rawFileName);
    bool virtual showProbFunc();
    bool virtual modelCheck(QString rawFormula, QString rawCheckTime);

    Model *model;
    QString fileName;
    QString placeName;
    GUIController *guic;

private:
    const char* QString2Char(QString rawQString);
};

}

#endif // CALCULATOR_H
