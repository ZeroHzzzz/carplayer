#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <ctrl.h>
#include <imagehandler.h>
#include <mainwindow.h>
#include <speed.h>
#include <QFile>
#include <QMessageBox>
#include <QString>

typedef enum ReadType {
    dataType,
    imgType,
} ReadType;

class DataHandler {
   public:
    char sdBuff[512];
    int dataNum = 0;
    ReadType readType;

    bool readdata(QString);
};

extern DataHandler datahandler;
#endif