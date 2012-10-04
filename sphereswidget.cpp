#include "sphereswidget.h"
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <qmath.h>

SpheresWidget::SpheresWidget(QString const & inputFileName, QString const & outputTxtFileName,
                             QString const & outputBmpFileName, QString const & fullOutputTxtFileName,
                             QWidget *parent)
    : QGLWidget(parent), frameCount(0), outputTxtFileName(outputTxtFileName), outputBmpFileName(outputBmpFileName), fullOutputTxtFileName(fullOutputTxtFileName)
{
    QFile inputFile(inputFileName);
    inputFile.open(QFile::ReadOnly);
    QTextStream inputStream(&inputFile);
    while (!inputStream.atEnd()) {
        QString line = inputStream.readLine();
        QStringList koords = line.split(" ", QString::SkipEmptyParts);
        if (koords.size() != 4)
            continue;
        Sphere sphereToAdd;
        sphereToAdd.x = koords[0].toFloat();
        sphereToAdd.y = koords[1].toFloat();
        sphereToAdd.z = koords[2].toFloat();
        sphereToAdd.r = koords[3].toFloat();

        spheres.append(sphereToAdd);
    }
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
    /// TODO: ���� �������� - ������������ ����� ����� ����� � �������������
    /// TODO: ������� ����� ������ ����� ����� - �� ���

    /// time to save results and exit.
    if (frameCount == 11000) {
        float performance = 0;
        {
            QFile fullResult(fullOutputTxtFileName);
            fullResult.open(QFile::WriteOnly);
            QTextStream fullResultStream(&fullResult);
            float medianeMsecsPerFrame = 0;
            fullResultStream << "msecs per frame: ";
            foreach (float msecsValue, msecInFrameList) {
                medianeMsecsPerFrame += msecsValue;
                fullResultStream << msecsValue << " ";
            }
            fullResultStream << endl << "average msecs per frame: " << medianeMsecsPerFrame / 10;
            performance = spheres.size() / (1000 * medianeMsecsPerFrame);
            fullResultStream << endl << "Mspheres per second: " << performance;
        }
        {
            QFile result(outputTxtFileName);
            result.open(QFile::WriteOnly);
            QTextStream resultStream(&result);
            resultStream << performance;
        }

        QImage temp = grabFrameBuffer();
            temp.save(outputBmpFileName);
        close();
        ++frameCount;
        return;
    }
    QDateTime beginTime = QDateTime::currentDateTime();

    doPaint();

    /// cause the first frame has too law rendering,
    /// we save the time of frames from 1 to 10 numbers.
    if (frameCount >= 1 && frameCount <= 10)
        msecInFrameList.append(beginTime.msecsTo(QDateTime::currentDateTime()));
    ++frameCount;
}

void SpheresWidget::doPaint()
{
    glClear(GL_COLOR_BUFFER_BIT);
//    glRotatef(0.01, 1, 1, 1);
//    glPointSize(20);
//    glLineWidth(8);
//    glEnable(GL_POINT_SMOOTH);

//    glColor3f(0.9,0.5,0.5);

//    glBegin(GL_POINTS);
//    glVertex2f(-0.3,0.8);
//    glVertex2f(0.0,0.8);
//    glVertex2f(0.3,0.8);
//    glEnd();

//    glColor3f(0.7,1.0,0.7);

//    glBegin(GL_LINES);
//    glVertex2f(-0.3,0.6);
//    glVertex2f(0.3,0.6);
//    glVertex2f(-0.6,0.2);
//    glVertex2f(0.6,0.2);
//    glVertex2f(-0.6,-0.2);
//    glVertex2f(0.6,-0.2);
//    glEnd();

//    glDisable(GL_POINT_SMOOTH);
//    glColor3f(0.5,0.5,1.0);

//    glBegin(GL_POINTS);
//    glVertex2f(-0.3,0.4);
//    glVertex2f(0.0,0.4);
//    glVertex2f(0.3,0.4);
//    glEnd();

//    glPointSize(10);
//    glEnable(GL_POINT_SMOOTH);
//    glColor3f(0.5,0.5,0.5);

//    GLfloat x=-0.6;
//    glBegin(GL_POINTS);
//    for(int i=0;i<7;i++)
//    {
//     glVertex2f(x,0.0);
//     x+=0.2;
//    }
//    glEnd();

//    glLineWidth(2);
//    glColor3f(1.0,1.0,1.0);

//    glBegin(GL_LINES);
//    glVertex2f(-0.6,-0.4);
//    glVertex2f(0.6,-0.4);
//    glVertex2f(-0.6,-0.6);
//    glVertex2f(0.6,-0.6);
//    glVertex2f(-0.6,-0.8);
//    glVertex2f(0.6,-0.8);
//    glEnd();
//    glFinish();
    //DrawSphere(1,1,1,1);

#define X .525731112119133606
#define Z .850650808352039932

GLfloat vdata[12][3] = {
      {-X,0.0,Z},{X,0.0,Z},{-X,0.0,-Z},{X,0.0,-Z},
      {0.0,Z,X},{0.0,Z,-X},{0.0,-Z,X},{0.0,-Z,-X},
      {Z,X,0.0},{-Z,X,0.0},{Z,-X,0.0},{-Z,-X,0.0}
};

GLuint tindices[20][3] = {
      {1,4,0},{4,9,0},{4,5,9},{8,5,4},{1,8,4},
      {1,10,8},{10,3,8},{8,3,5},{3,2,5},{3,7,2},
      {3,10,7},{10,6,7},{6,11,7},{6,0,11},{6,1,0},
      {10,1,6},{11,0,9},{2,11,9},{5,2,9},{11,2,7}
};

int i;

glBegin(GL_TRIANGLES);
      for(i=0;i<20;i++)
{
      //����� ���������� ���������� � �����

      glVertex3fv(&vdata[tindices[i][0]][0]);
      glVertex3fv(&vdata[tindices[i][1]][0]);
      glVertex3fv(&vdata[tindices[i][2]][0]);
}
glEnd();


}
