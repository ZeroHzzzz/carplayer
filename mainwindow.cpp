#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "headfile.h"
#include "imagehandler.h"
#include "maphandler.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    map_installEventFilter();
    initDeviceCombobox();
    // initMisc();
    changePlayerStatus(false);

    playbackTimer = new QTimer(this);
    connect(playbackTimer, &QTimer::timeout, this,
            &MainWindow::onPlaybackTimeout);
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::changePlayerStatus(bool status) {
    // true 启用
    ui->slider->setEnabled(status);
    ui->backward->setEnabled(status);
    ui->pause->setEnabled(status);
    ui->forward->setEnabled(status);
}

void MainWindow::initDeviceCombobox() {
    QFileInfoList drives = QDir::drives();
    for (const QFileInfo& drive : drives) {
        ui->comboBox_device->addItem(drive.absoluteFilePath());
    }
}

void MainWindow::initProgressBar(int maxn) {
    ui->slider->setRange(1, maxn);
}
void MainWindow::initMisc() {
    ui->readType_combobox->adjustSize();
    ui->ReadSDData->adjustSize();
    ui->ReadData->adjustSize();
}

void MainWindow::map_installEventFilter() {
    // ui->imgGray_label->installEventFilter(this);
    ui->AllMap->installEventFilter(this);
    ui->BaseMap->installEventFilter(this);
    ui->LineMap->installEventFilter(this);
    ui->LeftMap->installEventFilter(this);
    ui->RightMap->installEventFilter(this);
    ui->DeleteMap->installEventFilter(this);
    ui->InsideMap->installEventFilter(this);
}

void MainWindow::showLCDNumber(int index) {
    ui->lcdNumber->display(index);
}

void MainWindow::changeSliderIndex(int index) {
    ui->slider->setValue(index);
}
void MainWindow::showAllMaps(QPoint point) {
    if (maphandler.imgSize == _188_180) {
        maphandler.showImgGray(imagehandler.imgGray, ui->imgGray_label, point);
    }

    maphandler.showMap(imagehandler.allmap, ui->AllMap, point);
    maphandler.showMap(imagehandler.basemap, ui->BaseMap, point);
    maphandler.showMap(imagehandler.linemap, ui->LineMap, point);
    maphandler.showMap(imagehandler.leftmap, ui->LeftMap, point);
    maphandler.showMap(imagehandler.rightmap, ui->RightMap, point);
    maphandler.showMap(imagehandler.insidemap, ui->InsideMap, point);
    maphandler.showMap(imagehandler.deletemap, ui->DeleteMap, point);
    maphandler.showRealMap(ui->RealMap, point);
}

void MainWindow::mapEvent(uint8_t map[YM][XM],
                          QLabel* label,
                          uint8_t multiple) {
    QPoint relativePoint =
        label->mapFromGlobal(QPoint(cursor().pos().x(), cursor().pos().y()));
    int x = relativePoint.x() / multiple;
    int y = YY - relativePoint.y() / multiple;

    QPoint mapPoint;
    mapPoint.rx() = relativePoint.x() / multiple;
    mapPoint.ry() = relativePoint.y() / multiple;
    showAllMaps(mapPoint);

    QPoint showPoint = adjustPixelInfo(relativePoint, YM, XM, multiple);

    if (ui->pixelInfo_label->isHidden())
        ui->pixelInfo_label->show();

    ui->pixelInfo_label->setText(
        QString("[%1, %2] : %3").arg(y).arg(x).arg(map[y][x]));
    ui->pixelInfo_label->setStyleSheet("background-color:rgb(255,218,185)");
    ui->pixelInfo_label->move(showPoint.x(), showPoint.y());
    ui->pixelInfo_label->adjustSize();
}

