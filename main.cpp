#include "mainwindowwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowWidget w;
    w.show();
    return a.exec();
}
