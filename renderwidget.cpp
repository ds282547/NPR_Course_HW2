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
    Node *cur_RT,*last_cur_RT;

    QList<Node*> skyline;

    // first cell
    genCellSize(w,h);
    LB = createNode(QPoint(0,h));
    RB = createNode(QPoint(w,h));
    RT = createNode(QPoint(w,0));

    base_node = LB; // for next row
    createEdge(LB,RB,true); // Left to right
    createEdge(RT,RB,false); // Top to bottom
    tx = w;
    skyline.push_back(LB);skyline.push_back(RB);

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

        skyline.push_back(LB);skyline.push_back(RB);

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

    /*
    for(auto it=skyline.begin();it!=skyline.end();it++){
        debugPt(painter,(*it)->p);
    }
    */

    auto cur_it = skyline.begin();
    auto bcktr_it = cur_it;
    ty = 0;
    tx = 0;
    for(int i=0;i<5;++i){


        //qDebug() << "====";
        genCellSize(w,h);

        ty = (*cur_it)->y();
        while ((*cur_it)->x()<tx+w){
            ty = std::max(ty,(*cur_it)->y());
            cur_it++;
        }
        LB = createNode(QPoint(tx+0,ty+h));
        RB = createNode(QPoint(tx+w,ty+h));
        if ((*cur_it)->p == QPoint(tx+w,ty))
            RT = (*cur_it);
        else
            RT = createNode(QPoint(tx+w,ty));

        createEdge(LB,RB,true); // Left to right
        createEdge(RT,RB,false); // Top to bottom


        Node *bcktr = RT;
        bcktr_it = cur_it;
        if ((*cur_it)->x()>tx+w && (*cur_it)->y()==ty){
            //split
            bcktr_it--;
            (*bcktr_it)->right = RT;
            (*bcktr_it)->er->n2 = RT;
            createEdge(RT,*cur_it,true);

        }


        while ((*bcktr_it)->x()>tx && cur_it!=skyline.begin()){

            if ( (*bcktr_it)->y()==ty){
                createEdge(*bcktr_it,RT,true);
            }
            bcktr_it--;
        }

        tx+=w;
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

void RenderWidget::debugPt(QPainter &painter, QPoint p, QColor color)
{
    painter.setBrush(color);
    painter.drawEllipse(p,2,2);
}

void RenderWidget::drawResult(QPainter &painter)
{

    for(auto it=edges.begin();it!=edges.end();it++){
        painter.setPen(QColor(rand()&255,rand()&255,rand()&255));
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
