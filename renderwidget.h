#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QVector>
#include <QTime>

#include <QDebug>


class Edge;
class Node {
public:
    QPoint p;
    Node *upper,*lower,*right,*left;
    Edge *et,*eb,*er,*el;

    Node (QPoint p):p(p){
        upper = lower = right = left = NULL;
        et = eb = er = el = NULL;
    }
    inline int x() const{
        return p.x();
    }
    inline int y() const{
        return p.y();
    }


};
class Edge {
public:
    Node *n1,*n2;
    Edge (Node *n1,Node *n2):n1(n1),n2(n2){
    }
    void change_adj(Node *cur,Node *new_adj){
        if(cur==n1){
            n2 == new_adj;
        } else if(cur==n2){
            n1 == new_adj;
        } else {
            qDebug() << "Adj.Pt error!!";
        }
    }
    QVector<QPoint> line; // inter-node
};

class RenderWidget : public QWidget
{
    Q_OBJECT
public:


    void paintEvent(QPaintEvent * e /* event */);

    RenderWidget(QWidget *parent);
private:
    int LW,LH,RW,RWHalf,RH,RHHalf;
    int width,height;


    QVector<Edge*> edges;
    QVector<Node*> nodes;

    void debugPt(QPainter &painter,QPoint p, QColor color=Qt::red);
    void drawResult(QPainter &painter);
    void clearVector();
    void createEdge(Node *n1, Node *n2, bool LeftRight);
    void genCellSize(int &w,int &h);
    Node* createNode(QPoint pt);
};

#endif // RENDERWIDGET_H
