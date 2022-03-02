#include "dialog_setting.h"
#include "ui_dialog_setting.h"

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
}

DialogSetting::~DialogSetting()
{
    delete ui;
}
