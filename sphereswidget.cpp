#include "sphereswidget.h"
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <qmath.h>

SpheresWidget::SpheresWidget(QWidget *parent) : QGLWidget(parent),
    frameCount(0), frameToStartCalculatingFps(20), frameToStopCalculatingFps(1000), fps(0)
{}

void SpheresWidget::initializeGL() {
   qglClearColor(Qt::black);
}

void SpheresWidget::resizeGL(int nWidth, int nHeight) {
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SpheresWidget::paintGL() {
    if (frameCount > frameToStartCalculatingFps && !beginTime.isValid()) {
        beginTime = QDateTime::currentDateTime();
    }
    if (frameCount > frameToStopCalculatingFps && fps == 0) {
        fps = 1000 * (float)(frameToStopCalculatingFps - frameToStartCalculatingFps) / beginTime.msecsTo(QDateTime::currentDateTime());
        QFile result("result.txt");
        result.open(QFile::WriteOnly);

        QTextStream resultStream(&result);
        resultStream << fps;
    }


    ++frameCount;

    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(20);
    glLineWidth(8);
    glEnable(GL_POINT_SMOOTH);

    glColor3f(0.9,0.5,0.5);

    glBegin(GL_POINTS);
    glVertex2f(-0.3,0.8);
    glVertex2f(0.0,0.8);
    glVertex2f(0.3,0.8);
    glEnd();

    glColor3f(0.7,1.0,0.7);

    glBegin(GL_LINES);
    glVertex2f(-0.3,0.6);
    glVertex2f(0.3,0.6);
    glVertex2f(-0.6,0.2);
    glVertex2f(0.6,0.2);
    glVertex2f(-0.6,-0.2);
    glVertex2f(0.6,-0.2);
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glColor3f(0.5,0.5,1.0);

    glBegin(GL_POINTS);
    glVertex2f(-0.3,0.4);
    glVertex2f(0.0,0.4);
    glVertex2f(0.3,0.4);
    glEnd();

    glPointSize(10);
    glEnable(GL_POINT_SMOOTH);
    glColor3f(0.5,0.5,0.5);

    GLfloat x=-0.6;
    glBegin(GL_POINTS);
    for(int i=0;i<7;i++)
    {
     glVertex2f(x,0.0);
     x+=0.2;
    }
    glEnd();

    glLineWidth(2);
    glColor3f(1.0,1.0,1.0);

    glBegin(GL_LINES);
    glVertex2f(-0.6,-0.4);
    glVertex2f(0.6,-0.4);
    glVertex2f(-0.6,-0.6);
    glVertex2f(0.6,-0.6);
    glVertex2f(-0.6,-0.8);
    glVertex2f(0.6,-0.8);
    glEnd();
}
