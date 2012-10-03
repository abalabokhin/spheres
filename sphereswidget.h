#ifndef SPHERESWIDGET_H
#define SPHERESWIDGET_H

#include <QGLWidget>
#include <QDateTime>

class SpheresWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    SpheresWidget(QWidget *parent = NULL);

protected:
    void initializeGL();
    void resizeGL(int w, int nHeight) ;
    void paintGL();
private:
    QDateTime beginTime;
    int frameCount;
    int frameToStartCalculatingFps;
    int frameToStopCalculatingFps;
    float fps;
};

#endif // SPHERESWIDGET_H
