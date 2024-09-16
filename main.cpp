#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    controller.controlParam_init();
    motorhandler.motorPID_init();
    speedhandler.speed_init();
    w.show();
    return a.exec();
}
