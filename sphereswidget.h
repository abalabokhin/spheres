#ifndef SPHERESWIDGET_H
#define SPHERESWIDGET_H

#include <QGLWidget>
#include <QDateTime>

struct Sphere {
    float x;
    float y;
    float z;
    float r;
};

class SpheresWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    SpheresWidget(QString const & inputFileName, QString const & outputTxtFileName,
                  QString const & outputBmpFileName, QString const & fullOutputTxtFileName,
                  QWidget *parent = NULL);

protected:
    void initializeGL();
    void resizeGL(int w, int nHeight) ;
    void paintGL();
private:
    void doPaint();

    int frameCount;
    //int sphereCount;
    QList<float> msecInFrameList;

    QString outputTxtFileName;
    QString outputBmpFileName;
    QString fullOutputTxtFileName;

    QList<Sphere> spheres;
};

#endif // SPHERESWIDGET_H
