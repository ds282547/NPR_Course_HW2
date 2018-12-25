#include "renderwidget.h"

void RenderWidget::paintEvent(QPaintEvent *e)
{

    QPainter painter(this);
    if(current_step>=0){
        painter.drawImage(0,0,*(step_images.at(current_step)));
    }


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
    rand_seed = 921;//291; //348 //985 //755 // 65//92

    displacementBase = 1;
    displacementLimit = 3; //too small rock don't do displacement

    structure_height = 10;
    structure_inner_ratio = 0.5;

    for(int i=0;i<TOTAL_STEP;++i){
        QImage *image = new QImage(width,height,QImage::Format_RGB32);
        step_images.push_back(image);
    }

    current_step = -1;
}

RenderWidget::~RenderWidget()
{
    for(auto n=step_images.begin();n!=step_images.end();++n){
        delete (*n);
    }
}

int RenderWidget::regenRock()
{
    rand_seed = QTime::currentTime().msec();
    genAllStep();
    return rand_seed;
}

void RenderWidget::genBaseRock()
{
    qDebug() << rand_seed;
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
        bool rightTopOverlapping = false; //特殊case右上交疊
        bool LB_HigherThan_last_RT = false; // 整塊比上一塊高 最後產生左方邊的時候要考慮
        Node* rightTopOverlapping_RT;
        bool cond2 = (i==12) && (j == 6);
        bool cond = (i==10) && (j == 9);
        if(i==10&&j==10){
            //return;
        }
        //qDebug() << i << j;
        //qDebug() << "====";
        genCellSize(w,h);

        /*
        if(cond){
            return;
        }
        */

        if(tx+w>width){
            w = width-tx;

        }
        notLastCell = (tx+w)!=width;



        /*if(*(cur_it+1)){
            if((*(cur_it+1))->x() == (*cur_it)->x() && (*(cur_it+1))->y() < (*cur_it)->y()){
                ty = (*(cur_it+1))->y();
            }
        }*/




        bool special_seed_683 = false;
        auto next_cur_it = cur_it+1;
        if(next_cur_it!=skyline.end() && next_cur_it+1!=skyline.end()){
            if((*next_cur_it)->y()<(*cur_it)->y() &&
                    (*next_cur_it)->x()==tx){
               cur_it = next_cur_it;
               special_seed_683 = true;
            }
        }
        ty = (*cur_it)->y();

        while ((*cur_it)->x()<tx+w){

            ty = std::max(ty,(*cur_it)->y());
            cur_it++;
        }



        if(last_RB && last_RB->upper && QPoint(tx+0,ty+h)==last_RB->upper->p){
            //很小機率目前方塊左下與上塊方塊右上共點

            LB=last_RB->upper;
        } else {
            if(last_RB && last_RB->y()==ty+h)
                LB = last_RB;
            else
                LB = createNode(QPoint(tx+0,ty+h));
        }
        RB = createNode(QPoint(tx+w,ty+h));



        if ((*cur_it)->x() == tx+w){
            //考慮右上邊交疊狀況
            Node *top = *cur_it;
            if(cur_it+1 != skyline.end()){

                top = *(cur_it+1);
                if(top->y()<(*cur_it)->y()){
                    top = *cur_it;
                }
            }




            if(top->y() == ty){

                //共點
                RT = top;
            } else if(top->y() > ty){
                //右上覆蓋到上一列 (seed=401)

                Node *top_adj = top->upper;

                if(top_adj->p==QPoint(tx+w,ty)){
                    RT = top_adj;
                    rightTopOverlapping = true;
                    rightTopOverlapping_RT=top;
                } else {

                    //split
                    //qDebug() << "Sp";
                    RT = createNode(QPoint(tx+w,ty));
                    //qDebug()<<top->p;

                    top_adj->lower = RT;

                    top_adj->eb->n2=RT;
                    RT->upper = top_adj;
                    RT->et=top_adj->el;
                    createEdge(RT,top,false);

                    rightTopOverlapping = true;
                    rightTopOverlapping_RT=top;
                }
            } else {
                //右上離上列有距離 仍要產生新RT

                RT = createNode(QPoint(tx+w,ty));

            }

        }else{
            RT = createNode(QPoint(tx+w,ty));

        }

        if((*cur_it) && RT->x()==(*cur_it)->x() && RT->y()>(*cur_it)->y() && (*cur_it)->lower!=RT && notLastCell){
            createEdge(*cur_it,RT,false);
        }

        if(firstCell){
            firstCell=false;
            createEdge(last_RT,LB,false,false); // For left board
        }


        createEdge(LB,RB,true); // Left to right

        if(rightTopOverlapping)
            createEdge(rightTopOverlapping_RT,RB,false,notLastCell);
        else
            createEdge(RT,RB,false,notLastCell); // Top to bottom




        if(last_RB){

            if(last_RB->upper && last_RB->upper->y() >= LB->y()){

                if (last_RB->upper->y()!=LB->y()){

                    createEdge(LB,last_RB->upper,false);
                }
                LB_HigherThan_last_RT = true;
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

            edgeRightPt = (*bcktr_it);

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


        // 處理方形左方邊

        // 左上角的上層是密合邊



        if ((*bcktr_it)->y()==ty){

            if (last_RT->y() > ty){

                bool con_pt=false;

                if ((*bcktr_it)->x()==tx){
                    //同一點(機率小)
                    con_pt = true;
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

                // 垂直分割 考慮特殊狀況



                if(LB_HigherThan_last_RT){
                    createEdge(LT,LB,false);

                } else {
                    if(special_seed_683 && con_pt && LT->lower && LT->lower->p != last_RT->p){
                        debugPt(LT->lower->p);
                        debugPt(last_RT->p);
                        createEdge(LT->lower,last_RT,false);
                        qDebug()<<"683";

                    } else {
                        createEdge(LT,last_RT,false);
                    }
                }






            } else if (last_RT->y() == ty){

                LT = last_RT;
            } else {

            }

        }



        // 上邊與上層有空隙
        if (isRightPt){

            //debugPt(painter,last_RT->p);
            bool vacant_case = true;
            bool spciel_383 = false;


            if (last_RB && last_RB->y() <= ty){
                //特殊狀況 LT比上個的RB還下面 (含等號) seed:383 下面
                LT = last_RB;
                spciel_383 = true;

            } else if (last_RT->y() > ty){

                if((*bcktr_it)->x()==tx && (*bcktr_it)->y()<ty){
                    if((*bcktr_it)->lower==last_RT){
                        //bcktr到last_RT本來就有邊
                        //split
                        LT = createNode(QPoint(tx,ty));
                        (*bcktr_it)->lower=LT;
                        (*bcktr_it)->eb->n2=LT;
                        LT->upper = (*bcktr_it);
                        LT->et = (*bcktr_it)->eb;
                        createEdge(LT,last_RT,false);
                    } else {
                        //可能沒邊 bcktr有下邊 LT夾在中間

                        if((*bcktr_it)->lower && (*bcktr_it)->lower->y()>ty){
                            //split case - 985 (7,4)
                            Node *temp = (*bcktr_it)->lower;
                            LT = createNode(QPoint(tx,ty));
                            (*bcktr_it)->lower=LT;
                            (*bcktr_it)->eb->n2=LT;
                            LT->upper = (*bcktr_it);
                            LT->et = (*bcktr_it)->eb;
                            createEdge(LT,temp,false);
                            createEdge(temp,last_RT,false);


                        } else {

                            qDebug()<<"Unknown";
                            LT = createNode(QPoint(tx,ty));
                            createEdge((*bcktr_it),LT,false);
                            createEdge(LT,last_RT,false);
                            (*bcktr_it)->p+=QPoint(5,5);
                            last_RT->p+=QPoint(5,5);


                        }
                    }
                } else{

                    LT = createNode(QPoint(tx,ty));
                    createEdge(LT,last_RT,false);
                }


            } else if (last_RT->y() == ty){

                LT = last_RT;
                vacant_case = false;
            } else {
                //split
                // |
                // .----
                // |


                LT = createNode(QPoint(tx,ty));
                Node *lowest = last_RT;
                while(lowest->lower && lowest->p.y()<LT->y()){
                    lowest=lowest->lower;
                }



                if (lowest && lowest->p != LT->p) {
                    Node * lowest_uppder = lowest->upper;
                    lowest_uppder->lower = LT;
                    lowest_uppder->eb->n2 = LT;
                    LT->upper = lowest_uppder;
                    LT->et = lowest_uppder->eb;


                    createEdge(LT , lowest,false);

                }

                vacant_case = false;
            }


            createEdge(LT,edgeRightPt,true);







            //vacant space case 2-1
            if (last_RT->y() > (*bcktr_it)->y() && (*bcktr_it)->y() == (*(bcktr_it+1))->y() && (*bcktr_it)->x() < last_RT->x() && !spciel_383){
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

        // 底部同x連線


        if((*cur_it) && (*cur_it)->x() == RT->x() && (*cur_it)->y() < RT->y() && ((*cur_it)->lower!=RT) && !notLastCell){

            createEdge((*cur_it),RT,false);
        }


        last_RT = RT;
        last_RB = RB;
        tx+=w;
        cur_skyline.push_back(LB);cur_skyline.push_back(RB);

        j++;

        if(cond){
        }
        if( cond2){
        }
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
    //genBaseRockByPixel();

    useFloatPoints = false;
    genBaseRock();
    while(!determineStoneArea()){
        rand_seed = QTime::currentTime().msec();
        genBaseRock();
    }

    //draw step 0 result
    drawResult(0);

    convertToFloatPoints();
    displacement();

    drawResult(1);

    subdivision();

    drawResult(2);

    gen3DStructure();

    drawResult(3);
    drawResult(4);






}

void RenderWidget::displacement()
{
    std::default_random_engine g( time(NULL) );
    std::uniform_real_distribution<float> u(0.0, 1.0);

    for(auto it=nodes.begin();it!=nodes.end();it++){
        Node* node = *it;
        if(node->x()==0 || node->x()==height || node->y()==0 || node->y()==width)
            continue;
        unsigned order = (node->er != NULL) + (node->el != NULL) + (node->et != NULL) + (node->eb != NULL);
        QPointF dir;
        if(order==3){
            if(!node->er){
                dir = node->el->n1->pf - node->pf;

            } else if(!node->el){
                // displace right
                dir = node->er->n2->pf - node->pf;

            } else if(!node->et){
                // displace lower
                dir = node->el->n2->pf - node->pf;

            } else if(!node->eb){
                dir = node->et->n1->pf - node->pf;
            }
            dir*=u(g)*0.3;
            node->pf+=dir;
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

bool RenderWidget::determineStoneArea()
{
    // U R D L
    // 0 1 2 3
    int candidates[4]={1,2,3,0}; // R D L U
    int choosen_dir;
    int count = 0;



    bool db = false;
    for(auto it=nodes.begin();it!=nodes.end();it++){

        Node* node = *it;



        Node* step;
        for(int i=0;i<4;++i){
            step = node;

            if(step->dir(i) && !step->dir_visited[i]){
                int pt_count = 0;

                Rock* rock = new Rock();
                choosen_dir = i;
                rock->addPoint(step,i);
                step->dir_visited[choosen_dir] = true;
                step = step->dir(i);

                bool found = true;


                while(step!=node && pt_count<50){

                     pt_count++;

                    if(step->dir( candidates[choosen_dir] )){
                        // 1st Candidate
                        choosen_dir = candidates[choosen_dir];

                        rock->addPoint(step,choosen_dir);

                        step->dir_visited[choosen_dir] = true;
                        step = step->dir(choosen_dir);
                    } else if(step->dir( choosen_dir )){
                        // 2nd Candidate

                        rock->addPoint(step,choosen_dir);

                        step->dir_visited[choosen_dir] = true;
                        step = step->dir(choosen_dir);
                    } else {
                        // Fail
                        found = false;
                        break;
                    }
                }
                if(pt_count>15 && found){
                    qDebug()<<pt_count<<rock->size<<"Not OK" <<node->p;
                    //clear vector
                    delete rock;
                    for(auto r=rocks.begin();r!=rocks.end();++r){
                        delete *r;
                    }
                    rocks.clear();
                    return false;
                }
               if(found){
                   rocks.push_back(rock);
               }else{
                   delete rock;
               }
            }
        }

    }
    return true;
}

void RenderWidget::gen3DStructure()
{
    for(auto it=rocks.begin();it!=rocks.end();++it){
        Rock* rock = *it;
        QString m;
        Rock::RNode rn,rn_next;

        QPointF corner[5];
        QList<QPointF> edge[4];

        //reconstruction
        while(rock->nodes.back().nextDir != 3 || rock->nodes.first().nextDir != 0){
            Rock::RNode rn = rock->nodes.first();
            rock->nodes.pop_front();
            rock->nodes.push_back(rn);
        }


        for(auto n=rock->nodes.begin();(n+1)!=rock->nodes.end();++n){
            rn=*n;
            rn_next=*(n+1);
            if(rn.nextDir!=rn_next.nextDir){
                corner[rn.nextDir]=rn_next.node->pf;
            } else {
                edge[(rn.nextDir+3)%4].push_back(rn_next.node->pf);
            }
            m.append(QString::number(rn.nextDir));
        }
        // circularly comparsion -> with first
        rn=rn_next;rn_next=*(rock->nodes.begin());
        if(rn.nextDir!=rn_next.nextDir){

            corner[rn.nextDir]=rn_next.node->pf;
        } else {
            //reverse push
            edge[(rn.nextDir+3)%4].push_back(rn_next.node->pf);
        }
        // be circular
        corner[4]=corner[0];

        //generation
        QPointF centerPt=corner[0]+corner[1]+corner[2]+corner[3];
        centerPt/=4;

        qreal adj_ratio = (qAbs(corner[0].x()-corner[1].x())*qAbs((corner[1].y()-corner[2].y())))/2000;

        Point3 centerPt3(centerPt,structure_height*adj_ratio);



        //upper pyramid
        QPointF pyramid_corner[5];
        qreal middle_height = (1-structure_inner_ratio)*structure_height*adj_ratio ;

        for(int i=0;i<5;++i){
            pyramid_corner[i] = corner[i]*(1-structure_inner_ratio)+centerPt*structure_inner_ratio;
        }

        for(int i=0;i<4;++i){
            Triangle tri;
            tri.setPoint(0,pyramid_corner[i+1],middle_height);
            tri.setPoint(1,pyramid_corner[i], middle_height);
            tri.setPoint(2,centerPt3);
            triangles.push_back(tri);
        }
        //outside mesh
        // .__.__.
        // \./_\./
        for(int i=0;i<4;++i){
            QList<QPointF> innerEdge;
            int innerDivision;
            if(edge[i].isEmpty()){
                edge[i].push_back((corner[i]+corner[i+1])/2);
            }

            innerDivision = edge[i].size();


            edge[i].push_front(corner[i]);
            edge[i].push_back(corner[i+1]);


            for(int j=0;j<=innerDivision;++j){
                qreal t=qreal(j)/innerDivision;
                innerEdge.push_back(pyramid_corner[i]*(1-t) + pyramid_corner[i+1]*t);
            }


            // Inverted triangle
            for(auto m=edge[i].begin(),n=innerEdge.begin();(m+1)!=edge[i].end();++m,++n){
                Triangle tri;
                tri.setPoint(0,*(m+1),0);
                tri.setPoint(1,*m,0);
                tri.setPoint(2,*n,middle_height);
               triangles.push_back(tri);
            }
            // triangle
            for(auto m=edge[i].begin()+1,n=innerEdge.begin();(n+1)!=innerEdge.end();++m,++n){
                Triangle tri;
                tri.setPoint(0,*m,0);
                tri.setPoint(1,*n,middle_height);
                tri.setPoint(2,*(n+1),middle_height);



                triangles.push_back(tri);
            }


        }


    }

}

int RenderWidget::getRandSeed()
{
    return rand_seed;
}



void RenderWidget::genBaseRockByPixel()
{
    memset(pixels,0,sizeof(pixels[0][0])*512*512);

    //Row1
    int w,h;
    int ok_x=0;
    bool notLastCell;
    do{

        genCellSize(w,h);
        notLastCell = true;
        if(ok_x+w>=width-1){
            w=width-1-ok_x;
            notLastCell = false;
        }


        qDebug()<<ok_x+w;
        drawPixelRect(ok_x,0,w,h);
        ok_x+=w;


    }while(notLastCell);
}

void RenderWidget::drawPixelRect(int x, int y, int w, int h)
{
    for(int i=x;i<=x+w;++i){
        pixels[i][y]=1;
        pixels[i][y+h]=1;
    }
    for(int i=y;i<=y+h;++i){
        pixels[x][i]=1;
        pixels[x+w][i]=1;
    }
}

qreal RenderWidget::getDisplaceAmount(qreal limitDis)
{

    if(limitDis<=displacementLimit){
        return 0;
    } else {
        limitDis/=3;
        limitDis-=displacementBase;


        if(limitDis>0)
            return 3;//(rand() % limitDis) + displacementBase;
        else
            return 0;
    }
}

void RenderWidget::showStep(int step)
{
    current_step = step;
    update();
}

void RenderWidget::debugPt(QPointF p)
{
    debugPtVector.push_back(p);
}

void RenderWidget::debugPt(Node *n)
{
    debugNodeVector.push_back(n);
}

void RenderWidget::debugPt(QPainter &painter, QPointF p, QColor color)
{
    painter.setBrush(color);
    painter.drawEllipse(p,2,2);
}

void RenderWidget::drawResult(int step)
{
    QPainter painter(step_images.at(step));


    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0,0,511,511);
    QBrush b(QColor(255,0,0,50));
    static QPen pen(QColor(128,128,255));
    switch(step){
    case 0:

        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);

        for(auto it=edges.begin();it!=edges.end();it++){

            Edge* ed = *it;
            QPainterPath path;

            path.moveTo(ed->n1->p);
            path.lineTo(ed->n2->p);
            painter.drawPath(path);
        }
        qDebug() << "COOL!";
        break;
    case 1:
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);

        for(auto it=edges.begin();it!=edges.end();it++){

            Edge* ed = *it;
            QPainterPath path;

            path.moveTo(ed->n1->pf);
            path.lineTo(ed->n2->pf);
            painter.drawPath(path);
        }
        break;
    case 2:
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);

        for(auto it=edges.begin();it!=edges.end();it++){

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
        }
        break;
    case 3: // Normal Map

        painter.setBrush(b);
        painter.setPen(Qt::NoPen);

        for(QVector<Triangle>::iterator it=triangles.begin();it!=triangles.end();it++){
            painter.setBrush(QBrush(QColor(it->getNormalMapColor())));
            painter.drawPolygon((*it).getPoly());
        }
        /*
        for(auto it=rocks.begin();it!=rocks.end();it++){
            painter.drawPolygon((*it)->poly());
        }
        */

        pen.setWidth(5);
        painter.setPen(pen);

        for(auto it=edges.begin();it!=edges.end();it++){

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
        }
        break;
    case 4:

        painter.setBrush(Qt::gray);
        painter.drawRect(0,0,511,511);

        //pen.setColor(QColor(120,120,120));
        pen.setWidth(5);
        painter.setPen(pen);
        return;

        for(auto it=edges.begin();it!=edges.end();it++){

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
        }
        break;

    }



    /*
    for(auto it=debugPtVector.begin();it!=debugPtVector.end();it++){
        debugPt(painter,*it);
    }
    for(auto it=debugNodeVector.begin();it!=debugNodeVector.end();it++){
        debugPt(painter,(*it)->p);
    }*/

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
    for(QVector<Node*>::iterator it=debugNodeVector.begin();it!=debugNodeVector.end();it++){
        delete (*it);
    }
    debugNodeVector.clear();
    for(QVector<Rock*>::iterator it=rocks.begin();it!=rocks.end();it++){
        delete (*it);
    }
    rocks.clear();

    triangles.clear();

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

