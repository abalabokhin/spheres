#ifndef SPHERESWIDGET_H
#define SPHERESWIDGET_H

#include <QGLWidget>
#include <QDateTime>

class SpheresWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    SpheresWidget(QWidget *parent = NULL, QString const & inFileName = "input.txt");

protected:
    void initializeGL();
    void resizeGL(int w, int nHeight) ;
    void paintGL();
private:
    int frameCount;
    int sphereCount;
    QList<float> msecInFrameList;

};

#endif // SPHERESWIDGET_H
