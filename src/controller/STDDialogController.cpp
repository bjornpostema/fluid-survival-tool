#include "STDDialogController.h"
#include "ui_STDDialogView.h"

STDDialogController::STDDialogController(QWidget *parent, QString fileName) :
    QDialog(parent),
    ui(new Ui::STDDialogController)
{
    ui->setupUi(this);
    ui->labelPathToFile->setText(fileName);
}

STDDialogController::~STDDialogController()
{
    delete ui;
}

double STDDialogController::getMaxTime() {
    return ui->lineEditMaxTime->text().toDouble();
}

int STDDialogController::getImageScale() {
    return ui->lineEditImageScale->text().toInt();
}
