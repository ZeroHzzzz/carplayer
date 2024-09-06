#include <datahandler.h>

extern ImageHandler imagehandler;
extern Controller controller;
extern SpeedHandler speedhandler;

bool DataHandler::readdata(QString filename) {
    QString data, data_temp;
    readType = dataType;
    QFile myFile(filename);

    if (!myFile.open(QIODevice::ReadWrite))  // 以读写方式打开
    {
        // TODO: error
        return false;
    } else {
        QDataStream dataInput(&myFile);

        while (!dataInput.atEnd()) {
            dataInput >> data_temp;
            data += data_temp;
            ++dataNum;
        }
    }

    uint32_t offset = data.indexOf("boom7", offset);

    if (data[offset].toLatin1() != 'b') {
        return false;
    }

    for (uint16_t i = 0; i < 512; ++i)
        sdBuff[i] = data[offset + i].toLatin1();

    // TODO 其他处理逻辑

    uint8_t i = 0;
    uint8_t* ptr = imagehandler.allmap[0];
    uint8_t* ptrEnd = &imagehandler.allmap[YY][XX] + 1;
    char* ptrImg = &sdBuff[159];

    while (ptr != ptrEnd) {
        if (*ptrImg & 0x01 << i++)
            *ptr++ = 0;
        else
            *ptr++ = 1;

        if (i == 8) {
            i = 0;
            ++ptrImg;
        }
    }

    return true;
};