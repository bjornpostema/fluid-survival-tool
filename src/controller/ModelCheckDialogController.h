#ifndef MODELCHECKDIALOGCONTROLLER_H
#define MODELCHECKDIALOGCONTROLLER_H

#include <QDialog>

namespace Ui {
class ModelCheckDialogController;
}

class ModelCheckDialogController : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModelCheckDialogController(QWidget *parent = 0, QString fileName="");
    ~ModelCheckDialogController();
    QString getTTC();
    QString getFormula();
    double getMaxTime();
    
private:
    Ui::ModelCheckDialogController *ui;
};

#endif // MODELCHECKDIALOGCONTROLLER_H
