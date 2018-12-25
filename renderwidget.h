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


class Point3{
public:
    qreal x,y,z;
    Point3 (){
        x=y=z=0;
    }
    Point3 (qreal x,qreal y,qreal z):x(x),y(y),z(z){
    }
    Point3 (const QPointF &pt,qreal z):x(pt.x()),y(pt.y()),z(z){
    }
    Point3 operator -(const Point3 &b){
        Point3 m(x-b.x,y-b.y,z-b.z);
        return m;
    }
    QPointF toPoint2D(){
        return QPointF(x,y);
    }
    void set(const QPointF &pt,qreal _z){
        x=pt.x();y=pt.y();z=_z;
    }
    qreal length(){
        return qSqrt(x*x+y*y+z*z);
    }

    void normalize(){
        qreal l=length();
        x/=l;y/=l;z/=l;
    }
};


class Triangle{
public:
    Point3 points[3];

    QRgb getNormalMapColor(){
        Point3 U = points[1] - points[0];
        Point3 V = points[2] - points[0];
        Point3 N;
        N.x = U.y*V.z - U.z*V.y;
        N.y = U.z*V.x - U.x*V.z;
        N.z = U.x*V.y - U.y*V.x;
        N.normalize();
        qreal r = qFloor(N.x*128)+128;
        qreal g = qFloor(N.y*128)+128;
        qreal b = qFloor(qAbs(N.z)*256);
        return qRgb(r,g,b);
    }
    QPolygonF getPoly(){
        QPolygonF poly;
        for(int i=0;i<3;++i)
            poly.push_back(points[i].toPoint2D());
        return poly;
    }
    void setPoint(const uint index,QPointF pt,qreal height){
        points[index].x = pt.x();
        points[index].y = pt.y();
        points[index].z = height;
    }
    void setPoint(const uint index, const Point3 &pt){
        points[index] = pt;
    }
};

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
    typedef struct {
       Node* node;
       int nextDir;
    } RNode;
    QList<RNode> nodes;
    int size;
    Rock(){
        size=0;
    }

    void addPoint(Node *&n,int nd){
        size++;
        RNode rn = {n,nd};
        nodes.push_back(rn);
    }
    QPolygonF poly(){
        QPolygonF p;
        for(auto it=nodes.begin();it!=nodes.end();it++){
            p.push_back((*it).node->pf);
        }
        return p;
    }
};

class RenderWidget : public QWidget
{
    Q_OBJECT
public:


    void paintEvent(QPaintEvent * e /* event */);

    RenderWidget(QWidget *parent);
    ~RenderWidget();
    int regenRock();
    void genBaseRock();

    void genAllStep();
    void displacement();
    void subdivision();
    bool determineStoneArea();
    void gen3DStructure();
    qreal structure_height;
    qreal structure_inner_ratio;

    int rand_seed;
    int getRandSeed();
    void genBaseRockByPixel();
    void drawPixelRect(int x,int y,int w,int h);
    bool pixels[512][512];

    qreal getDisplaceAmount(qreal limitDis);

    const int TOTAL_STEP = 5;
    int current_step;
    void showStep(int step);
    QVector<QImage*> step_images;
private:
    int LW,LH,RW,RWHalf,RH,RHHalf,WI,HI;
    int width,height;

    qreal displacementBase;
    qreal displacementLimit;

    bool useFloatPoints;
    QVector<Edge*> edges;
    QVector<Node*> nodes;
    QVector<Rock*> rocks;

    QVector<Triangle> triangles;

    Node* gg;

    QVector<QPointF> debugPtVector;
    QVector<Node*> debugNodeVector;




    void debugPt(QPointF p);
    void debugPt(Node*n);
    void debugPt(QPainter &painter,QPointF p, QColor color=Qt::red);
    void drawResult(int step);
    void clearVector();
    void createEdge(Node *n1, Node *n2, bool LeftRight, bool innerEdge=true);
    void genCellSize(int &w,int &h);

    void convertToFloatPoints();
    Node* createNode(QPoint pt);
};

#endif // RENDERWIDGET_H
