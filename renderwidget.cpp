#include "renderwidget.h"

void RenderWidget::paintEvent(QPaintEvent *e)
{

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0,0,512,512);
    painter.setBrush(Qt::NoBrush);
    drawResult(painter);


}


RenderWidget::RenderWidget(QWidget *parent):QWidget(parent)
{
    LW = 50;
    LH = 40;
    RW = 50;
    RH = 20;
    //與上次的間隔
    WI = 4;
    HI = 4;
    RWHalf = RW/2;
    RHHalf = RH/2;
    width = 512;
    height = 512;
    rand_seed = 401;

    displacementBase = 2;
    displacementLimit = 5; //too small rock don't do displacement

    genAllStep();
}

void RenderWidget::regenRock()
{
    rand_seed = QTime::currentTime().msec();
    genAllStep();
}

void RenderWidget::genBaseRock()
{
    qsrand(rand_seed);



    clearVector();


    // generate first row
    int tx,ty,last_tx,h,w,last_h;
    Edge *e1;
    Node *LT,*RT,*RB,*LB;
    Node *base_node,*last_RB,*temp_node2;
    Node *cur_RT,*last_RT;
    bool notLastCell = true;
    bool sameHeight;

    QList<Node*> skyline;
    QList<Node*> cur_skyline;

    // 左上角第一格
    genCellSize(w,h);
    LT = createNode(QPoint(0,0));
    LB = createNode(QPoint(0,h));
    RB = createNode(QPoint(w,h));
    RT = createNode(QPoint(w,0));


    base_node = LB; // for next row
    createEdge(LB,RB,true); // Left to right
    createEdge(RT,RB,false); // Top to bottom

    createEdge(LT,LB,false,false); // For Board
    createEdge(LT,RT,true,false); // For Board
    tx = w;
    skyline.push_back(LB);skyline.push_back(RB);

    LT = RT;

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
        // Omit Last Edge(X) (NO NEED ANYMORE)
        RT = createNode(QPoint(tx+w,0));
        createEdge(RT,RB,false,notLastCell); // Last Cell For Right Board
        // Draw Higher Line
        if(!sameHeight) {
            if(h>last_h){
                createEdge(last_RB,LB,false);

            } else if(h<last_h){
                // split edge
                temp_node2 = last_RB->upper;
                last_RB->upper = LB;
                LB->lower = last_RB;
                last_RB->et->n1 = LB;
                LB->eb = last_RB->et;
                createEdge( temp_node2,LB,false);


            }
        }
        createEdge(LT,RT,true,false); // For Top Board
        LT = RT;
        tx+=w;


    } while (tx<width);

    /*
    for(auto it=skyline.begin();it!=skyline.end();it++){
        debugPt(painter,(*it)->p);
    }
    */

    // 第二層以後
    unsigned i=2,j=1;
    bool lastRow;


    do {

    bool firstCell = true;
    auto cur_it = skyline.begin();
    auto bcktr_it = cur_it;
    last_RT = (*cur_it);
    last_RB = NULL;
    ty = 0;
    tx = 0;
    notLastCell = true;

    j=1;
    do {
        bool cond = (i==8) && (j == 7);
        //qDebug() << i << j;
        if(i==8 && j == 8){
            return;
        }
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

        if(last_RB && last_RB->upper && LB->p==last_RB->upper->p){
            //很小機率目前方塊左下與上塊方塊右上共點
            LB=last_RB;
        } else {
            if(last_RB && last_RB->y()==ty+h)
                LB = last_RB;
            else
                LB = createNode(QPoint(tx+0,ty+h));
        }
        RB = createNode(QPoint(tx+w,ty+h));


        if ((*cur_it)->x() == tx+w){
            Node *top = *cur_it;
            if(cur_it+1 != skyline.end()){
                top = *(cur_it+1);
                if(top)
            }
            RT = *cur_it;

        }else
            RT = createNode(QPoint(tx+w,ty));

        if(firstCell){
            firstCell=false;
            createEdge(last_RT,LB,false,false); // For left board
        }


        createEdge(LB,RB,true); // Left to right

        createEdge(RT,RB,false,notLastCell); // Top to bottom



        if(last_RB){

            if(last_RB->upper && last_RB->upper->y() >= LB->y()){
                if (last_RB->upper->y()!=LB->y()){
                    createEdge(LB,last_RB->upper,false);
                }
            }else{
                if(last_RB->y() > LB->y()){
                    //split
                    Node *temp = last_RB->upper;

                    if(temp){
                        if(temp->p!=LB->p){
                            temp->lower = LB;
                            temp->eb->n2 = LB;
                            LB->upper = temp;
                            LB->et = temp->eb;
                        }
                    }
                    createEdge(LB,last_RB,false);





                } else if (last_RB->y() < LB->y()){

                    createEdge(last_RB,LB,false);
                }
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
            RT->left = (*bcktr_it);
            (*bcktr_it)->er->n2 = RT;
            RT->el = (*bcktr_it)->er;
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




        // 檢查整個上面的邊 -> 從上個row的skyline右邊到左邊檢查(backward)
        while (cur_it!=skyline.begin() && (*bcktr_it)->x()>tx){


            if ( (*bcktr_it)->y()==ty){


                isRightPt = !isRightPt;
                if ( isRightPt){
                    edgeRightPt = *bcktr_it;

                } else if((*bcktr_it)->x()!=edgeRightPt->x()){

                    createEdge(*bcktr_it,edgeRightPt,true);
                }

            } else if((*(bcktr_it-1))->y()<ty){ // 去掉重疊的case

                //vacant space case 1
                auto bcktr_it_next = bcktr_it-1;
                if((*bcktr_it_next)->x() == (*bcktr_it)->x()){
                    //選出y大的

                    Node *bottom = (*bcktr_it_next)->y() > (*bcktr_it)->y() ? (*bcktr_it_next) : (*bcktr_it);
                    Node *edgePt = createNode(QPoint(bottom->x(),ty));

                    createEdge(bottom, edgePt ,false);
                    createEdge(edgePt, edgeRightPt,true);
                    edgeRightPt = edgePt;

                }
            }

            bcktr_it--;

        }

        // 左上角的上層是密合邊


        if ((*bcktr_it)->y()==ty){
            if (last_RT->y() > ty){

                //同一點(機率小)
                if ((*bcktr_it)->x()==tx){
                    LT = (*bcktr_it);
                }else{
                    //split
                    LT = createNode(QPoint(tx,ty));
                    Node* temp_node = (*bcktr_it)->right;
                    (*bcktr_it)->right = LT;
                    LT->left = (*bcktr_it);
                    (*bcktr_it)->er->n2 = LT;
                    LT->el = (*bcktr_it)->er;

                    // 水平左側分割
                    createEdge(LT,temp_node,true);
                }

                // 垂直分割
                createEdge(LT,last_RT,false);





            } else if (last_RT->y() == ty){

                LT = last_RT;
            } else {

            }

        }



        // 上邊與上層有空隙
        if (isRightPt){

            //debugPt(painter,last_RT->p);
            bool vacant_case = true;
            if (last_RT->y() > ty){
                LT = createNode(QPoint(tx,ty));

                createEdge(LT,last_RT,false);

            } else if (last_RT->y() == ty){
                LT = last_RT;
                vacant_case = false;
            } else {
                //split
                LT = createNode(QPoint(tx,ty));

                if (last_RT->lower && last_RT->lower->p != LT->p) {

                    createEdge(LT , last_RT->lower,false);
                    last_RT->lower = LT;
                    last_RT->eb->n2 = LT;
                    LT->upper = last_RT;
                    LT->et = last_RT->eb;
                }
                vacant_case = false;
            }

            createEdge(LT,edgeRightPt,true);





            //vacant space case 2-1
            if (last_RT->y() > (*bcktr_it)->y() && (*bcktr_it)->y() == (*(bcktr_it+1))->y() && (*bcktr_it)->x() < last_RT->x()){
                Node* vc_center;
                if (vacant_case){
                    vc_center = createNode(QPoint(LT->x(),(*bcktr_it)->y()));
                    createEdge(vc_center, LT, false);

                    //debugPt(LT->p);
                } else {
                    vc_center = createNode(QPoint(last_RT->x(),(*bcktr_it)->y()));
                    createEdge(vc_center, last_RT, false);
                }
                //split
                Node* vc_temp = (*bcktr_it)->right;
                (*bcktr_it)->right = vc_center;
                (*bcktr_it)->er->n2 = vc_center;
                vc_center->left = (*bcktr_it);
               vc_center->el = (*bcktr_it)->er;
                createEdge(vc_center,vc_temp,true);
            }


        }

        last_RT = RT;
        last_RB = RB;
        tx+=w;
        cur_skyline.push_back(LB);cur_skyline.push_back(RB);

        j++;
    } while(tx<width);


        skyline.swap(cur_skyline);
        cur_skyline.clear();

        //檢查skyline看做完了沒 全部超過height就是做完了
        lastRow = true;
        for(auto skyit = skyline.begin();skyit!=skyline.end();++skyit){
            lastRow = lastRow && ( (*skyit)->y() >= height );
        }

        i++;
    } while (!lastRow);


}

void RenderWidget::genAllStep()
{
    useFloatPoints = false;
    genBaseRock();
    //displacement();
    convertToFloatPoints();
    //subdivision();
    //determineStoneArea();



}

void RenderWidget::displacement()
{

    for(auto it=nodes.begin();it!=nodes.end();it++){
        Node* node = *it;
        if(node->x()==0 || node->x()==height || node->y()==0 || node->y()==width)
            continue;
        unsigned order = (node->er != NULL) + (node->el != NULL) + (node->et != NULL) + (node->eb != NULL);
        int dx=0,dy=0;
        if(order==3){
            if(!node->er){
                // displace left
                dx = -getDisplaceAmount(node->x() - node->left->x());
            } else if(!node->el){
                // displace right
                dx=getDisplaceAmount(node->right->x() - node->x());
            } else if(!node->et){
                // displace lower

                dy=getDisplaceAmount(node->lower->y() - node->y());
            } else if(!node->eb){
                // displace upper

                dy=-getDisplaceAmount(node->y() - node->upper->y());

            }
            node->p+=QPoint(dx,dy);
        }
    }
}

void RenderWidget::subdivision()
{
    std::default_random_engine g( time(NULL) );
    std::uniform_real_distribution<float> u(-1.0, 1.0);


    int iteration_count;

    static const double lg2 = log(2);
    for(auto it=edges.begin();it!=edges.end();it++){

        Edge* ed = *it;
        if(!ed->inner) continue;
        ed->line.push_back(ed->n1->pf);
        ed->line.push_back(ed->n2->pf);

        iteration_count = (int)(log(ed->distance())/lg2) - 2;

        if(iteration_count<0)
            iteration_count==0;
        if(ed->distance()==0){
            qDebug() << ed->n1->p << ed->n2->p << rand_seed;
        }

        qreal displacement_reduction;
        int iteration_pow2 = 1;

        for(int i=0;i<iteration_count;++i,iteration_pow2<<=1){

            QVector<QPointF>::iterator pt=ed->line.begin();
            QVector<QPointF>::iterator pt_next=pt+1;

            displacement_reduction = (qreal)1/(iteration_pow2);

            while(pt_next!=ed->line.end()){
                QPointF midpoint = ((*pt)+(*pt_next))/2;
                QPointF displaceDir;
                displaceDir.setX(pt->y()-pt_next->y());
                displaceDir.setY(pt->x()-pt_next->x());

                displaceDir/=qSqrt(QPointF::dotProduct(displaceDir,displaceDir));

                midpoint+=displaceDir*(u(g)*3*displacement_reduction);

                pt = ed->line.insert(pt_next,midpoint);


                pt++;
                pt_next = pt+1;
            }
        }

    }
}

void RenderWidget::determineStoneArea()
{
    // U R D L
    // 0 1 2 3
    int candidates[4]={1,2,3,0}; // R D L U
    int choosen_dir;
    int count = 0;
    bool db = false;
    for(auto it=nodes.begin();it!=nodes.end();it++){

        Node* node = *it;

        db = (node->x()==348 &&node->y()==316);
        if(db){
            qDebug()<<node->lower;
        }
        if(!db){
           //continue;
        }

        Node* step;
        for(int i=0;i<4;++i){
            step = node;
            if(step->dir(i) && !step->dir_visited[i]){

                Rock* rock = new Rock();
                choosen_dir = i;
                rock->addPoint(step);
                step->dir_visited[choosen_dir] = true;
                step = step->dir(i);

                bool found = true;


                while(step!=node){
                    rock->addPoint(step);
                    if(step->dir( candidates[choosen_dir] )){
                        // 1st Candidate
                        choosen_dir = candidates[choosen_dir];

                       step->dir_visited[choosen_dir] = true;
                       step = step->dir(choosen_dir);
                    } else if(step->dir( choosen_dir )){
                        // 2nd Candidate

                        step->dir_visited[choosen_dir] = true;
                        step = step->dir(choosen_dir);
                    } else {
                        // Fail
                        found = false;
                        break;
                    }
                }
               if(found){
                   rocks.push_back(rock);
               }else{
                   delete rock;
               }
            }
        }

    }
}

int RenderWidget::getDisplaceAmount(int limitDis)
{

    if(limitDis<=displacementLimit){
        return 0;
    } else {
        limitDis/=3;
        limitDis-=displacementBase;

        if(limitDis>0)
            return (rand() % limitDis) + displacementBase;
        else
            return 0;
    }
}

void RenderWidget::debugPt(QPointF p)
{
    debugPtVector.push_back(p);
}

void RenderWidget::debugPt(QPainter &painter, QPointF p, QColor color)
{
    painter.setBrush(color);
    painter.drawEllipse(p,2,2);
}

void RenderWidget::drawResult(QPainter &painter)
{
    //painter.setRenderHint(QPainter::Antialiasing, true);
    if(!useFloatPoints){
        convertToFloatPoints();
    }

    for(auto it=edges.begin();it!=edges.end();it++){
        painter.setPen(QColor(rand()%200,rand()%200,rand()%200));
        Edge* ed = *it;
        QPainterPath path;

        path.moveTo(ed->n1->pf);
        if(!ed->line.isEmpty()){
            for(auto m=ed->line.begin();m!=ed->line.end();m++){
                path.lineTo(*m);
            }
        }
        path.lineTo(ed->n2->pf);
        painter.drawPath(path);
        //if(ed->n2->p==QPoint(0,0)){
            //qDebug() << ed->n1;
        //}
    }

    for(auto it=debugPtVector.begin();it!=debugPtVector.end();it++){
        debugPt(painter,*it);
    }
    QBrush b(QColor(255,0,0,50));
    painter.setBrush(b);
    for(auto it=rocks.begin();it!=rocks.end();it++){

        painter.drawPolygon((*it)->poly);
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
    for(QVector<Rock*>::iterator it=rocks.begin();it!=rocks.end();it++){
        delete (*it);
    }
    rocks.clear();

    //clear debug pt list
    QVector<QPointF> empty;
    debugPtVector.swap(empty);

}

void RenderWidget::createEdge(Node *n1, Node *n2, bool LeftRight, bool innerEdge)
{
    Edge *edge = new Edge(n1,n2,innerEdge);
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
    static int lastW = -1;
    static int lastH = -1;
    w = LW + (qrand() % RW) - RWHalf;
    h = LH + (qrand() % RH) - RHHalf;
    // 跟上次的差距必須超過一定間格
    if(lastW>=0){
        while(abs(lastW-w)<WI){
            w = LW + (qrand() % RW) - RWHalf;
        }
    }
    if(lastH>=0){
        while(abs(lastH-h)<HI){
            h = LH + (qrand() % RH) - RHHalf;
        }
    }
    lastW = w;
    lastH = h;

}

void RenderWidget::convertToFloatPoints()
{
    useFloatPoints = true;
    for(auto it=nodes.begin();it!=nodes.end();it++){
        (*it)->copyToFloatPt();
    }
}

Node* RenderWidget::createNode(QPoint pt)
{
    Node *node = new Node(pt);
    nodes.push_back(node);
    return node;
}

