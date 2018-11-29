#include "circlebutton.h"

CircleButton::CircleButton(QWidget *parent) : QWidget(parent)
{
    resize(50,50);
    this->setAttribute(Qt::WA_NoSystemBackground);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void CircleButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setOpacity(0.5);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::SolidLine);
    painter.setBrush(Qt::gray);
    painter.save();
    painter.drawEllipse(0,0,width(),height());

    painter.restore();
}
