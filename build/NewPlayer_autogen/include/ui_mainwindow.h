/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *comboBox_device;
    QPushButton *ReadSDData;
    QTextEdit *textEdit_readpath;
    QPushButton *pushButton_browserreadpath;
    QPushButton *ReadData;
    QGroupBox *groupBox;
    QLabel *imgGray_label;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *RightMap;
    QLabel *RealMap;
    QLabel *LeftMap;
    QLabel *DeleteMap;
    QLabel *BaseMap;
    QLabel *InsideMap;
    QLabel *AllMap;
    QLabel *LineMap;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QSlider *slider;
    QLCDNumber *lcdNumber;
    QLabel *pixelInfo_label;
    QPushButton *backward;
    QPushButton *forward;
    QPushButton *pause;
    QGroupBox *groupBox_2;
    QLabel *STEP_label;
    QLabel *cornerLeft_label;
    QLabel *bnum_label;
    QLabel *rnum_label;
    QLabel *top_left_label;
    QLabel *left_top_label;
    QLabel *top_label;
    QLabel *speedTop_label;
    QLabel *bottom_left_label;
    QLabel *num_rm_label;
    QLabel *left_bottom_label;
    QLabel *num_lm_label;
    QLabel *right_bottom_label;
    QLabel *top_right_label;
    QLabel *lnum_label;
    QLabel *start_lm_label;
    QLabel *bottom_right_label;
    QLabel *dnum_all_label;
    QLabel *right_top_label;
    QLabel *cornerRight_label;
    QLabel *start_rm_label;
    QLabel *Ramp_label;
    QLabel *angle_protect_label;
    QLabel *Annulus_label;
    QLabel *actual_ramp_label;
    QLabel *actual_IF_label;
    QLabel *actual_park_label;
    QLabel *parking_label;
    QLabel *force_brake_label;
    QLabel *speedMin_label;
    QLabel *act_TarYawrate_label;
    QLabel *motorPWML_label;
    QLabel *nowDev_label;
    QLabel *kbright_label;
    QLabel *rightnum_label;
    QLabel *line_forbid_label;
    QLabel *act_dirPWM_label;
    QLabel *controlLine_label;
    QLabel *leftnum_label;
    QLabel *annuYaw_label;
    QLabel *motorPWMR_label;
    QLabel *kbmid_label;
    QLabel *midnum_label;
    QLabel *running_brake_flag_label;
    QLabel *Yaw_Var_label;
    QLabel *LSMI_DK_label;
    QLabel *dirPWM_label;
    QLabel *pidOut_label;
    QLabel *kbleft_label;
    QLabel *speedPWM_label;
    QLabel *act_curvature_label;
    QLabel *aimSpeed_label;
    QLabel *MPID_ki_label;
    QLabel *MPID_AC_P_label;
    QLabel *MPID_UD_label;
    QLabel *MPID_oncei_label;
    QLabel *aimSpeedL_label;
    QLabel *aimSpeedR_label;
    QLabel *ADC2_label;
    QLabel *ADC4_label;
    QLabel *ARI_nowSpeedL_label;
    QLabel *ADC1_label;
    QLabel *act_speedType_label;
    QLabel *ARI_aimSpeed_label;
    QLabel *Pitch_label;
    QLabel *ARI_brakeType_label;
    QLabel *act_brakeType_label;
    QLabel *ARI_speedType_label;
    QLabel *Yaw_label;
    QLabel *ARI_aimSpeedL_label;
    QLabel *ADC_MAX_label;
    QLabel *ADC0_label;
    QLabel *ADC3_label;
    QLabel *yawRate_label;
    QLabel *SDInfo_label;
    QLabel *ARI_nowSpeedR_label;
    QLabel *ARI_aimSpeedR_label;
    QLabel *nowSpeedR_label;
    QLabel *ARI_nowThreshold_label;
    QLabel *pitchRate_label;
    QLabel *d_yawRate_label;
    QLabel *nowSpeedM_label;
    QLabel *TF_DISTENCE_label;
    QLabel *TF_STRENGTH_label;
    QLabel *ARI_nowDeviation_label;
    QLabel *act_dYRate_label;
    QLabel *nowSpeedL_label;
    QLabel *servo_turn_label;
    QLabel *aveDevLeft_label;
    QLabel *aveDevRight_label;
    QLabel *aveDev_label;
    QLabel *nowThreshold_label;
    QComboBox *readType_combobox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1937, 935);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        comboBox_device = new QComboBox(centralwidget);
        comboBox_device->setObjectName(QString::fromUtf8("comboBox_device"));
        comboBox_device->setGeometry(QRect(30, 10, 101, 31));
        ReadSDData = new QPushButton(centralwidget);
        ReadSDData->setObjectName(QString::fromUtf8("ReadSDData"));
        ReadSDData->setGeometry(QRect(150, 10, 101, 31));
        textEdit_readpath = new QTextEdit(centralwidget);
        textEdit_readpath->setObjectName(QString::fromUtf8("textEdit_readpath"));
        textEdit_readpath->setGeometry(QRect(290, 10, 331, 31));
        pushButton_browserreadpath = new QPushButton(centralwidget);
        pushButton_browserreadpath->setObjectName(QString::fromUtf8("pushButton_browserreadpath"));
        pushButton_browserreadpath->setGeometry(QRect(640, 10, 80, 31));
        ReadData = new QPushButton(centralwidget);
        ReadData->setObjectName(QString::fromUtf8("ReadData"));
        ReadData->setGeometry(QRect(750, 10, 81, 31));
        ReadData->setAutoDefault(false);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 40, 911, 861));
        imgGray_label = new QLabel(groupBox);
        imgGray_label->setObjectName(QString::fromUtf8("imgGray_label"));
        imgGray_label->setGeometry(QRect(710, 680, 72, 15));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 30, 734, 611));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        RightMap = new QLabel(layoutWidget);
        RightMap->setObjectName(QString::fromUtf8("RightMap"));
        RightMap->setMinimumSize(QSize(47, 60));
        RightMap->setBaseSize(QSize(47, 60));
        RightMap->setScaledContents(true);

        gridLayout->addWidget(RightMap, 1, 1, 1, 1);

        RealMap = new QLabel(layoutWidget);
        RealMap->setObjectName(QString::fromUtf8("RealMap"));
        RealMap->setMinimumSize(QSize(47, 60));
        RealMap->setBaseSize(QSize(47, 60));
        RealMap->setScaledContents(true);

        gridLayout->addWidget(RealMap, 0, 2, 1, 1);

        LeftMap = new QLabel(layoutWidget);
        LeftMap->setObjectName(QString::fromUtf8("LeftMap"));
        LeftMap->setMinimumSize(QSize(47, 60));
        LeftMap->setBaseSize(QSize(47, 60));
        LeftMap->setScaledContents(true);

        gridLayout->addWidget(LeftMap, 1, 0, 1, 1);

        DeleteMap = new QLabel(layoutWidget);
        DeleteMap->setObjectName(QString::fromUtf8("DeleteMap"));
        DeleteMap->setMinimumSize(QSize(47, 60));
        DeleteMap->setBaseSize(QSize(47, 60));
        DeleteMap->setScaledContents(true);

        gridLayout->addWidget(DeleteMap, 2, 0, 1, 1);

        BaseMap = new QLabel(layoutWidget);
        BaseMap->setObjectName(QString::fromUtf8("BaseMap"));
        BaseMap->setMinimumSize(QSize(47, 60));
        BaseMap->setBaseSize(QSize(47, 60));
        BaseMap->setScaledContents(true);

        gridLayout->addWidget(BaseMap, 0, 0, 1, 1);

        InsideMap = new QLabel(layoutWidget);
        InsideMap->setObjectName(QString::fromUtf8("InsideMap"));
        InsideMap->setMinimumSize(QSize(47, 60));
        InsideMap->setBaseSize(QSize(47, 60));
        InsideMap->setScaledContents(true);

        gridLayout->addWidget(InsideMap, 2, 1, 1, 1);

        AllMap = new QLabel(layoutWidget);
        AllMap->setObjectName(QString::fromUtf8("AllMap"));
        AllMap->setMinimumSize(QSize(47, 60));
        AllMap->setBaseSize(QSize(47, 60));
        AllMap->setScaledContents(true);

        gridLayout->addWidget(AllMap, 1, 2, 1, 1);

        LineMap = new QLabel(layoutWidget);
        LineMap->setObjectName(QString::fromUtf8("LineMap"));
        LineMap->setMinimumSize(QSize(47, 60));
        LineMap->setBaseSize(QSize(47, 60));
        LineMap->setScaledContents(true);

        gridLayout->addWidget(LineMap, 0, 1, 1, 1);

        layoutWidget1 = new QWidget(groupBox);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(70, 710, 561, 41));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        slider = new QSlider(layoutWidget1);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setOrientation(Qt::Horizontal);
        slider->setTickInterval(1);

        horizontalLayout_2->addWidget(slider);

        lcdNumber = new QLCDNumber(layoutWidget1);
        lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));

        horizontalLayout_2->addWidget(lcdNumber);

        pixelInfo_label = new QLabel(groupBox);
        pixelInfo_label->setObjectName(QString::fromUtf8("pixelInfo_label"));
        pixelInfo_label->setGeometry(QRect(780, 20, 41, 16));
        backward = new QPushButton(groupBox);
        backward->setObjectName(QString::fromUtf8("backward"));
        backward->setGeometry(QRect(180, 795, 80, 31));
        forward = new QPushButton(groupBox);
        forward->setObjectName(QString::fromUtf8("forward"));
        forward->setGeometry(QRect(400, 795, 80, 31));
        pause = new QPushButton(groupBox);
        pause->setObjectName(QString::fromUtf8("pause"));
        pause->setGeometry(QRect(290, 795, 80, 31));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(970, 40, 921, 721));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        STEP_label = new QLabel(groupBox_2);
        STEP_label->setObjectName(QString::fromUtf8("STEP_label"));
        STEP_label->setGeometry(QRect(20, 490, 67, 17));
        cornerLeft_label = new QLabel(groupBox_2);
        cornerLeft_label->setObjectName(QString::fromUtf8("cornerLeft_label"));
        cornerLeft_label->setGeometry(QRect(160, 490, 91, 17));
        bnum_label = new QLabel(groupBox_2);
        bnum_label->setObjectName(QString::fromUtf8("bnum_label"));
        bnum_label->setGeometry(QRect(20, 240, 67, 17));
        rnum_label = new QLabel(groupBox_2);
        rnum_label->setObjectName(QString::fromUtf8("rnum_label"));
        rnum_label->setGeometry(QRect(160, 280, 67, 17));
        top_left_label = new QLabel(groupBox_2);
        top_left_label->setObjectName(QString::fromUtf8("top_left_label"));
        top_left_label->setGeometry(QRect(160, 370, 67, 17));
        left_top_label = new QLabel(groupBox_2);
        left_top_label->setObjectName(QString::fromUtf8("left_top_label"));
        left_top_label->setGeometry(QRect(160, 400, 67, 17));
        top_label = new QLabel(groupBox_2);
        top_label->setObjectName(QString::fromUtf8("top_label"));
        top_label->setGeometry(QRect(20, 210, 67, 17));
        speedTop_label = new QLabel(groupBox_2);
        speedTop_label->setObjectName(QString::fromUtf8("speedTop_label"));
        speedTop_label->setGeometry(QRect(160, 210, 67, 17));
        bottom_left_label = new QLabel(groupBox_2);
        bottom_left_label->setObjectName(QString::fromUtf8("bottom_left_label"));
        bottom_left_label->setGeometry(QRect(160, 460, 101, 17));
        num_rm_label = new QLabel(groupBox_2);
        num_rm_label->setObjectName(QString::fromUtf8("num_rm_label"));
        num_rm_label->setGeometry(QRect(160, 310, 67, 17));
        left_bottom_label = new QLabel(groupBox_2);
        left_bottom_label->setObjectName(QString::fromUtf8("left_bottom_label"));
        left_bottom_label->setGeometry(QRect(160, 430, 101, 17));
        num_lm_label = new QLabel(groupBox_2);
        num_lm_label->setObjectName(QString::fromUtf8("num_lm_label"));
        num_lm_label->setGeometry(QRect(20, 310, 67, 17));
        right_bottom_label = new QLabel(groupBox_2);
        right_bottom_label->setObjectName(QString::fromUtf8("right_bottom_label"));
        right_bottom_label->setGeometry(QRect(20, 430, 121, 17));
        top_right_label = new QLabel(groupBox_2);
        top_right_label->setObjectName(QString::fromUtf8("top_right_label"));
        top_right_label->setGeometry(QRect(20, 370, 91, 17));
        lnum_label = new QLabel(groupBox_2);
        lnum_label->setObjectName(QString::fromUtf8("lnum_label"));
        lnum_label->setGeometry(QRect(20, 280, 67, 17));
        start_lm_label = new QLabel(groupBox_2);
        start_lm_label->setObjectName(QString::fromUtf8("start_lm_label"));
        start_lm_label->setGeometry(QRect(20, 340, 71, 17));
        bottom_right_label = new QLabel(groupBox_2);
        bottom_right_label->setObjectName(QString::fromUtf8("bottom_right_label"));
        bottom_right_label->setGeometry(QRect(20, 460, 111, 17));
        dnum_all_label = new QLabel(groupBox_2);
        dnum_all_label->setObjectName(QString::fromUtf8("dnum_all_label"));
        dnum_all_label->setGeometry(QRect(160, 240, 67, 17));
        right_top_label = new QLabel(groupBox_2);
        right_top_label->setObjectName(QString::fromUtf8("right_top_label"));
        right_top_label->setGeometry(QRect(20, 400, 81, 17));
        cornerRight_label = new QLabel(groupBox_2);
        cornerRight_label->setObjectName(QString::fromUtf8("cornerRight_label"));
        cornerRight_label->setGeometry(QRect(20, 560, 101, 17));
        start_rm_label = new QLabel(groupBox_2);
        start_rm_label->setObjectName(QString::fromUtf8("start_rm_label"));
        start_rm_label->setGeometry(QRect(160, 340, 67, 17));
        Ramp_label = new QLabel(groupBox_2);
        Ramp_label->setObjectName(QString::fromUtf8("Ramp_label"));
        Ramp_label->setGeometry(QRect(20, 110, 67, 17));
        angle_protect_label = new QLabel(groupBox_2);
        angle_protect_label->setObjectName(QString::fromUtf8("angle_protect_label"));
        angle_protect_label->setGeometry(QRect(21, 50, 111, 20));
        Annulus_label = new QLabel(groupBox_2);
        Annulus_label->setObjectName(QString::fromUtf8("Annulus_label"));
        Annulus_label->setGeometry(QRect(20, 90, 67, 17));
        actual_ramp_label = new QLabel(groupBox_2);
        actual_ramp_label->setObjectName(QString::fromUtf8("actual_ramp_label"));
        actual_ramp_label->setGeometry(QRect(20, 130, 91, 16));
        actual_IF_label = new QLabel(groupBox_2);
        actual_IF_label->setObjectName(QString::fromUtf8("actual_IF_label"));
        actual_IF_label->setGeometry(QRect(20, 170, 111, 16));
        actual_park_label = new QLabel(groupBox_2);
        actual_park_label->setObjectName(QString::fromUtf8("actual_park_label"));
        actual_park_label->setGeometry(QRect(20, 150, 91, 16));
        parking_label = new QLabel(groupBox_2);
        parking_label->setObjectName(QString::fromUtf8("parking_label"));
        parking_label->setGeometry(QRect(20, 70, 72, 15));
        force_brake_label = new QLabel(groupBox_2);
        force_brake_label->setObjectName(QString::fromUtf8("force_brake_label"));
        force_brake_label->setGeometry(QRect(20, 30, 101, 16));
        speedMin_label = new QLabel(groupBox_2);
        speedMin_label->setObjectName(QString::fromUtf8("speedMin_label"));
        speedMin_label->setGeometry(QRect(330, 420, 72, 15));
        act_TarYawrate_label = new QLabel(groupBox_2);
        act_TarYawrate_label->setObjectName(QString::fromUtf8("act_TarYawrate_label"));
        act_TarYawrate_label->setGeometry(QRect(330, 580, 121, 20));
        motorPWML_label = new QLabel(groupBox_2);
        motorPWML_label->setObjectName(QString::fromUtf8("motorPWML_label"));
        motorPWML_label->setGeometry(QRect(330, 520, 72, 15));
        nowDev_label = new QLabel(groupBox_2);
        nowDev_label->setObjectName(QString::fromUtf8("nowDev_label"));
        nowDev_label->setGeometry(QRect(330, 360, 67, 17));
        kbright_label = new QLabel(groupBox_2);
        kbright_label->setObjectName(QString::fromUtf8("kbright_label"));
        kbright_label->setGeometry(QRect(180, 70, 67, 17));
        rightnum_label = new QLabel(groupBox_2);
        rightnum_label->setObjectName(QString::fromUtf8("rightnum_label"));
        rightnum_label->setGeometry(QRect(400, 50, 67, 17));
        line_forbid_label = new QLabel(groupBox_2);
        line_forbid_label->setObjectName(QString::fromUtf8("line_forbid_label"));
        line_forbid_label->setGeometry(QRect(180, 90, 101, 17));
        act_dirPWM_label = new QLabel(groupBox_2);
        act_dirPWM_label->setObjectName(QString::fromUtf8("act_dirPWM_label"));
        act_dirPWM_label->setGeometry(QRect(330, 540, 81, 16));
        controlLine_label = new QLabel(groupBox_2);
        controlLine_label->setObjectName(QString::fromUtf8("controlLine_label"));
        controlLine_label->setGeometry(QRect(180, 110, 101, 17));
        leftnum_label = new QLabel(groupBox_2);
        leftnum_label->setObjectName(QString::fromUtf8("leftnum_label"));
        leftnum_label->setGeometry(QRect(400, 30, 67, 17));
        annuYaw_label = new QLabel(groupBox_2);
        annuYaw_label->setObjectName(QString::fromUtf8("annuYaw_label"));
        annuYaw_label->setGeometry(QRect(330, 460, 72, 15));
        motorPWMR_label = new QLabel(groupBox_2);
        motorPWMR_label->setObjectName(QString::fromUtf8("motorPWMR_label"));
        motorPWMR_label->setGeometry(QRect(400, 250, 72, 15));
        kbmid_label = new QLabel(groupBox_2);
        kbmid_label->setObjectName(QString::fromUtf8("kbmid_label"));
        kbmid_label->setGeometry(QRect(180, 30, 67, 17));
        midnum_label = new QLabel(groupBox_2);
        midnum_label->setObjectName(QString::fromUtf8("midnum_label"));
        midnum_label->setGeometry(QRect(400, 70, 67, 17));
        running_brake_flag_label = new QLabel(groupBox_2);
        running_brake_flag_label->setObjectName(QString::fromUtf8("running_brake_flag_label"));
        running_brake_flag_label->setGeometry(QRect(330, 600, 151, 20));
        Yaw_Var_label = new QLabel(groupBox_2);
        Yaw_Var_label->setObjectName(QString::fromUtf8("Yaw_Var_label"));
        Yaw_Var_label->setGeometry(QRect(330, 500, 72, 15));
        LSMI_DK_label = new QLabel(groupBox_2);
        LSMI_DK_label->setObjectName(QString::fromUtf8("LSMI_DK_label"));
        LSMI_DK_label->setGeometry(QRect(330, 440, 72, 15));
        dirPWM_label = new QLabel(groupBox_2);
        dirPWM_label->setObjectName(QString::fromUtf8("dirPWM_label"));
        dirPWM_label->setGeometry(QRect(330, 400, 67, 17));
        pidOut_label = new QLabel(groupBox_2);
        pidOut_label->setObjectName(QString::fromUtf8("pidOut_label"));
        pidOut_label->setGeometry(QRect(330, 380, 67, 17));
        kbleft_label = new QLabel(groupBox_2);
        kbleft_label->setObjectName(QString::fromUtf8("kbleft_label"));
        kbleft_label->setGeometry(QRect(180, 50, 67, 17));
        speedPWM_label = new QLabel(groupBox_2);
        speedPWM_label->setObjectName(QString::fromUtf8("speedPWM_label"));
        speedPWM_label->setGeometry(QRect(330, 480, 72, 15));
        act_curvature_label = new QLabel(groupBox_2);
        act_curvature_label->setObjectName(QString::fromUtf8("act_curvature_label"));
        act_curvature_label->setGeometry(QRect(330, 560, 111, 20));
        aimSpeed_label = new QLabel(groupBox_2);
        aimSpeed_label->setObjectName(QString::fromUtf8("aimSpeed_label"));
        aimSpeed_label->setGeometry(QRect(400, 100, 72, 15));
        MPID_ki_label = new QLabel(groupBox_2);
        MPID_ki_label->setObjectName(QString::fromUtf8("MPID_ki_label"));
        MPID_ki_label->setGeometry(QRect(400, 220, 72, 15));
        MPID_AC_P_label = new QLabel(groupBox_2);
        MPID_AC_P_label->setObjectName(QString::fromUtf8("MPID_AC_P_label"));
        MPID_AC_P_label->setGeometry(QRect(400, 130, 81, 16));
        MPID_UD_label = new QLabel(groupBox_2);
        MPID_UD_label->setObjectName(QString::fromUtf8("MPID_UD_label"));
        MPID_UD_label->setGeometry(QRect(400, 160, 72, 15));
        MPID_oncei_label = new QLabel(groupBox_2);
        MPID_oncei_label->setObjectName(QString::fromUtf8("MPID_oncei_label"));
        MPID_oncei_label->setGeometry(QRect(400, 190, 91, 16));
        aimSpeedL_label = new QLabel(groupBox_2);
        aimSpeedL_label->setObjectName(QString::fromUtf8("aimSpeedL_label"));
        aimSpeedL_label->setGeometry(QRect(400, 290, 41, 9));
        aimSpeedR_label = new QLabel(groupBox_2);
        aimSpeedR_label->setObjectName(QString::fromUtf8("aimSpeedR_label"));
        aimSpeedR_label->setGeometry(QRect(400, 320, 41, 9));
        ADC2_label = new QLabel(groupBox_2);
        ADC2_label->setObjectName(QString::fromUtf8("ADC2_label"));
        ADC2_label->setGeometry(QRect(540, 210, 67, 17));
        ADC4_label = new QLabel(groupBox_2);
        ADC4_label->setObjectName(QString::fromUtf8("ADC4_label"));
        ADC4_label->setGeometry(QRect(540, 270, 67, 17));
        ARI_nowSpeedL_label = new QLabel(groupBox_2);
        ARI_nowSpeedL_label->setObjectName(QString::fromUtf8("ARI_nowSpeedL_label"));
        ARI_nowSpeedL_label->setGeometry(QRect(540, 90, 121, 17));
        ADC1_label = new QLabel(groupBox_2);
        ADC1_label->setObjectName(QString::fromUtf8("ADC1_label"));
        ADC1_label->setGeometry(QRect(540, 180, 67, 17));
        act_speedType_label = new QLabel(groupBox_2);
        act_speedType_label->setObjectName(QString::fromUtf8("act_speedType_label"));
        act_speedType_label->setGeometry(QRect(540, 430, 111, 16));
        ARI_aimSpeed_label = new QLabel(groupBox_2);
        ARI_aimSpeed_label->setObjectName(QString::fromUtf8("ARI_aimSpeed_label"));
        ARI_aimSpeed_label->setGeometry(QRect(540, 30, 111, 17));
        Pitch_label = new QLabel(groupBox_2);
        Pitch_label->setObjectName(QString::fromUtf8("Pitch_label"));
        Pitch_label->setGeometry(QRect(540, 300, 67, 17));
        ARI_brakeType_label = new QLabel(groupBox_2);
        ARI_brakeType_label->setObjectName(QString::fromUtf8("ARI_brakeType_label"));
        ARI_brakeType_label->setGeometry(QRect(540, 390, 111, 17));
        act_brakeType_label = new QLabel(groupBox_2);
        act_brakeType_label->setObjectName(QString::fromUtf8("act_brakeType_label"));
        act_brakeType_label->setGeometry(QRect(540, 470, 111, 16));
        ARI_speedType_label = new QLabel(groupBox_2);
        ARI_speedType_label->setObjectName(QString::fromUtf8("ARI_speedType_label"));
        ARI_speedType_label->setGeometry(QRect(540, 360, 121, 17));
        Yaw_label = new QLabel(groupBox_2);
        Yaw_label->setObjectName(QString::fromUtf8("Yaw_label"));
        Yaw_label->setGeometry(QRect(540, 330, 67, 17));
        ARI_aimSpeedL_label = new QLabel(groupBox_2);
        ARI_aimSpeedL_label->setObjectName(QString::fromUtf8("ARI_aimSpeedL_label"));
        ARI_aimSpeedL_label->setGeometry(QRect(540, 60, 121, 17));
        ADC_MAX_label = new QLabel(groupBox_2);
        ADC_MAX_label->setObjectName(QString::fromUtf8("ADC_MAX_label"));
        ADC_MAX_label->setGeometry(QRect(540, 120, 67, 17));
        ADC0_label = new QLabel(groupBox_2);
        ADC0_label->setObjectName(QString::fromUtf8("ADC0_label"));
        ADC0_label->setGeometry(QRect(540, 150, 67, 17));
        ADC3_label = new QLabel(groupBox_2);
        ADC3_label->setObjectName(QString::fromUtf8("ADC3_label"));
        ADC3_label->setGeometry(QRect(540, 240, 67, 17));
        yawRate_label = new QLabel(groupBox_2);
        yawRate_label->setObjectName(QString::fromUtf8("yawRate_label"));
        yawRate_label->setGeometry(QRect(710, 330, 67, 17));
        SDInfo_label = new QLabel(groupBox_2);
        SDInfo_label->setObjectName(QString::fromUtf8("SDInfo_label"));
        SDInfo_label->setGeometry(QRect(710, 30, 67, 17));
        ARI_nowSpeedR_label = new QLabel(groupBox_2);
        ARI_nowSpeedR_label->setObjectName(QString::fromUtf8("ARI_nowSpeedR_label"));
        ARI_nowSpeedR_label->setGeometry(QRect(710, 90, 121, 17));
        ARI_aimSpeedR_label = new QLabel(groupBox_2);
        ARI_aimSpeedR_label->setObjectName(QString::fromUtf8("ARI_aimSpeedR_label"));
        ARI_aimSpeedR_label->setGeometry(QRect(710, 60, 111, 17));
        nowSpeedR_label = new QLabel(groupBox_2);
        nowSpeedR_label->setObjectName(QString::fromUtf8("nowSpeedR_label"));
        nowSpeedR_label->setGeometry(QRect(710, 530, 81, 16));
        ARI_nowThreshold_label = new QLabel(groupBox_2);
        ARI_nowThreshold_label->setObjectName(QString::fromUtf8("ARI_nowThreshold_label"));
        ARI_nowThreshold_label->setGeometry(QRect(710, 180, 151, 17));
        pitchRate_label = new QLabel(groupBox_2);
        pitchRate_label->setObjectName(QString::fromUtf8("pitchRate_label"));
        pitchRate_label->setGeometry(QRect(710, 300, 91, 17));
        d_yawRate_label = new QLabel(groupBox_2);
        d_yawRate_label->setObjectName(QString::fromUtf8("d_yawRate_label"));
        d_yawRate_label->setGeometry(QRect(710, 370, 72, 15));
        nowSpeedM_label = new QLabel(groupBox_2);
        nowSpeedM_label->setObjectName(QString::fromUtf8("nowSpeedM_label"));
        nowSpeedM_label->setGeometry(QRect(710, 460, 101, 16));
        TF_DISTENCE_label = new QLabel(groupBox_2);
        TF_DISTENCE_label->setObjectName(QString::fromUtf8("TF_DISTENCE_label"));
        TF_DISTENCE_label->setGeometry(QRect(710, 120, 131, 17));
        TF_STRENGTH_label = new QLabel(groupBox_2);
        TF_STRENGTH_label->setObjectName(QString::fromUtf8("TF_STRENGTH_label"));
        TF_STRENGTH_label->setGeometry(QRect(710, 150, 101, 17));
        ARI_nowDeviation_label = new QLabel(groupBox_2);
        ARI_nowDeviation_label->setObjectName(QString::fromUtf8("ARI_nowDeviation_label"));
        ARI_nowDeviation_label->setGeometry(QRect(710, 210, 151, 17));
        act_dYRate_label = new QLabel(groupBox_2);
        act_dYRate_label->setObjectName(QString::fromUtf8("act_dYRate_label"));
        act_dYRate_label->setGeometry(QRect(710, 400, 81, 16));
        nowSpeedL_label = new QLabel(groupBox_2);
        nowSpeedL_label->setObjectName(QString::fromUtf8("nowSpeedL_label"));
        nowSpeedL_label->setGeometry(QRect(710, 500, 72, 15));
        servo_turn_label = new QLabel(groupBox_2);
        servo_turn_label->setObjectName(QString::fromUtf8("servo_turn_label"));
        servo_turn_label->setGeometry(QRect(540, 510, 41, 9));
        aveDevLeft_label = new QLabel(groupBox_2);
        aveDevLeft_label->setObjectName(QString::fromUtf8("aveDevLeft_label"));
        aveDevLeft_label->setGeometry(QRect(540, 540, 41, 9));
        aveDevRight_label = new QLabel(groupBox_2);
        aveDevRight_label->setObjectName(QString::fromUtf8("aveDevRight_label"));
        aveDevRight_label->setGeometry(QRect(540, 570, 41, 9));
        aveDev_label = new QLabel(groupBox_2);
        aveDev_label->setObjectName(QString::fromUtf8("aveDev_label"));
        aveDev_label->setGeometry(QRect(540, 600, 41, 9));
        nowThreshold_label = new QLabel(groupBox_2);
        nowThreshold_label->setObjectName(QString::fromUtf8("nowThreshold_label"));
        nowThreshold_label->setGeometry(QRect(20, 610, 41, 9));
        readType_combobox = new QComboBox(centralwidget);
        readType_combobox->addItem(QString());
        readType_combobox->addItem(QString());
        readType_combobox->setObjectName(QString::fromUtf8("readType_combobox"));
        readType_combobox->setGeometry(QRect(860, 10, 71, 31));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1937, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ReadSDData->setText(QCoreApplication::translate("MainWindow", "ReadSDData", nullptr));
        pushButton_browserreadpath->setText(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210", nullptr));
        ReadData->setText(QCoreApplication::translate("MainWindow", "ReadData", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        imgGray_label->setText(QCoreApplication::translate("MainWindow", "imgGray", nullptr));
        RightMap->setText(QCoreApplication::translate("MainWindow", "RightMap", nullptr));
        RealMap->setText(QCoreApplication::translate("MainWindow", "RealMap", nullptr));
        LeftMap->setText(QCoreApplication::translate("MainWindow", "LeftMap", nullptr));
        DeleteMap->setText(QCoreApplication::translate("MainWindow", "DeleteMap", nullptr));
        BaseMap->setText(QCoreApplication::translate("MainWindow", "BaseMap", nullptr));
        InsideMap->setText(QCoreApplication::translate("MainWindow", "InsideMap", nullptr));
        AllMap->setText(QCoreApplication::translate("MainWindow", "AllMap", nullptr));
        LineMap->setText(QCoreApplication::translate("MainWindow", "LineMap", nullptr));
        pixelInfo_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        backward->setText(QCoreApplication::translate("MainWindow", "\345\220\216\351\200\200", nullptr));
        forward->setText(QCoreApplication::translate("MainWindow", "\345\211\215\350\277\233", nullptr));
        pause->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213/\346\232\202\345\201\234", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        STEP_label->setText(QCoreApplication::translate("MainWindow", "STEP", nullptr));
        cornerLeft_label->setText(QCoreApplication::translate("MainWindow", "cornerLeft", nullptr));
        bnum_label->setText(QCoreApplication::translate("MainWindow", "bnum", nullptr));
        rnum_label->setText(QCoreApplication::translate("MainWindow", "rnum", nullptr));
        top_left_label->setText(QCoreApplication::translate("MainWindow", "top_left", nullptr));
        left_top_label->setText(QCoreApplication::translate("MainWindow", "left_top", nullptr));
        top_label->setText(QCoreApplication::translate("MainWindow", "top", nullptr));
        speedTop_label->setText(QCoreApplication::translate("MainWindow", "speedTop", nullptr));
        bottom_left_label->setText(QCoreApplication::translate("MainWindow", "bottom_left", nullptr));
        num_rm_label->setText(QCoreApplication::translate("MainWindow", "num_rm", nullptr));
        left_bottom_label->setText(QCoreApplication::translate("MainWindow", "left_bottom", nullptr));
        num_lm_label->setText(QCoreApplication::translate("MainWindow", "num_lm", nullptr));
        right_bottom_label->setText(QCoreApplication::translate("MainWindow", "right_bottom", nullptr));
        top_right_label->setText(QCoreApplication::translate("MainWindow", "top_right", nullptr));
        lnum_label->setText(QCoreApplication::translate("MainWindow", "lnum", nullptr));
        start_lm_label->setText(QCoreApplication::translate("MainWindow", "start_lm", nullptr));
        bottom_right_label->setText(QCoreApplication::translate("MainWindow", "bottom_right", nullptr));
        dnum_all_label->setText(QCoreApplication::translate("MainWindow", "dnum_all", nullptr));
        right_top_label->setText(QCoreApplication::translate("MainWindow", "right_top", nullptr));
        cornerRight_label->setText(QCoreApplication::translate("MainWindow", "cornerRight", nullptr));
        start_rm_label->setText(QCoreApplication::translate("MainWindow", "start_rm", nullptr));
        Ramp_label->setText(QCoreApplication::translate("MainWindow", "Ramp", nullptr));
        angle_protect_label->setText(QCoreApplication::translate("MainWindow", "angle_protect", nullptr));
        Annulus_label->setText(QCoreApplication::translate("MainWindow", "Annulus", nullptr));
        actual_ramp_label->setText(QCoreApplication::translate("MainWindow", "actual_ramp", nullptr));
        actual_IF_label->setText(QCoreApplication::translate("MainWindow", "actual_annulus", nullptr));
        actual_park_label->setText(QCoreApplication::translate("MainWindow", "actual_park", nullptr));
        parking_label->setText(QCoreApplication::translate("MainWindow", "Parking", nullptr));
        force_brake_label->setText(QCoreApplication::translate("MainWindow", "force_brake", nullptr));
        speedMin_label->setText(QCoreApplication::translate("MainWindow", "speedMin", nullptr));
        act_TarYawrate_label->setText(QCoreApplication::translate("MainWindow", "act_TarYawrate", nullptr));
        motorPWML_label->setText(QCoreApplication::translate("MainWindow", "motorPWML", nullptr));
        nowDev_label->setText(QCoreApplication::translate("MainWindow", "nowDev", nullptr));
        kbright_label->setText(QCoreApplication::translate("MainWindow", "kbright", nullptr));
        rightnum_label->setText(QCoreApplication::translate("MainWindow", "rightnum", nullptr));
        line_forbid_label->setText(QCoreApplication::translate("MainWindow", "line_forbid", nullptr));
        act_dirPWM_label->setText(QCoreApplication::translate("MainWindow", "act_dirPWM", nullptr));
        controlLine_label->setText(QCoreApplication::translate("MainWindow", "controlLine", nullptr));
        leftnum_label->setText(QCoreApplication::translate("MainWindow", "leftnum", nullptr));
        annuYaw_label->setText(QCoreApplication::translate("MainWindow", "annuYaw", nullptr));
        motorPWMR_label->setText(QCoreApplication::translate("MainWindow", "motorPWMR", nullptr));
        kbmid_label->setText(QCoreApplication::translate("MainWindow", "kbmid", nullptr));
        midnum_label->setText(QCoreApplication::translate("MainWindow", "midnum", nullptr));
        running_brake_flag_label->setText(QCoreApplication::translate("MainWindow", "running_brake_flag", nullptr));
        Yaw_Var_label->setText(QCoreApplication::translate("MainWindow", "Yaw_Var", nullptr));
        LSMI_DK_label->setText(QCoreApplication::translate("MainWindow", "LSMI.DK", nullptr));
        dirPWM_label->setText(QCoreApplication::translate("MainWindow", "dirPWM", nullptr));
        pidOut_label->setText(QCoreApplication::translate("MainWindow", "pidOut", nullptr));
        kbleft_label->setText(QCoreApplication::translate("MainWindow", "kbleft", nullptr));
        speedPWM_label->setText(QCoreApplication::translate("MainWindow", "speedPWM", nullptr));
        act_curvature_label->setText(QCoreApplication::translate("MainWindow", "act_curvature", nullptr));
        aimSpeed_label->setText(QCoreApplication::translate("MainWindow", "aimSpeed", nullptr));
        MPID_ki_label->setText(QCoreApplication::translate("MainWindow", "MPID_ki", nullptr));
        MPID_AC_P_label->setText(QCoreApplication::translate("MainWindow", "MPID_AC_P", nullptr));
        MPID_UD_label->setText(QCoreApplication::translate("MainWindow", "MPID_UD", nullptr));
        MPID_oncei_label->setText(QCoreApplication::translate("MainWindow", "MPID_oncei", nullptr));
        aimSpeedL_label->setText(QCoreApplication::translate("MainWindow", "aimSpeedL_label", nullptr));
        aimSpeedR_label->setText(QCoreApplication::translate("MainWindow", "aimSpeedR_label", nullptr));
        ADC2_label->setText(QCoreApplication::translate("MainWindow", "ADC2", nullptr));
        ADC4_label->setText(QCoreApplication::translate("MainWindow", "ADC4", nullptr));
        ARI_nowSpeedL_label->setText(QCoreApplication::translate("MainWindow", "ARI_nowSpeedL", nullptr));
        ADC1_label->setText(QCoreApplication::translate("MainWindow", "ADC1", nullptr));
        act_speedType_label->setText(QCoreApplication::translate("MainWindow", "act_speedType", nullptr));
        ARI_aimSpeed_label->setText(QCoreApplication::translate("MainWindow", "ARI_aimSpeed", nullptr));
        Pitch_label->setText(QCoreApplication::translate("MainWindow", "Pitch", nullptr));
        ARI_brakeType_label->setText(QCoreApplication::translate("MainWindow", "ARI_brakeType", nullptr));
        act_brakeType_label->setText(QCoreApplication::translate("MainWindow", "act_brakeType", nullptr));
        ARI_speedType_label->setText(QCoreApplication::translate("MainWindow", "ARI_speedType", nullptr));
        Yaw_label->setText(QCoreApplication::translate("MainWindow", "Yaw", nullptr));
        ARI_aimSpeedL_label->setText(QCoreApplication::translate("MainWindow", "ARI_aimSpeedL", nullptr));
        ADC_MAX_label->setText(QCoreApplication::translate("MainWindow", "ADC_MAX", nullptr));
        ADC0_label->setText(QCoreApplication::translate("MainWindow", "ADC0", nullptr));
        ADC3_label->setText(QCoreApplication::translate("MainWindow", "ADC3", nullptr));
        yawRate_label->setText(QCoreApplication::translate("MainWindow", "yawRate", nullptr));
        SDInfo_label->setText(QCoreApplication::translate("MainWindow", "SDInfo", nullptr));
        ARI_nowSpeedR_label->setText(QCoreApplication::translate("MainWindow", "ARI_nowSpeedR", nullptr));
        ARI_aimSpeedR_label->setText(QCoreApplication::translate("MainWindow", "ARI_aimSpeedR", nullptr));
        nowSpeedR_label->setText(QCoreApplication::translate("MainWindow", "nowSpeedR", nullptr));
        ARI_nowThreshold_label->setText(QCoreApplication::translate("MainWindow", "ARI_nowThreshold", nullptr));
        pitchRate_label->setText(QCoreApplication::translate("MainWindow", "pitchRate", nullptr));
        d_yawRate_label->setText(QCoreApplication::translate("MainWindow", "d_yawRate", nullptr));
        nowSpeedM_label->setText(QCoreApplication::translate("MainWindow", "nowSpeedM", nullptr));
        TF_DISTENCE_label->setText(QCoreApplication::translate("MainWindow", "TF_DISTENCE", nullptr));
        TF_STRENGTH_label->setText(QCoreApplication::translate("MainWindow", "TF_STRENGTH", nullptr));
        ARI_nowDeviation_label->setText(QCoreApplication::translate("MainWindow", "ARI_nowDeviation", nullptr));
        act_dYRate_label->setText(QCoreApplication::translate("MainWindow", "act_dYRate", nullptr));
        nowSpeedL_label->setText(QCoreApplication::translate("MainWindow", "nowSpeedL", nullptr));
        servo_turn_label->setText(QCoreApplication::translate("MainWindow", "servo_turn_label", nullptr));
        aveDevLeft_label->setText(QCoreApplication::translate("MainWindow", "aveDevLeft_label", nullptr));
        aveDevRight_label->setText(QCoreApplication::translate("MainWindow", "aveDevRight_label", nullptr));
        aveDev_label->setText(QCoreApplication::translate("MainWindow", "aveDev_label", nullptr));
        nowThreshold_label->setText(QCoreApplication::translate("MainWindow", "nowThreshold_label", nullptr));
        readType_combobox->setItemText(0, QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\250\241\345\274\217", nullptr));
        readType_combobox->setItemText(1, QCoreApplication::translate("MainWindow", "\345\233\276\345\203\217\346\250\241\345\274\217", nullptr));

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
