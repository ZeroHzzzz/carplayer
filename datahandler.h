#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <ctrl.h>
#include <imagehandler.h>
#include <mainwindow.h>
#include <speed.h>
#include <QFile>
#include <QMessageBox>
#include <QString>

#define SECTOR_START 4096  // 4096-255=3841
#define SECTOR_SIZE 512
#define SECTOR_NUM_IMGBUFF 67

typedef enum ReadType {
    dataType,
    imgType,
} ReadType;

class DataHandler {
   public:
    char sdBuff[512];
    QString data;
    int imgNum = 0;
    int imgCnt = 0;
    int dataNum = 0;
    int dataCnt;
    ReadType readType;
    uint32_t offset;
    QString nowImgName;

    bool data2Trex(QString, QString, std::string&);
    bool data2Img(QString, QString, std::string&);
    bool readdata();
    bool readNextData();
    bool readPreviousData();
    QString getImgName(QString, int8_t);
    bool readimg(QString);
};

extern DataHandler datahandler;
#endif