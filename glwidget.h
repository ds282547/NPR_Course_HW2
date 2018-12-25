#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "GL/glut.h"
#include <QVector>
#include <QGridLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QDebug>
#include <QListWidget>
#include <QTimer>
#include <QOpenGLFunctions>

#include <QMessageBox>

#include <QOpenGLShaderProgram>

#include "glm/glm.h"
extern "C"
{
    #include "glm_helper.h"
}

class GLWidget : public QGLWidget,  protected  QOpenGLFunctions
{
    Q_OBJECT
public:

    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();


    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void load3DModel(const QString &filename);

    QOpenGLShaderProgram program;
    void prepareShader();
    void sendNormalMap(QImage* map);
    void sendTexture(QImage* map);

    void processModelData();
    GLuint vboName;

    struct MyVertex {
        float x, y, z;        // Vertex
        float nx, ny, nz;     // Normal
        float u, v;         // Texcoord0
        float tanx,tany,tanz;
        float bitx,bity,bitz;
    };
    MyVertex *vertices;
    unsigned vertices_count;
    unsigned coords_count;

    GLMmodel *model;



    void camera_light_ball_move();
    void draw_light_bulb();


    QTimer clk;
    QImage img;




    float mousex,mousey;
    bool mleft;
    float eyex;
    float eyey;
    float eyez;

    float eyet;//theta in degree
    float eyep;//phi in degree

    GLfloat light_pos[3];
    GLfloat ball_pos[3];
    GLfloat ball_rot[3];

    bool forward;
    bool backward;
    bool left;
    bool right;
    bool up;
    bool down;
    bool lforward;
    bool lbackward;
    bool lleft;
    bool lright;
    bool lup;
    bool ldown;
    bool bforward;
    bool bbackward;
    bool bleft;
    bool bright;
    bool bup;
    bool bdown;
    bool bx;
    bool by;
    bool bz;
    bool brx;
    bool bry;
    bool brz;

    const float rotation_speed = 0.3;

    GLuint textures[2];
    const int TEXTURE_HEIGHT = 512;
    const int TEXTURE_WIDTH = 512;
    const int TEXTURE_COUNT = 2;




signals:
public slots:
    void clearScreen();
protected:

protected:



};



#endif // GLWIDGET_H
