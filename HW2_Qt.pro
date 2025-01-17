#-------------------------------------------------
#
# Project created by QtCreator 2018-10-17T05:38:41
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HW1_Qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    glwidget.cpp \
    renderwidget.cpp \
    glm/glm.c \
    glm/glm_util.c \
    glm/glmimg.c \
    glm/glmimg_devil.c \
    glm/glmimg_jpg.c \
    glm/glmimg_png.c \
    glm/glmimg_sdl.c \
    glm/glmimg_sim.c \
    glm_helper.c

HEADERS += \
        mainwindow.h \
    glwidget.h \
    renderwidget.h \
    glm/glm.h \
    glm/glmint.h \
    glm_helper.h

FORMS += \
        mainwindow.ui


LIBS += "$$PWD\GL\glut32.lib"

RESOURCES += \
    shaders.qrc