bool MainWindow::eventFilter(QObject* object, QEvent* event) {
    // if (object == ui->imgGray_label) {
    //     if (event->type() == QEvent::MouseButtonPress) {
    //         imgGrayEvent(imgGray, ui->imgGray_label, 2);
    //         return true;
    //     }
    // }
    if (object == ui->AllMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.allmap, ui->AllMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->BaseMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.basemap, ui->BaseMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->LineMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.linemap, ui->LineMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->LeftMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.leftmap, ui->LeftMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->RightMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.rightmap, ui->RightMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->DeleteMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.deletemap, ui->DeleteMap, MULTIPLE);
            return true;
        }
    } else if (object == ui->InsideMap) {
        if (event->type() == QEvent::MouseButtonPress) {
            mapEvent(imagehandler.insidemap, ui->InsideMap, MULTIPLE);
            return true;
        }
    }
    return false;
}

QPoint MainWindow::adjustPixelInfo(QPoint relativePoint,
                                   uint8_t mapRow,
                                   uint8_t mapCol,
                                   uint8_t multiple) {
    QPoint showPoint;
    int titleBarHeight = frameGeometry().height() - geometry().height();
    showPoint.rx() = cursor().pos().x() - this->pos().x();
    showPoint.ry() = cursor().pos().y() - this->pos().y() - titleBarHeight;
    QPoint margin;
    margin.rx() = 10;
    margin.ry() = 10;

    /* 四种情况  <:+  >:- */
    if (relativePoint.x() <
            mapCol * multiple - ui->pixelInfo_label->width() - margin.x() &&
        relativePoint.y() <
            mapRow * multiple - ui->pixelInfo_label->height() - margin.y()) {
        showPoint += margin;
    } else if (relativePoint.x() < mapCol * multiple -
                                       ui->pixelInfo_label->width() -
                                       margin.x() &&
               relativePoint.y() > mapRow * multiple -
                                       ui->pixelInfo_label->height() -
                                       margin.y()) {
        showPoint.ry() -= ui->pixelInfo_label->height();
        showPoint.rx() += margin.x();
        showPoint.ry() -= margin.y();
    } else if (relativePoint.x() > mapCol * multiple -
                                       ui->pixelInfo_label->width() -
                                       margin.x() &&
               relativePoint.y() < mapRow * multiple -
                                       ui->pixelInfo_label->height() -
                                       margin.y()) {
        showPoint.rx() -= ui->pixelInfo_label->width();
        showPoint.rx() -= margin.x();
        showPoint.ry() += margin.y();
    } else if (relativePoint.x() > mapCol * multiple -
                                       ui->pixelInfo_label->width() -
                                       margin.x() &&
               relativePoint.y() > mapRow * multiple -
                                       ui->pixelInfo_label->height() -
                                       margin.y()) {
        showPoint.rx() -= ui->pixelInfo_label->width();
        showPoint.ry() -= ui->pixelInfo_label->height();
        showPoint -= margin;
    }

    return showPoint;
}

void MainWindow::onPlaybackTimeout() {
    // 根据当前读取类型，调用读取下一帧数据或图像
    if (datahandler.readNextData()) {
        showAllMaps();
        showLCDNumber(datahandler.dataCnt);
        changeSliderIndex(datahandler.dataCnt);
    } else {
        playbackTimer->stop();
        showAllMaps();
        showLCDNumber(datahandler.dataCnt);
        changeSliderIndex(datahandler.dataCnt);
    }
    // letGo
}

QString MainWindow::getReadDevice() const {
    return ui->comboBox_device->currentText();
}

QString MainWindow::getReadPathText() const {
    return ui->textEdit_readpath->toPlainText();
}

QString MainWindow::getReadTypeText() const {
    return ui->readType_combobox->currentText();
}

void MainWindow::on_pushButton_browserreadpath_clicked() {
    QString dirPath;
    if (datahandler.readType == dataType) {
        dirPath = QFileDialog::getOpenFileName(
            this, "选择文件", "", "T-REX Files (*.T-REX);;All Files (*.*)");
    } else if (datahandler.readType == imgType) {
        dirPath = QFileDialog::getExistingDirectory(this, "选择目录", "");
    }

    if (!dirPath.isEmpty()) {
        ui->textEdit_readpath->setText(dirPath);
    }
}

void MainWindow::on_textEdit_readpath_textChanged() {
    QString savePath = ui->textEdit_readpath->toPlainText();
    if (!QFileInfo::exists(savePath)) {
        QMessageBox::warning(this, "路径错误",
                             "读取路径无效，请选择有效的文件路径。");
    }
}

