#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QButtonGroup>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
public slots:
    void switchStep(int step);
private slots:
    void on_pushButtonDraw_released();


    void on_pushButtonGen_released();

    void on_pushButtonLoadObj_released();

    void on_pushButtonGenWSeed_released();

private:
    Ui::MainWindow *ui;
    QButtonGroup *stepsRatios;

};

#endif // MAINWINDOW_H
