#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include "sphereswidget.h"

int main(int argc, char *argv[]) {

    QApplication app(argc,argv);
    SpheresWidget window;
    window.show();
    QTimer * everytimeUpdater = new QTimer(&window);
    bool temp = QObject::connect(everytimeUpdater, SIGNAL(timeout()),
                     &window,  SLOT(updateGL()));
    everytimeUpdater->start();
    return app.exec();
}
