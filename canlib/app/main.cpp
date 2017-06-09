#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
//#include <QLibrary>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load the embedded font.
    if (QFontDatabase::addApplicationFont(":/new/prefix1/DejaVuSans.ttf") != -1)
    {
        QFont font("DejaVuSans");
        font.setPixelSize(11);
        a.setFont(font);
    }

//    QLibrary library("/home/user/rr/proj/canlib/canlib_build/libcanlib.so.1.0.0");
//    if (!library.load())
//     qDebug() << library.errorString();
//     if (library.load())
//     qDebug() << "library loaded";

    MainWindow w;
    w.show();

    return a.exec();
}
