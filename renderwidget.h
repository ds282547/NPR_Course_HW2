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
#include <qmath.h>
#include <random>


class Edge;
class Node {
public:
    QPoint p;
    QPointF pf;
    Node *upper,*right,*lower,*left;
    Edge *et,*er,*eb,*el;

    bool dir_visited[4]={0};

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
    inline qreal xf() const{
        return pf.x();
    }
    inline qreal yf() const{
        return pf.y();
    }
    Node *dir(unsigned d){
        return *(&upper+d);
    }
    void debugEdge(){
        qDebug() << (upper?"Upper ":"") << (lower?"Lower ":"") << (right?"Right ":"") << (left?"Left ":"");
    }
    void copyToFloatPt(){
        pf = p;
    }

};
class Edge {
public:
    Node *n1,*n2;
    bool inner;
    Edge (Node *n1,Node *n2,bool inner):n1(n1),n2(n2),inner(inner){
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
    qreal distance(){
        QPointF d = n1->pf - n2->pf;
        return sqrt(QPointF::dotProduct(d,d));
    }
    QVector<QPointF> line; // inter-node
};

class Rock{
    public:
    QPolygonF poly;
    void addPoint(Node *&n){
        poly.push_back(n->pf);
    }
};

class RenderWidget : public QWidget
{
    Q_OBJECT
public:


    void paintEvent(QPaintEvent * e /* event */);

    RenderWidget(QWidget *parent);
    void regenRock();
    void genBaseRock();
    void genAllStep();
    void displacement();
    void subdivision();
    void determineStoneArea();

    int getDisplaceAmount(int limitDis);
private:
    int LW,LH,RW,RWHalf,RH,RHHalf,WI,HI;
    int width,height;

    int displacementBase;
    int displacementLimit;

    bool useFloatPoints;
    QVector<Edge*> edges;
    QVector<Node*> nodes;
    QVector<Rock*> rocks;

    Node* gg;

    QVector<QPointF> debugPtVector;

    int rand_seed;

    void debugPt(QPointF p);

    void debugPt(QPainter &painter,QPointF p, QColor color=Qt::red);
    void drawResult(QPainter &painter);
    void clearVector();
    void createEdge(Node *n1, Node *n2, bool LeftRight, bool innerEdge=true);
    void genCellSize(int &w,int &h);

    void convertToFloatPoints();
    Node* createNode(QPoint pt);
};

#endif // RENDERWIDGET_H
