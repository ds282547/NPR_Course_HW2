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
    forward = false;
    backward = false;
    left = false;
    right = false;
    up = false;
    down = false;
    lforward = false;
    lbackward = false;
    lleft = false;
    lright = false;
    lup = false;
    ldown = false;
    bforward = false;
    bbackward = false;
    bleft = false;
    bright = false;
    bup = false;
    bdown = false;
    bx = false;
    by = false;
    bz = false;
    brx = false;
    bry = false;
    brz = false;

    light_pos[0] = 1.1;light_pos[1] = 1.0;light_pos[2] = 1.3;
    ball_pos[0]=ball_pos[1]=ball_pos[2]=0;
    ball_rot[0]=ball_rot[1]=ball_rot[2]=0;


    clk.start(10);
    QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(update()));
    vertices_count = 0;
    coords_count = 0;
    vboName = 0;
    vertices = NULL;

    model=NULL;
}

GLWidget::~GLWidget()
{
    if(vertices){
        free(vertices);
    }
}

void GLWidget::clearScreen()
{
    glClearColor(1,1,1,1);
}




void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    eyex = 0.0;
    eyey = 0.0;
    eyez = 5.6;

    eyet = 0.0;//theta in degree
    eyep = 90.0;//phi in degree


    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width()/ (GLfloat)height(), 0.001f, 100.0f);

    unsigned __int8 textureData[512][512][3];

    glGenTextures(2,textures);
    for(int i=0;i<TEXTURE_COUNT;i++){
        glBindTexture(GL_TEXTURE_2D,textures[i]);

        for(int y = 0; y < TEXTURE_HEIGHT; ++y)
            for(int x = 0; x < TEXTURE_WIDTH; ++x){
                textureData[y][x][0] = textureData[y][x][1] = 128;
                textureData[y][x][2] = 255;
            }

        // Create a texture
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)textureData);

        // Set up the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    glEnable(GL_TEXTURE_2D);

    prepareShader();

}

void GLWidget::paintGL()
{
    float P[16],V[16],M[16];
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glGetFloatv(GL_PROJECTION_MATRIX, P);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    gluLookAt(
        eyex,
        eyey,
        eyez,
        eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180),
        eyey + sin(eyet*M_PI / 180),
        eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
        0.0,
        1.0,
        0.0);

    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
    draw_light_bulb();
    program.bind();

    glGetFloatv(GL_MODELVIEW_MATRIX,V);

    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX,M);
    glPopMatrix();

    QMatrix4x4 mMatrix(M);
    QMatrix4x4 vMatrix(V);
    QMatrix4x4 pMatrix(P);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);
    glActiveTexture(0);

    program.setUniformValue("normalMap",0);
    program.setUniformValue("diffuseMap",1);
    program.setUniformValue("projection",pMatrix.transposed());

    program.setUniformValue("view",vMatrix.transposed());
    program.setUniformValue("model",mMatrix.transposed());
    program.setUniformValue("lightPos",light_pos[0],light_pos[1],light_pos[2]);
    program.setUniformValue("viewPos",eyex,eyey,eyez);





    if(coords_count>0){
        glDrawArrays(GL_TRIANGLES, 0,coords_count);
    }

    camera_light_ball_move();

}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)w/ (GLfloat)h, 0.001f, 100.0f);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float x = event->x();
    float y = event->y();
    if (mleft)
    {
        eyep -= (x - mousex)*0.1;
        eyet -= (y - mousey)*0.12;
        if (eyet > 89.9)
            eyet = 89.9;
        else if (eyet < -89.9)
            eyet = -89.9;
        if (eyep > 360)
            eyep -= 360;
        else if (eyep < 0)
            eyep += 360;
    }
    mousex = x;
    mousey = y;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        mousex = event->x();
        mousey = event->y();
        mleft = true;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        mleft = false;
    }
}

