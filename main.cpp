#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include "sphereswidget.h"

int main(int argc, char *argv[]) {

    QApplication app(argc,argv);

    QString inputFileName = "input.txt";
    QString outputTxtFileName = "output.txt";
    QString outputBmpFileName = "output.bmp";
    QString fullOutputTxtFileName = "fullOutput.txt";

    SpheresWidget window(inputFileName, outputTxtFileName, outputBmpFileName, fullOutputTxtFileName);
    window.resize(1024, 1024);

    window.show();
    QTimer * everytimeUpdater = new QTimer(&window);
    QObject::connect(everytimeUpdater, SIGNAL(timeout()),
                     &window,  SLOT(updateGL()));
    everytimeUpdater->start();
    return app.exec();
}
