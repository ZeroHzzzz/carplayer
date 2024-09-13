#include <datahandler.h>
#include <maphandler.h>
#include <string>
DataHandler datahandler;

bool DataHandler::readdata() {
    offset = data.indexOf("boom7", offset);

    if (data[offset].toLatin1() != 'b') {
        return false;
    }

    for (uint16_t i = 0; i < 512; ++i)
        sdBuff[i] = data[offset + i].toLatin1();

    int16_t pitch = (sdBuff[5] << 8) | ((uint8_t)sdBuff[6]);
    imagehandler.angle.Pitch = pitch / 100;

    int16_t yaw = (sdBuff[7] << 8) | ((uint8_t)sdBuff[8]);
    imagehandler.angle.Yaw = yaw / 100;

    int16_t speedMin = (sdBuff[13] << 8) | ((uint8_t)sdBuff[14]);
    SI.speedMin = speedMin;

    SI.aimSpeed = (sdBuff[9] << 8) | ((uint8_t)sdBuff[10]);
    SI.aimSpeedL = (sdBuff[21] << 8) | ((uint8_t)sdBuff[22]);
    SI.aimSpeedR = (sdBuff[23] << 8) | ((uint8_t)sdBuff[24]);

    motorhandler.servo_turn = (sdBuff[25] << 8) | ((uint8_t)sdBuff[26]);

    SI.motorPWML = (sdBuff[11] << 8) | ((uint8_t)sdBuff[12]);
    SI.motorPWMR = (sdBuff[13] << 8) | ((uint8_t)sdBuff[14]);

    SI.varL[0] = (sdBuff[17] << 8) | ((uint8_t)sdBuff[18]);
    SI.varR[0] = (sdBuff[19] << 8) | ((uint8_t)sdBuff[20]);
    controller.angle_protect_flag = (uint8_t)sdBuff[90];
    controller.force_brake_flag = (uint8_t)sdBuff[91];

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

/* 读取后一帧数据 */
bool DataHandler::readNextData() {
    offset += 512;
    dataCnt += 1;
    if (dataCnt > (int)dataNum) {
        offset = 0;
        dataCnt = 0;
        return false;
    }

    if (readdata()) {
        maphandler.imgSize = _47_60;
        imagehandler.standard();
        imagehandler.getlinemap();
    }
    return true;
}

/* 读取前一帧数据 */
bool DataHandler::readPreviousData() {
    offset -= 512;
    dataCnt -= 1;
    if (dataCnt < 1) {
        offset = 0;
        dataCnt = 0;
    }
    if (readdata()) {
        maphandler.imgSize = _47_60;
        imagehandler.standard();
        imagehandler.getlinemap();
    }
}

QString DataHandler::getImgName(QString imgName, int8_t dir) {
    QString newImgName;
    QString str;
    QString imgPrefix;
    QString imgSuffix = ".bmp";

    imgName = imgName.simplified();  // 头尾去空格
    QStringList sections =
        imgName.split(QRegExp("[/]"));  // 用正则表达式 分离(去掉)/
    QString realImgName = sections.last();

    for (uint8_t i = 0; i < realImgName.size(); ++i) {
        if (realImgName.at(i) >= '0' && realImgName.at(i) <= '9') {
            str += realImgName.at(i);
        }
    }
    // 获取 除了数字和.bmp的前几位字符内容
    imgPrefix = imgName.mid(0, imgName.indexOf(str + imgSuffix));

    /* 重新命名图像文件名 */
    uint16_t imgNum = str.toUInt();  // string--->unsig int
    if (dir == 1) {
        imgNum++;
    } else if (dir == -1) {
        imgNum--;
    }
    newImgName =
        imgPrefix + QString::number(imgNum) + imgSuffix;  // 图片文件命名格式

    if (QFileInfo(newImgName).isFile()) {
        return newImgName;
    } else {
        imgNum = 1;
        newImgName = imgPrefix + QString::number(imgNum) + imgSuffix;
        return newImgName;
    }
}

bool DataHandler::readimg(QString filename) {
    if (QFileInfo(filename).isFile()) {
        maphandler.imgMat =
            cv::imread(filename.toStdString(), cv::IMREAD_GRAYSCALE);
        if (maphandler.imgMat.cols == XM && maphandler.imgMat.rows == YM) {
            maphandler.imgSize = _47_60;
        } else if (maphandler.imgMat.cols == COL &&
                   maphandler.imgMat.rows == ROW) {
            maphandler.imgSize = _188_180;
        } else {
            return false;
        }
        return true;
    }
    return false;
}

bool DataHandler::data2Trex(QString readPath,
                            QString savePath,
                            std::string& errormsg) {
    int fd = open(readPath.toStdString().c_str(), O_RDONLY | O_BINARY);
    if (fd == -1) {
        errormsg = "没有权限访问设备";
        return false;
    }

    lseek64(fd, SECTOR_START * SECTOR_SIZE, SEEK_SET);
    QDateTime dateTime = QDateTime::currentDateTime();
    QString fileName = savePath + dateTime.toString("MM_dd_HH_mm");
    fileName += ".T-REX";
    QFile file(fileName);

    if (file.exists()) {
        errormsg = "文件已存在";
        return false;
    } else {
        file.open(QIODevice::ReadWrite);
        bool success = false;
        do {
            if (fd != -1) {
                QDataStream dataOutput(&file);
                char _sdBuff[512];
                QString _str;
                read(fd, _sdBuff, 512);

                if (!(_sdBuff[0] == 'b' && _sdBuff[1] == 'o' &&
                      _sdBuff[2] == 'o' && _sdBuff[3] == 'm' &&
                      _sdBuff[4] == '7')) {
                    errormsg = "文件格式不正确";
                    return false;
                }
                for (uint16_t i = 0; i < 512; ++i) {
                    _str += _sdBuff[i];
                }
                dataOutput << _str;
                success = true;
            } else {
                errormsg = "没有权限访问设备";
                return false;
            }
        } while (0);  // 只执行一次即可，不需要循环

        if (success) {
            errormsg = "操作成功";
        }
    }
    file.close();
    _close(fd);
    return true;
}

bool DataHandler::data2Img(QString readPath,
                           QString savePath,
                           std::string& errormsg) {
    int fd = open(readPath.toStdString().c_str(), O_RDONLY | O_BINARY);
    if (fd == -1) {
        errormsg = "没有权限访问设备";
        return false;
    }

    lseek64(fd, SECTOR_START * SECTOR_SIZE, SEEK_SET);
    do {
        if (fd != -1) {
            char imgGray_SDBuff[SECTOR_SIZE * SECTOR_NUM_IMGBUFF];
            char imgGray_buff[ROW * COL];

            /* 图像数组--->图片 */
            read(fd, imgGray_SDBuff,
                 sizeof(imgGray_SDBuff));  // 图像读取 必须读512的整数倍
            memcpy(imgGray_buff, imgGray_SDBuff,
                   sizeof(imgGray_buff));  // 数组内容复制
            cv::Mat mat(ROW, COL, CV_8UC1,
                        imgGray_buff);  // 图像数组 转换成 8bite灰度图

            /* 文件命名 and 保存 */
            QString imgNameSave = savePath + QString::number(imgNum) + ".bmp";
            // 文件名
            cv::imwrite(imgNameSave.toStdString(),
                        mat);  // 将图像保存到指定文件

            ++imgNum;
            if (imgNum >= 2000)
                break;
        } else {
            errormsg = "maybe don't have permission";
            break;
        }
    } while (1);
}