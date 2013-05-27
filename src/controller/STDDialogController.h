#ifndef STDDIALOGCONTROLLER_H
#define STDDIALOGCONTROLLER_H

#include <QDialog>

namespace Ui {
class STDDialogController;
}

class STDDialogController : public QDialog
{
    Q_OBJECT
    
public:
    explicit STDDialogController(QWidget *parent = 0, QString fileName="");
    ~STDDialogController();
    double getMaxTime();
    int getImageScale();
    
private:
    Ui::STDDialogController *ui;
};

#endif // STDDIALOGCONTROLLER_H
