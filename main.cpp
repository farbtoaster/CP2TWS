#include "mainwindow.h"
#include "ueberdialog.h"
#include "worker.h"
#include <QApplication>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