void MainWindow::on_ReadData_clicked() {
    QString filename = getReadPathText();
    if (!filename.isEmpty()) {
        if (datahandler.readType == dataType) {
            datahandler.data.clear();
            datahandler.dataNum = 0;
            datahandler.offset = 0;

            QFile myFile(filename);
            if (!myFile.open(QIODevice::ReadWrite)) {
                // TODO
            } else {
                QDataStream dataInput(&myFile);
                QString datatmp;
                while (!dataInput.atEnd()) {
                    dataInput >> datatmp;
                    datahandler.data += datatmp;
                    ++datahandler.dataNum;
                }
            }
            datahandler.offset =
                datahandler.data.indexOf("boom7", datahandler.offset);

            if (datahandler.readdata()) {
                datahandler.dataCnt = 0;
                maphandler.imgSize = _47_60;
                imagehandler.standard();
                imagehandler.getlinemap();
                controller.go(maphandler.imgSize);
                showVar();
                showAllMaps();
                changePlayerStatus(true);
                showLCDNumber(datahandler.dataCnt);
                initProgressBar(datahandler.dataNum);
            }
        } else if (datahandler.readType == imgType) {
            if (datahandler.readimg(filename)) {
                datahandler.imgCnt = 0;
                datahandler.nowImgName = filename;
                maphandler.Mat2Map(maphandler.imgMat);
                imagehandler.standard();
                controller.go(maphandler.imgSize);
                showVar();
                imagehandler.getlinemap();
                showAllMaps();
                changePlayerStatus(true);
                showLCDNumber(datahandler.imgCnt);
                initProgressBar(datahandler.imgNum);
            }
        }
    } else {
        QMessageBox::warning(this, "路径错误",
                             "读取路径无效，请选择有效的文件路径。");
    }
}

void MainWindow::on_backward_clicked() {
    // 加个终止条件
    datahandler.readPreviousData();
    // letGo
    showVar();
    controller.go(maphandler.imgSize);
    showAllMaps();
    showLCDNumber(datahandler.dataCnt);
    changeSliderIndex(datahandler.dataCnt);
}

void MainWindow::on_forward_clicked() {
    datahandler.readNextData();
    // letGo
    showVar();
    controller.go(maphandler.imgSize);
    showAllMaps();
    showLCDNumber(datahandler.dataCnt);
    changeSliderIndex(datahandler.dataCnt);
}

void MainWindow::on_pause_clicked() {
    if (!playbackTimer->isActive()) {
        playbackTimer->start(
            timerInterval);  // 开始播放，每隔 20ms 获取一帧数据
    } else {
        playbackTimer->stop();
    }
}

void MainWindow::on_slider_valueChanged(int value) {
    playbackTimer->stop();
    showLCDNumber(value);
    goToData(value);
}

void MainWindow::goToData(int value) {
    datahandler.offset = value * 512;
    datahandler.dataCnt = value;
    maphandler.imgSize = _47_60;
    datahandler.readdata();
    imagehandler.standard();
    imagehandler.getlinemap();
    controller.go(maphandler.imgSize);
    // letgo
    showVar();
    showAllMaps();
}

