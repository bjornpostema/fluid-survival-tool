#include "PlaceProbDialogController.h"
#include "ui_PlaceProbDialogView.h"

PlaceProbDialogController::PlaceProbDialogController(QWidget *parent, QString fileName) :
    QDialog(parent),
    ui(new Ui::PlaceProbDialogController)
{
    ui->setupUi(this);
    ui->labelPathToFile->setText(fileName);
}

PlaceProbDialogController::~PlaceProbDialogController()
{
    delete ui;
}

QString PlaceProbDialogController::getPlaceName() {
    return ui->lineEditPlaceName->text();
}

double PlaceProbDialogController::getMaxTime() {
    return ui->lineEditMaxTime->text().toDouble();
}

double PlaceProbDialogController::getTimeStep() {
    return ui->lineEditTimeStep->text().toDouble();
}

double PlaceProbDialogController::getConstStep() {
    return ui->lineEditConstStep->text().toDouble();
}

double PlaceProbDialogController::getConstStart() {
    return ui->lineEditConstStart->text().toDouble();
}

double PlaceProbDialogController::getConstEnd() {
    return ui->lineEditConstEnd->text().toDouble();
}

double PlaceProbDialogController::getConst() {
    return ui->lineEditConst->text().toDouble();
}

bool PlaceProbDialogController::checkConstRange() {
    return ui->radioConstRange->isChecked();
}

bool PlaceProbDialogController::checkSpecConst() {
    return ui->radioSpecConst->isChecked();
}
