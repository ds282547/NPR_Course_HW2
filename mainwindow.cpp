#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //signals
    connect(ui->pushButtonClear, SIGNAL(released()), ui->glwidget , SLOT(clearScreen()));
    ui->splitter->setSizes(QList<int>() << 100 << 100);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButtonDraw_released()
{


}