void GLWidget::camera_light_ball_move()
{
    GLfloat dx = 0, dy = 0, dz=0;
    static const float speed = 0.03;

    if(left|| right || forward || backward || up || down)
    {
        if (left)
            dx = -speed;
        else if (right)
            dx = speed;
        if (forward)
            dy = speed;
        else if (backward)
            dy = -speed;
        eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
        eyey += dy*sin(eyet*M_PI / 180);
        eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
        if (up)
            eyey += speed;
        else if (down)
            eyey -= speed;

    }
    if(lleft || lright || lforward || lbackward || lup || ldown)
        {
            dx = 0;
            dy = 0;
            if (lleft)
                dx = -speed;
            else if (lright)
                dx = speed;
            if (lforward)
                dy = speed;
            else if (lbackward)
                dy = -speed;
            light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
            light_pos[1] += dy*sin(eyet*M_PI / 180);
            light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
            if (lup)
                light_pos[1] += speed;
            else if(ldown)
                light_pos[1] -= speed;
        }
        if (bleft || bright || bforward || bbackward || bup || bdown)
        {
            dx = 0;
            dy = 0;
            if (bleft)
                dx = -speed;
            else if (bright)
                dx = speed;
            if (bforward)
                dy = speed;
            else if (bbackward)
                dy = -speed;
            ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
            ball_pos[1] += dy*sin(eyet*M_PI / 180);
            ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
            if (bup)
                ball_pos[1] += speed;
            else if (bdown)
                ball_pos[1] -= speed;
        }
        if(bx||by||bz || brx || bry || brz)
        {
            dx = 0;
            dy = 0;
            dz = 0;
            if (bx)
                dx = -rotation_speed;
            else if (brx)
                dx = rotation_speed;
            if (by)
                dy = rotation_speed;
            else if (bry)
                dy = -rotation_speed;
            if (bz)
                dz = rotation_speed;
            else if (brz)
                dz = -rotation_speed;
            ball_rot[0] += dx;
            ball_rot[1] += dy;
            ball_rot[2] += dz;
        }
}

void GLWidget::draw_light_bulb()
{

    GLUquadric *quad;
    static const GLfloat light0_disffuse[3] = { 1, 1, 1 };
    static const GLfloat light_rad = 0.05;//radius of the light bulb

    quad = gluNewQuadric();
    glPushMatrix();
    glColor3fv(light0_disffuse);
    glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
    gluSphere(quad, light_rad, 40, 20);
    glPopMatrix();

}

void GLWidget::keyReleaseEvent(QKeyEvent *event){
    if(event->key()>100){
        return;
    }
    switch(event->key()+32){
        case 'd':
        {
            right = false;
            break;
        }
        case 'a':
        {
            left = false;
            break;
        }
        case 'w':
        {
            forward = false;
            break;
        }
        case 's':
        {
            backward = false;
            break;
        }
        case 'q':
        {
            up = false;
            break;
        }
        case 'e':
        {
            down = false;
            break;
        }
        case 't':
        {
            lforward = false;
            break;
        }
        case 'g':
        {
            lbackward = false;
            break;
        }
        case 'h':
        {
            lright = false;
            break;
        }
        case 'f':
        {
            lleft = false;
            break;
        }
        case 'r':
        {
            lup = false;
            break;
        }
        case 'y':
        {
            ldown = false;
            break;
        }
        case 'i':
        {
            bforward = false;
            break;
        }
        case 'k':
        {
            bbackward = false;
            break;
        }
        case 'l':
        {
            bright = false;
            break;
        }
        case 'j':
        {
            bleft = false;
            break;
        }
        case 'u':
        {

            bup = false;
            break;
        }
        case 'o':
        {
            bdown = false;
            break;
        }
        case '7':
        {
            bx = false;
            break;
        }
        case '8':
        {
            by = false;
            break;
        }
        case '9':
        {
            bz = false;
            break;
        }
        case '4':
        {

            brx = false;
            break;
        }
        case '5':
        {
            bry = false;
            break;
        }
        case '6':
        {
            brz = false;
            break;
        }

    }
}

void GLWidget::load3DModel(const QString &filename)
{

    if(model){
        glmDelete(model);
    }
    model = glmReadOBJ(filename.toStdString().c_str());
    glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0, GL_FALSE);
    processModelData();
}


void GLWidget::prepareShader()
{

    program.addShaderFromSourceFile(QOpenGLShader::Vertex,":/v.vert");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment,":/f.frag");
    program.link();
    program.bind();
}

void GLWidget::sendNormalMap(QImage* map)
{

    unsigned __int8 textureData[512][512][3];
    for(int y = 0; y < TEXTURE_HEIGHT; ++y)
        for(int x = 0; x < TEXTURE_WIDTH; ++x){
            //qDebug() << map.pixelColor(x,y).red();
            textureData[y][x][0] = ( unsigned __int8)map->pixelColor(y,x).red();
            textureData[y][x][1] = ( unsigned __int8)map->pixelColor(y,x).green();
            textureData[y][x][2] = ( unsigned __int8)map->pixelColor(y,x).blue();



        }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)textureData);

}

void GLWidget::sendTexture(QImage *map)
{
    unsigned __int8 textureData[512][512][3];
    for(int y = 0; y < TEXTURE_HEIGHT; ++y)
        for(int x = 0; x < TEXTURE_WIDTH; ++x){
            //qDebug() << map.pixelColor(x,y).red();
            textureData[y][x][0] = ( unsigned __int8)map->pixelColor(y,x).red();
            textureData[y][x][1] = ( unsigned __int8)map->pixelColor(y,x).green();
            textureData[y][x][2] = ( unsigned __int8)map->pixelColor(y,x).blue();

        }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)textureData);
}


