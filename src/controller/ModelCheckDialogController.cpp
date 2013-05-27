#include "ModelCheckDialogController.h"
#include "ui_ModelCheckDialogView.h"

ModelCheckDialogController::ModelCheckDialogController(QWidget *parent, QString fileName) :
    QDialog(parent),
    ui(new Ui::ModelCheckDialogController)
{
    ui->setupUi(this);
    ui->labelPathToFile->setText(fileName);
}

ModelCheckDialogController::~ModelCheckDialogController()
{
    delete ui;
}

QString ModelCheckDialogController::getTTC() {
    return ui->lineEditTTC->text();
}

QString ModelCheckDialogController::getFormula() {
    return ui->comboFormula->currentText();
}

double ModelCheckDialogController::getMaxTime() {
    return ui->lineEditMaxTime->text().toDouble();
}
