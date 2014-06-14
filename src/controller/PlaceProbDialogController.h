#ifndef PLACEPROBDIALOGCONTROLLER_H
#define PLACEPROBDIALOGCONTROLLER_H

#include <QDialog>

namespace Ui {
class PlaceProbDialogController;
}

class PlaceProbDialogController : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlaceProbDialogController(QWidget *parent = 0, QString fileName="", bool isDES=false);
    ~PlaceProbDialogController();
    QString getPlaceName();
    double getMaxTime();
    double getTimeStep();
    int getDESRuns();
    double getConstStep();
    double getConstStart();
    double getConstEnd();
    double getConst();
    bool checkConstRange();
    bool checkSpecConst();
    
private:
    Ui::PlaceProbDialogController *ui;
};

#endif // PLACEPROBDIALOGCONTROLLER_H