void GLWidget::processModelData()
{
    vertices_count = model->numtriangles * 3;
    coords_count = vertices_count * ( sizeof(MyVertex) / sizeof(float));
    if(!vertices_count){
        return;
    }
    if(vertices){
        free(vertices);
    }



    vertices = (MyVertex*)malloc(sizeof(MyVertex) * vertices_count);

    unsigned indx = 0;
    for (unsigned i = 0; i < model->numtriangles; ++i) {
        QVector3D pos[3];
        QVector2D uv[3];
        for (unsigned j = 0; j < 3; ++j) {
            pos[j] = QVector3D(model->vertices[model->triangles[i].vindices[j] * 3],model->vertices[model->triangles[i].vindices[j] * 3+1],model->vertices[model->triangles[i].vindices[j] * 3+2]);
            uv[j] = QVector2D( model->texcoords[model->triangles[i].tindices[j] * 2], model->texcoords[model->triangles[i].tindices[j] * 2+1]);

            memcpy(&vertices[indx].x,&model->vertices[model->triangles[i].vindices[j] * 3],sizeof(float)*3);
            memcpy(&vertices[indx].nx, &model->normals[model->triangles[i].nindices[j] * 3], sizeof(float) * 3);
            memcpy(&vertices[indx].u, &model->texcoords[model->triangles[i].tindices[j] * 2], sizeof(float) * 2);
            indx++;

        }


        QVector3D edge1 = pos[1]-pos[0],edge2 = pos[2]-pos[0];
        QVector2D deltaUV1 =uv[1]-uv[0],deltaUV2 = uv[2]-uv[0];
        float f = 1.0f/(deltaUV1.x()* deltaUV2.y() - deltaUV1.y()*deltaUV2.x());
        QVector3D tangent,bitangent;
        tangent.setX(f*(deltaUV2.y()*edge1.x() - deltaUV1.y()*edge2.x()));
        tangent.setY(f*(deltaUV2.y()*edge1.y() - deltaUV1.y()*edge2.y()));
        tangent.setZ(f*(deltaUV2.y()*edge1.z() - deltaUV1.y()*edge2.z()));


        bitangent.setX(f*(-deltaUV2.x()*edge1.x() + deltaUV1.x()*edge2.x()));
        bitangent.setY(f*(-deltaUV2.x()*edge1.y() + deltaUV1.x()*edge2.y()));
        bitangent.setZ(f*(-deltaUV2.x()*edge1.z() + deltaUV1.x()*edge2.z()));


        for (unsigned j = 0; j < 3; ++j) {
            vertices[indx-j].tanx =  tangent.x();
            vertices[indx-j].tany =  tangent.y();
            vertices[indx-j].tanz =  tangent.z();
            vertices[indx-j].bitx =  bitangent.x();
            vertices[indx-j].bity =  bitangent.y();
            vertices[indx-j].bitz =  bitangent.z();
        }
    }

    #define BUFFER_OFFSET(i) ((void*)(i))

    if(!vboName){
        glGenBuffers(1, &vboName);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboName	);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coords_count, &vertices[0].x, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,sizeof(MyVertex),BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(24));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(32));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(44));

    qDebug() << "OK!";
}
void GLWidget::keyPressEvent(QKeyEvent *event){
    if(event->key()>100){
        return;
    }

    switch(event->key()+32){
        case 'd':
        {
            right = true;
            break;
        }
        case 'a':
        {
            left = true;
            break;
        }
        case 'w':
        {
            forward = true;
            break;
        }
        case 's':
        {
            backward = true;
            break;
        }
        case 'q':
        {
            up = true;
            break;
        }
        case 'e':
        {
            down = true;
            break;
        }
        case 't':
        {
            lforward = true;
            break;
        }
        case 'g':
        {
            lbackward = true;
            break;
        }
        case 'h':
        {
            lright = true;
            break;
        }
        case 'f':
        {
            lleft = true;
            break;
        }
        case 'r':
        {
            lup = true;
            break;
        }
        case 'y':
        {
            ldown = true;
            break;
        }
        case 'i':
        {
            bforward = true;
            break;
        }
        case 'k':
        {
            bbackward = true;
            break;
        }
        case 'l':
        {
            bright = true;
            break;
        }
        case 'j':
        {
            bleft = true;
            break;
        }
        case 'u':
        {

            bup = true;
            break;
        }
        case 'o':
        {
            bdown = true;
            break;
        }
        case '7':
        {
            bx = true;
            break;
        }
        case '8':
        {
            by = true;
            break;
        }
        case '9':
        {
            bz = true;
            break;
        }
        case '4':
        {

            brx = true;
            break;
        }
        case '5':
        {
            bry = true;
            break;
        }
        case '6':
        {
            brz = true;
            break;
        }

    }
}

