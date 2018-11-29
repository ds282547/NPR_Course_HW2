#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
{
    /*
    QImage image(800,600,QImage::Format_RGBA8888);
    QPainter paint;
    paint.begin(&image);
    paint.setBrush(Qt::NoBrush);
    paint.setPen(Qt::red);
    paint.setFont(QFont("Arial",10));
    paint.drawText(100,100,"Fucker");
    paint.end();
    image.save("image.png");
    */
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}


void GLWidget::debugMat3(GLWidget::mat3 m)
{
    qDebug() << m.m[0] << "," << m.m[1] << "," << m.m[2];
    qDebug() << m.m[3] << "," << m.m[4] << "," << m.m[5];
    qDebug() << m.m[6] << "," << m.m[7] << "," << m.m[8];
}

void GLWidget::initializeGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

}

void GLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width(),height(),0);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{

}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{

}


