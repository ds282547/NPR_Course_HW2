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
    Node *base_node,*last_RB,*temp_node2;
    Node *cur_RT,*last_RT;
    bool notLastCell = true;
    bool sameHeight;

    QList<Node*> skyline;

    // 左上角第一格
    genCellSize(w,h);
    LB = createNode(QPoint(0,h));
    RB = createNode(QPoint(w,h));
    RT = createNode(QPoint(w,0));

    base_node = LB; // for next row
    createEdge(LB,RB,true); // Left to right
    createEdge(RT,RB,false); // Top to bottom
    tx = w;
    skyline.push_back(LB);skyline.push_back(RB);

    notLastCell = true;
    do {
        last_h = h;
        last_RB = RB;
        genCellSize(w,h);
        if(tx+w>width){
            w = width-tx;
            notLastCell = false;
        }

        if (sameHeight = (last_h == h) ){
            //share right-bottom pt
            qDebug() << "same height";
            LB = last_RB;
        } else {
            LB = createNode(QPoint(tx,h));
        }
        RB = createNode(QPoint(tx+w,h));

        skyline.push_back(LB);skyline.push_back(RB);

        createEdge(LB,RB,true);
        // Omit Last Edge
        if(notLastCell) {
            RT = createNode(QPoint(tx+w,0));
            createEdge(RT,RB,false);
        }
        // Draw Higher Line
        if(!sameHeight) {
            if(h>last_h){
                createEdge(last_RB,LB,false);

            } else if(h<last_h){
                // split edge
                temp_node2 = last_RB->upper;
                last_RB->upper = LB;
                last_RB->et->change_adj(last_RB,LB);
                createEdge( temp_node2,LB,false);
            }
        }

        tx+=w;

    } while (tx<width);

    /*
    for(auto it=skyline.begin();it!=skyline.end();it++){
        debugPt(painter,(*it)->p);
    }
    */

    // 第二層

    auto cur_it = skyline.begin();
    auto bcktr_it = cur_it;
    last_RT = (*cur_it);
    last_RB = NULL;
    ty = 0;
    tx = 0;
    notLastCell = true;
    //for(int i=0;i<5;++i){
    do {

        //qDebug() << "====";
        genCellSize(w,h);



        if(tx+w>width){
            w = width-tx;
            notLastCell = false;
        }

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
        if(notLastCell)
            createEdge(RT,RB,false); // Top to bottom

        if(last_RB){
            if(last_RB->y() > LB->y()){
                createEdge(LB,last_RB,false);
            } else if (last_RB->y() < LB->y()){
                createEdge(last_RB,LB,false);
            }
        }

        Node *bcktr = RT;

        Node *edgeRightPt;
        bool isRightPt; // for top-edge

        bcktr_it = cur_it;

        if ((*cur_it)->x()>tx+w && (*cur_it)->y()==ty){
            //split ---.---
            //         |
            bcktr_it--;

            (*bcktr_it)->right = RT;
            (*bcktr_it)->er->n2 = RT;
            //qDebug() << bcktr_it - skyline.begin();
            createEdge(RT,*cur_it,true);
            cur_it = skyline.insert(bcktr_it+1,RT);

            isRightPt = false;
        } else {
            // no split new edge
            //  ----------
            //
            //   ------.
            //         |
            isRightPt = true;
            edgeRightPt = RT;
        }

        while (cur_it!=skyline.begin() && (*bcktr_it)->x()>tx){
            if ( (*bcktr_it)->y()==ty){
                isRightPt = !isRightPt;
                if ( isRightPt){
                    edgeRightPt = *bcktr_it;

                } else {
                    createEdge(*bcktr_it,edgeRightPt,true);
                }

            }
            bcktr_it--;

        }
        // 左上角的上層是密合邊
        if ((*bcktr_it)->y()==ty){

            if (last_RT->y() > ty){
                //split
                LT = createNode(QPoint(tx,ty));
                Node* temp_node = (*bcktr_it)->right;
                (*bcktr_it)->right = LT;
                (*bcktr_it)->er->n2 = LT;
                // 水平左側分割
                createEdge(LT,temp_node,true);
                // 垂直分割
                createEdge(LT,last_RT,false);
            } else if (last_RT->y() == ty){
                LT = last_RT;
            }
        }
        // 上邊與上層有空隙
        if (isRightPt){
            debugPt(painter,last_RT->p);
            if (last_RT->y() > ty){
                LT = createNode(QPoint(tx,ty));
                createEdge(LT,last_RT,false);
            } else if (last_RT->y() == ty){
                LT = last_RT;
            } else {
                //split
                LT = createNode(QPoint(tx,ty));

                if (last_RT->lower) {
                    createEdge(LT , last_RT->lower,false);
                    last_RT->lower = LT;
                    last_RT->eb->n2 = LT;
                }
            }
            createEdge(LT,edgeRightPt,true);
        }
        last_RT = RT;
        last_RB = RB;
        tx+=w;
    } while(tx<width);
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
