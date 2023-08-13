
#include "clientmainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientMainWindow w;
    w.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);

    QFile styleFile;
    styleFile.setFileName(":/style/style1.qss");
    styleFile.open(QFile::ReadOnly);
    QString qssStr = styleFile.readAll();
    a.setStyleSheet(qssStr);

    w.show();
    return a.exec();
}
