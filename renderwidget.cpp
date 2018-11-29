#include "renderwidget.h"

void RenderWidget::paintEvent(QPaintEvent *e)
{

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0,0,512,512);

    qsrand(QTime::currentTime().msec());
    // generate first row
    int tx,ty,last_tx,h,w,last_h;
    Edge *e1;
    Node *LT,*RT,*RB,*LB;
    Node *base_node,*temp_node,*temp_node2;
    // first cell
    genCellSize(w,h);
    LB = createNode(QPoint(0,h));
    RB = createNode(QPoint(w,h));
    RT = createNode(QPoint(w,0));

    base_node = LB; // for next row
    createEdge(LB,RB,true); // Left to right
    createEdge(RT,RB,false); // Top to bottom
    tx = w;


    bool notLastCell = true;
    do {
        last_h = h;
        temp_node = RB;
        genCellSize(w,h);
        if(tx+w>width){
            w = width-tx;
            notLastCell = false;
        }
        LB = createNode(QPoint(tx,h));
        RB = createNode(QPoint(tx+w,h));

        createEdge(LB,RB,true);
        // Omit Last Edge
        if(notLastCell) {
            RT = createNode(QPoint(tx+w,0));
            createEdge(RT,RB,false);
        }
        // Draw Higher Line
        if(h>last_h){
            createEdge(temp_node,LB,false);

        } else if(h<last_h){
            // split edge
            temp_node2 = temp_node->upper;
            temp_node->upper = LB;
            temp_node->et->change_adj(temp_node,LB);
            createEdge( temp_node2,LB,false);
        } else {
            temp_node->right = LB;
            LB->left = temp_node;
        }
        tx+=w;

    } while (tx<width);

    genCellSize(w,h);

    temp_node = base_node;
    ty = 0;
    tx = 0;
    last_tx = 0;
    for(int i=0;i<5;++i){


        qDebug() << "====";
        ty = temp_node->p.y();
        while (temp_node->p.x()-last_tx<w){
            qDebug() << temp_node->p;
            if(temp_node->right){
                temp_node = temp_node->right;
            }else if(temp_node->lower){

                temp_node = temp_node->lower;
                qDebug() << "L = " << temp_node->p;
            } else if(temp_node->upper){
                temp_node = temp_node->upper;
                qDebug() << "U = " << temp_node->p;
            }
            ty = std::max(ty,temp_node->p.y());
        }
        LB = createNode(QPoint(last_tx+0,ty+h));
        RB = createNode(QPoint(last_tx+w,ty+h));
        RT = createNode(QPoint(last_tx+w,ty));

        createEdge(LB,RB,true); // Left to right
        createEdge(RT,RB,false); // Top to bottom

        last_tx+=w;
    }
    drawResult(painter);
    clearVector();

}

RenderWidget::RenderWidget(QWidget *parent):QWidget(parent)
{
    LW = 40;
    LH = 30;
    RW = 30;
    RH = 20;
    RWHalf = RW/2;
    RHHalf = RH/2;
    width = 512;
    height = 512;
}

void RenderWidget::drawResult(QPainter &painter)
{
    painter.setPen(Qt::black);
    for(auto it=edges.begin();it!=edges.end();it++){
        Edge* ed = *it;
        QPainterPath path;

        path.moveTo(ed->n1->p);
        path.lineTo(ed->n2->p);
        painter.drawPath(path);
    }
}

void RenderWidget::clearVector()
{
    for(QVector<Edge*>::iterator it=edges.begin();it!=edges.end();it++){
        delete (*it);
    }
    edges.clear();
    for(QVector<Node*>::iterator it=nodes.begin();it!=nodes.end();it++){
        delete (*it);
    }
    nodes.clear();

}

void RenderWidget::createEdge(Node *n1, Node *n2, bool LeftRight)
{
    Edge *edge = new Edge(n1,n2);
    edges.push_back(edge);
    if(LeftRight){
        n1->right = n2;
        n2->left = n1;
        n1->er = edge;
        n2->el = edge;
    } else {//Top Bottom
        n1->lower = n2;
        n2->upper = n1;
        n1->eb = edge;
        n2->et = edge;
    }
}

void RenderWidget::genCellSize(int &w, int &h)
{
    w = LW + (qrand() % RW) - RWHalf;
    h = LH + (qrand() % RH) - RHHalf;
}

Node* RenderWidget::createNode(QPoint pt)
{
    Node *node = new Node(pt);
    nodes.push_back(new Node(pt));
    return node;
}
