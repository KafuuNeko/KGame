#include "window_main.h"
#include "ui_window_main.h"

#include "dialog_about.h"

#include <qwidget.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton->setText("Init Context");

    connect(ui->pushButton, &QPushButton::clicked, [this](){
        DialogAbout dialog(this);
        dialog.exec();
    });
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