void MainWindow::showVar() {
    {
        ui->nowThreshold_label->setText(
            QString("nowThreshold: %1").arg(imagehandler.nowThreshold));
        ui->top_label->setText(QString("top: %1").arg(II.top));
        ui->bnum_label->setText(QString("bnum: %1").arg(II.bnum_all));
        ui->lnum_label->setText(QString("lnum: %1").arg(II.lnum_all));
        ui->rnum_label->setText(QString("rnum: %1").arg(II.rnum_all));
        ui->top_right_label->setText(
            QString("TR: %1 %2").arg(TR_Y(0)).arg(TR_X(0)));
        ui->right_top_label->setText(
            QString("RT: %1 %2").arg(RT_Y(0)).arg(RT_X(0)));
        ui->right_bottom_label->setText(
            QString("RB: %1 %2").arg(RB_Y(0)).arg(RB_X(0)));
        ui->bottom_right_label->setText(
            QString("BR: %1 %2").arg(BR_Y(0)).arg(BR_X(0)));
        ui->top_left_label->setText(
            QString("TL: %1 %2").arg(TL_Y(0)).arg(TL_X(0)));
        ui->left_top_label->setText(
            QString("LT: %1 %2").arg(LT_Y(0)).arg(LT_X(0)));
        ui->left_bottom_label->setText(
            QString("LB: %1 %2").arg(LB_Y(0)).arg(LB_X(0)));
        ui->bottom_left_label->setText(
            QString("BL: %1 %2").arg(BL_Y(0)).arg(BL_X(0)));
        ui->num_lm_label->setText(QString("num_lm: %1").arg(II.num_lm));
        ui->start_lm_label->setText(
            QString("start_lm: %1 %2").arg(II.start_lm[0]).arg(II.start_lm[1]));
        ui->start_rm_label->setText(
            QString("start_rm: %1 %2").arg(II.start_rm[0]).arg(II.start_rm[1]));
        ui->num_lm_label->setText(QString("num_lm: %1").arg(II.num_lm));
        ui->num_rm_label->setText(QString("num_rm: %1").arg(II.num_rm));
        ui->dnum_all_label->setText(QString("dnum: %1").arg(II.dnum_all));
        ui->speedTop_label->setText(QString("speedTop: %1").arg(II.speedTop));

        QString str;

        switch (IF.annulus) {
            case AL1:
                str = "AL1";
                break;
            case AL2:
                str = "AL2";
                break;
            case AL3:
                str = "AL3";
                break;
            case AL4:
                str = "AL4";
                break;
            case AL5:
                str = "AL5";
                break;
            case AR1:
                str = "AR1";
                break;
            case AR2:
                str = "AR2";
                break;
            case AR3:
                str = "AR3";
                break;
            case AR4:
                str = "AR4";
                break;
            case AR5:
                str = "AR5";
                break;
            default:
                str = "0";
                break;
        }
        if (IF.annulusDelay == 1)
            ui->Annulus_label->setText(QString("Annulus: Annulus_dalay"));
        else if (IF.annulusDelay == 0)
            ui->Annulus_label->setText(QString("Annulus: ") + str);

        ui->STEP_label->setText(QString("STEP: %1").arg(II.step));
        ui->Ramp_label->setText(QString("Ramp: %1").arg(IF.ramp));
        ui->cornerRight_label->setText(
            QString("cornerRight: %1 %2").arg(II.right_y).arg(II.right_x));
        ui->cornerLeft_label->setText(
            QString("cornerLeft: %1 %2").arg(II.left_y).arg(II.left_x));
        ui->kbmid_label->setText(QString("kbmid: %1 %2")
                                     .arg(controller.kbmid[0])
                                     .arg(controller.kbmid[1]));
        ui->kbleft_label->setText(QString("kbleft: %1 %2")
                                      .arg(controller.kbleft[0])
                                      .arg(controller.kbleft[1]));
        ui->kbright_label->setText(QString("kbright: %1 %2")
                                       .arg(controller.kbright[0])
                                       .arg(controller.kbright[1]));
        ui->leftnum_label->setText(QString("leftnum: %1").arg(II.leftnum));
        ui->midnum_label->setText(QString("midnum: %1").arg(II.midnum));
        ui->rightnum_label->setText(QString("rightnum: %1").arg(II.rightnum));

        switch (II.line_forbid) {
            case 0:
                str = "BOTHLINE";
                break;
            case 1:
                str = "LEFTLINE";
                break;
            case 2:
                str = "RIGHTLINE";
                break;
            case 3:
                str = "MIDLINE";
                break;
            case 4:
                str = "NONELINE";
                break;
            default:
                break;
        }
        ui->line_forbid_label->setText(QString("line_forbid: ") + str);

        switch (controller.controlLine) {
            case 0:
                str = "BOTHLINE";
                break;
            case 1:
                str = "LEFTLINE";
                break;
            case 2:
                str = "RIGHTLINE";
                break;
            case 3:
                str = "MIDLINE";
                break;
            case 4:
                str = "NONELINE";
                break;
            default:
                break;
        }
        ui->controlLine_label->setText(QString("contrilLine: ") + str);

        ui->nowDev_label->setText(QString("nowDev: %1").arg(LSMI.nowDeviation));
        ui->dirPWM_label->setText(QString("dirPWM: %1").arg(DIRP.dirPWM));
        ui->pidOut_label->setText(
            QString("TARYAW: %1").arg(DIRP.targetYawRate));
        ui->Pitch_label->setText(
            QString("pitch: %1").arg(imagehandler.angle.Pitch));
        ui->pitchRate_label->setText(
            QString("pitchRate:%1").arg(imagehandler.SystemAttitudeRate.Pitch));
        ui->Yaw_label->setText(QString("yaw: %1").arg(imagehandler.angle.Yaw));
        ui->yawRate_label->setText(
            QString("yawRate: %1").arg(imagehandler.SystemAttitudeRate.Yaw));
        ui->d_yawRate_label->setText(
            QString("d_yawRate: %1")
                .arg(imagehandler.SystemAttitudeRate.Yaw -
                     controller.Last_YawRate));
        ui->act_dYRate_label->setText(
            QString("act_dYRate: %1").arg(controller.act_dYRate));
        ui->speedMin_label->setText(QString("speedMin: %1").arg(SI.speedMin));
        ui->force_brake_label->setText(
            QString("force_brake: %1").arg(controller.force_brake_flag));
        ui->parking_label->setText(QString("parking: %1").arg(IF.parking));
        ui->nowSpeedM_label->setText(
            QString("nowSpeedM: %1").arg(controller.act_nowSpeedM));
        ui->nowSpeedL_label->setText(QString("varL[0]: %1").arg(SI.varL[0]));
        ui->nowSpeedR_label->setText(QString("varR[0]: %1").arg(SI.varR[0]));
        ui->MPID_oncei_label->setText(
            QString("MPID_oncei: %1").arg(controller.MPID_oncei));
        ui->MPID_ki_label->setText(
            QString("MPID_ki: %1").arg(controller.MPID_ki));
        ui->MPID_AC_P_label->setText(
            QString("MPID.AC_P: %1").arg(controller.MPID_AC_P));
        ui->MPID_UD_label->setText(
            QString("MPID_UD: %1").arg(controller.MPID_UD));
        ui->aimSpeed_label->setText(QString("aimSpeed: %1").arg(SI.aimSpeed));
        ui->running_brake_flag_label->setText(
            QString("running_brake: %1").arg(controller.running_brake_flag));
        ui->act_dirPWM_label->setText(
            QString("act_dirPWM: %1").arg(controller.act_dirPWM));
        ui->act_curvature_label->setText(
            QString("act_curvature: %1").arg(controller.act_curvature));
        ui->act_TarYawrate_label->setText(
            QString("act_TarYawrate: %1").arg(controller.act_TarYawrate));
        ui->LSMI_DK_label->setText(QString("LSMI.DK: %1").arg(LSMI.DK));
        ui->annuYaw_label->setText(
            QString("annuYaw: %1").arg(controller.annulus_yaw));
        ui->speedPWM_label->setText(QString("speedPWM: %1").arg(DIRP.speedPWM));
        ui->motorPWML_label->setText(
            QString("motorPWML: %1").arg(SI.motorPWML));
        ui->motorPWMR_label->setText(
            QString("motorPWMR: %1").arg(SI.motorPWMR));
        ui->actual_IF_label->setText(
            QString("actual_annulus: %1").arg(imagehandler.IIF.annulus));
        ui->actual_ramp_label->setText(
            QString("actual_ramp: %1").arg(imagehandler.IIF.ramp));
        ui->actual_park_label->setText(
            QString("actual_park: %1").arg(imagehandler.IIF.parking));
        ui->Yaw_Var_label->setText(
            QString("yaw_var: %1").arg(controller.Yaw_Rate_variance));
        ui->angle_protect_label->setText(
            QString("angle_protect: %1").arg(controller.angle_protect_flag));

        /* my_information */
        ui->servo_turn_label->setText(
            QString("servo_turn: %1").arg(motorhandler.servo_turn));
        ui->aimSpeedL_label->setText(
            QString("aimSpeedL: %1").arg(SI.aimSpeedL));
        ui->aimSpeedR_label->setText(
            QString("aimSpeedR: %1").arg(SI.aimSpeedR));
        ui->aveDevLeft_label->setText(
            QString("aveDev_Left:%1").arg(LSMI.aveDeviationLeft));
        ui->aveDevRight_label->setText(
            QString("aveDev_Right:%1").arg(LSMI.aveDeviationRight));
        ui->aveDev_label->setText(QString("aveDev:%1").arg(LSMI.aveDeviation));

        switch (controller.act_speedType) {
            case 0:
                ui->act_speedType_label->setText(QString("act_SpeedType: N"));
                break;
            case 1:
                ui->act_speedType_label->setText(QString("act_SpeedType: F"));
                break;
            case 2:
                ui->act_speedType_label->setText(QString("act_SpeedType: H"));
                break;
            case 3:
                ui->act_speedType_label->setText(QString("act_SpeedType: B"));
                break;
            case 4:
                ui->act_speedType_label->setText(QString("act_SpeedType: E"));
                break;
            default:
                break;
        }

        switch (controller.act_brakeType) {
            case 0:
                ui->act_brakeType_label->setText(QString("act_brakeType: SB"));
                break;
            case 1:
                ui->act_brakeType_label->setText(QString("act_brakeType: SSB"));
                break;
            case 2:
                ui->act_brakeType_label->setText(QString("act_brakeType: EEB"));
                break;
            default:
                break;
        }
    }

    /* testBox test变量查看 */
    // {
    //     ui->testBox_textBrowser->document()->clear();

    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test0));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test1));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test2));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test3));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test4));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test5));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test6));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test7));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test8));
    //     ui->testBox_textBrowser->insertPlainText(
    //         QString("%1\n").arg(testBox.test9));
    // }

    {
        ui->nowThreshold_label->adjustSize();
        ui->top_label->adjustSize();
        ui->bnum_label->adjustSize();
        ui->lnum_label->adjustSize();
        ui->rnum_label->adjustSize();
        ui->top_right_label->adjustSize();
        ui->right_top_label->adjustSize();
        ui->right_bottom_label->adjustSize();
        ui->bottom_right_label->adjustSize();
        ui->top_left_label->adjustSize();
        ui->left_top_label->adjustSize();
        ui->left_bottom_label->adjustSize();
        ui->bottom_left_label->adjustSize();
        ui->start_lm_label->adjustSize();
        ui->start_rm_label->adjustSize();
        ui->num_lm_label->adjustSize();
        ui->num_rm_label->adjustSize();
        ui->dnum_all_label->adjustSize();
        ui->speedTop_label->adjustSize();
        ui->Annulus_label->adjustSize();
        ui->Ramp_label->adjustSize();
        ui->STEP_label->adjustSize();
        ui->cornerRight_label->adjustSize();
        ui->cornerLeft_label->adjustSize();
        ui->kbmid_label->adjustSize();
        ui->kbleft_label->adjustSize();
        ui->kbright_label->adjustSize();
        ui->leftnum_label->adjustSize();
        ui->midnum_label->adjustSize();
        ui->rightnum_label->adjustSize();
        ui->line_forbid_label->adjustSize();
        ui->controlLine_label->adjustSize();
        ui->nowDev_label->adjustSize();
        ui->dirPWM_label->adjustSize();
        ui->pidOut_label->adjustSize();
    }

    /* 大小适配 */
    {
        ui->ARI_aimSpeed_label->adjustSize();
        ui->ARI_aimSpeedL_label->adjustSize();
        ui->ARI_aimSpeedR_label->adjustSize();
        ui->ARI_nowSpeedL_label->adjustSize();
        ui->ARI_nowSpeedR_label->adjustSize();
        ui->ADC_MAX_label->adjustSize();
        ui->ADC0_label->adjustSize();
        ui->ADC1_label->adjustSize();
        ui->ADC2_label->adjustSize();
        ui->ADC3_label->adjustSize();
        ui->ADC4_label->adjustSize();
        ui->Yaw_label->adjustSize();
        ui->Pitch_label->adjustSize();
        ui->ARI_speedType_label->adjustSize();
        ui->ARI_brakeType_label->adjustSize();
        ui->TF_DISTENCE_label->adjustSize();
        ui->TF_STRENGTH_label->adjustSize();
        ui->pitchRate_label->adjustSize();
        ui->yawRate_label->adjustSize();
        ui->d_yawRate_label->adjustSize();
        ui->speedMin_label->adjustSize();
        ui->force_brake_label->adjustSize();
        ui->angle_protect_label->adjustSize();
        ui->nowSpeedR_label->adjustSize();
        ui->parking_label->adjustSize();
        ui->nowSpeedL_label->adjustSize();
        ui->nowSpeedM_label->adjustSize();
        ui->MPID_oncei_label->adjustSize();
        ui->MPID_ki_label->adjustSize();
        ui->MPID_UD_label->adjustSize();
        ui->MPID_AC_P_label->adjustSize();
        ui->aimSpeed_label->adjustSize();
        ui->act_speedType_label->adjustSize();
        ui->act_brakeType_label->adjustSize();
        ui->running_brake_flag_label->adjustSize();
        ui->act_TarYawrate_label->adjustSize();
        ui->act_dYRate_label->adjustSize();
        ui->act_curvature_label->adjustSize();
        ui->act_dirPWM_label->adjustSize();
        ui->Yaw_Var_label->adjustSize();
        ui->actual_IF_label->adjustSize();
        ui->actual_ramp_label->adjustSize();
        ui->speedPWM_label->adjustSize();
        ui->actual_park_label->adjustSize();
        ui->motorPWML_label->adjustSize();
        ui->motorPWMR_label->adjustSize();
        ui->annuYaw_label->adjustSize();
        ui->LSMI_DK_label->adjustSize();
        ui->ARI_nowThreshold_label->adjustSize();
        ui->ARI_nowDeviation_label->adjustSize();

        /* my_information */
        ui->servo_turn_label->adjustSize();
        ui->aimSpeedL_label->adjustSize();
        ui->aimSpeedR_label->adjustSize();

        ui->aveDevLeft_label->adjustSize();
        ui->aveDevRight_label->adjustSize();
        ui->aveDev_label->adjustSize();
    }
}
void MainWindow::on_readType_combobox_currentIndexChanged(int index) {
    ui->textEdit_readpath->blockSignals(true);
    ui->textEdit_readpath->clear();
    ui->textEdit_readpath->blockSignals(false);

    if (index == 0) {  // 索引
        datahandler.readType = dataType;
    } else {
        datahandler.readType = imgType;
    }
}

