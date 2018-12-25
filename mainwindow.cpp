#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //signals

    ui->splitter->setSizes(QList<int>() << 100 << 100);
    ui->plainTextEditSeed->document()->setPlainText( QString::number(ui->widget->getRandSeed()) );

    stepsRatios = new QButtonGroup(this);
    stepsRatios->addButton(ui->radioButton,0);
    stepsRatios->addButton(ui->radioButton_2,1);
    stepsRatios->addButton(ui->radioButton_3,2);
    stepsRatios->addButton(ui->radioButton_4,3);
    stepsRatios->addButton(ui->radioButton_5,4);
    connect(stepsRatios,SIGNAL(buttonReleased(int)),this,SLOT(switchStep(int)));
    ui->groupBox->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key::Key_M){
        ui->widget->regenRock();
        ui->widget->update();
    } else {
        ui->glwidget->keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    ui->glwidget->keyReleaseEvent(event);
}



void MainWindow::on_pushButtonDraw_released()
{

    ui->glwidget->sendNormalMap(ui->widget->step_images.at(3));
    ui->glwidget->sendTexture(ui->widget->step_images.at(4));

}

void MainWindow::switchStep(int step)
{
    ui->widget->showStep(step);
    qDebug() << step;
}


void MainWindow::on_pushButtonGen_released()
{
    ui->widget->genAllStep();
    ui->groupBox->setEnabled(true);

}

void MainWindow::on_pushButtonLoadObj_released()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Load Obj File"), "",tr("Obj (*.obj)"));
    if (fileName.isEmpty())
            return;
    else {
        ui->glwidget->load3DModel(fileName);
    }
}
