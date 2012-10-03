#include "sphereswidget.h"
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <qmath.h>

SpheresWidget::SpheresWidget(QWidget *parent, QString const & inFileName)
    :
    QGLWidget(parent),
    frameCount(0), sphereCount(10)
{














}

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
    /// TODO: сделать более точный подсчет времени - а то среднее время кадра = 0;
    if (frameCount == 11) {
        QFile result("result.txt");
        result.open(QFile::WriteOnly);

        QTextStream resultStream(&result);
        float medianeMsecsPerFrame = 0;
        resultStream << "msecs per frame: ";
        foreach (float msecsValue, msecInFrameList) {
            medianeMsecsPerFrame += msecsValue;
            resultStream << msecsValue << " ";
        }
        resultStream << endl << "average msecs per frame: " << medianeMsecsPerFrame / 10;
        resultStream << endl << "Mspheres per second: " << sphereCount / medianeMsecsPerFrame;
        close();
    }
    QDateTime beginTime = QDateTime::currentDateTime();

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
    if (frameCount)
        msecInFrameList.append(beginTime.msecsTo(QDateTime::currentDateTime()));
    ++frameCount;
}