void MainWindow::on_ReadSDData_clicked() {
    QString savepath = QFileDialog::getExistingDirectory(this, "选择目录", "");
    if (savepath.isEmpty()) {
        QMessageBox::warning(this, "警告", "未选择任何目录，请选择一个目录！");
    }

    if (datahandler.readType == dataType) {
        QString devicePath = getReadDevice();
        if (devicePath.endsWith("/")) {
            devicePath.chop(1);  // 去掉末尾的斜杠，变成 "C:"
        }
        QString fullDevicePath = QStringLiteral("\\\\.\\") + devicePath;
        std::string errormsg = "";
        bool status = datahandler.data2Trex(fullDevicePath, savepath, errormsg);
        if (!status) {
            QMessageBox::warning(this, tr("错误"), tr(errormsg.c_str()));
            return;
        } else {
            QMessageBox::information(this, tr("提示"), tr(errormsg.c_str()));
            return;
        }
    } else if (datahandler.readType == imgType) {
        QString devicePath = getReadDevice();
        if (devicePath.endsWith("/")) {
            devicePath.chop(1);  // 去掉末尾的斜杠，变成 "C:"
        }
        QString fullDevicePath = QStringLiteral("\\\\.\\") + devicePath;
        std::string errormsg = "";
        bool status = datahandler.data2Img(fullDevicePath, savepath, errormsg);
        if (!status) {
            QMessageBox::warning(this, tr("错误"), tr(errormsg.c_str()));
            return;
        } else {
            QMessageBox::information(this, tr("提示"), tr(errormsg.c_str()));
            return;
        }
    }
}
