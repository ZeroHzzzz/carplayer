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
#include <QtWidgets/QDial>
#include <QtWidgets/QGroupBox>
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
    QTextEdit *textEdit_savepath;
    QPushButton *pushButton_browsersavepath;
    QPushButton *ReadData;
    QGroupBox *groupBox;
    QLabel *BaseMap;
    QLabel *LineMap;
    QLabel *RealMap;
    QLabel *LeftMap;
    QLabel *DeleteMap;
    QLabel *RightMap;
    QLabel *BothLine;
    QLabel *LeftLine;
    QLabel *RightLine;
    QLabel *InsideMap;
    QLabel *AllMap;
    QSlider *slider;
    QLCDNumber *lcdNumber;
    QPushButton *forward;
    QPushButton *backward;
    QDial *dial;
    QPushButton *pause;
    QGroupBox *groupBox_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1118, 649);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        comboBox_device = new QComboBox(centralwidget);
        comboBox_device->setObjectName(QString::fromUtf8("comboBox_device"));
        comboBox_device->setGeometry(QRect(20, 10, 101, 21));
        ReadSDData = new QPushButton(centralwidget);
        ReadSDData->setObjectName(QString::fromUtf8("ReadSDData"));
        ReadSDData->setGeometry(QRect(130, 10, 51, 21));
        textEdit_savepath = new QTextEdit(centralwidget);
        textEdit_savepath->setObjectName(QString::fromUtf8("textEdit_savepath"));
        textEdit_savepath->setGeometry(QRect(200, 10, 101, 21));
        pushButton_browsersavepath = new QPushButton(centralwidget);
        pushButton_browsersavepath->setObjectName(QString::fromUtf8("pushButton_browsersavepath"));
        pushButton_browsersavepath->setGeometry(QRect(310, 10, 51, 21));
        ReadData = new QPushButton(centralwidget);
        ReadData->setObjectName(QString::fromUtf8("ReadData"));
        ReadData->setGeometry(QRect(370, 10, 51, 21));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 40, 421, 501));
        BaseMap = new QLabel(groupBox);
        BaseMap->setObjectName(QString::fromUtf8("BaseMap"));
        BaseMap->setGeometry(QRect(20, 30, 81, 101));
        LineMap = new QLabel(groupBox);
        LineMap->setObjectName(QString::fromUtf8("LineMap"));
        LineMap->setGeometry(QRect(120, 30, 71, 101));
        RealMap = new QLabel(groupBox);
        RealMap->setObjectName(QString::fromUtf8("RealMap"));
        RealMap->setGeometry(QRect(220, 30, 71, 101));
        LeftMap = new QLabel(groupBox);
        LeftMap->setObjectName(QString::fromUtf8("LeftMap"));
        LeftMap->setGeometry(QRect(20, 150, 81, 101));
        DeleteMap = new QLabel(groupBox);
        DeleteMap->setObjectName(QString::fromUtf8("DeleteMap"));
        DeleteMap->setGeometry(QRect(220, 150, 71, 101));
        RightMap = new QLabel(groupBox);
        RightMap->setObjectName(QString::fromUtf8("RightMap"));
        RightMap->setGeometry(QRect(120, 150, 71, 101));
        BothLine = new QLabel(groupBox);
        BothLine->setObjectName(QString::fromUtf8("BothLine"));
        BothLine->setGeometry(QRect(220, 270, 71, 101));
        LeftLine = new QLabel(groupBox);
        LeftLine->setObjectName(QString::fromUtf8("LeftLine"));
        LeftLine->setGeometry(QRect(20, 270, 81, 101));
        RightLine = new QLabel(groupBox);
        RightLine->setObjectName(QString::fromUtf8("RightLine"));
        RightLine->setGeometry(QRect(120, 270, 71, 101));
        InsideMap = new QLabel(groupBox);
        InsideMap->setObjectName(QString::fromUtf8("InsideMap"));
        InsideMap->setGeometry(QRect(320, 150, 71, 101));
        AllMap = new QLabel(groupBox);
        AllMap->setObjectName(QString::fromUtf8("AllMap"));
        AllMap->setGeometry(QRect(320, 270, 71, 101));
        slider = new QSlider(groupBox);
        slider->setObjectName(QString::fromUtf8("slider"));
        slider->setGeometry(QRect(30, 380, 291, 31));
        slider->setOrientation(Qt::Horizontal);
        lcdNumber = new QLCDNumber(groupBox);
        lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));
        lcdNumber->setGeometry(QRect(343, 380, 61, 31));
        forward = new QPushButton(groupBox);
        forward->setObjectName(QString::fromUtf8("forward"));
        forward->setGeometry(QRect(230, 430, 51, 51));
        backward = new QPushButton(groupBox);
        backward->setObjectName(QString::fromUtf8("backward"));
        backward->setGeometry(QRect(90, 430, 51, 51));
        dial = new QDial(groupBox);
        dial->setObjectName(QString::fromUtf8("dial"));
        dial->setGeometry(QRect(160, 420, 50, 64));
        pause = new QPushButton(groupBox);
        pause->setObjectName(QString::fromUtf8("pause"));
        pause->setGeometry(QRect(310, 430, 51, 51));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(460, 0, 561, 541));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1118, 17));
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
        pushButton_browsersavepath->setText(QCoreApplication::translate("MainWindow", "\346\265\217\350\247\210", nullptr));
        ReadData->setText(QCoreApplication::translate("MainWindow", "ReadData", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        BaseMap->setText(QCoreApplication::translate("MainWindow", "BaseMap", nullptr));
        LineMap->setText(QCoreApplication::translate("MainWindow", "LineMap", nullptr));
        RealMap->setText(QCoreApplication::translate("MainWindow", "RealMap", nullptr));
        LeftMap->setText(QCoreApplication::translate("MainWindow", "LeftMap", nullptr));
        DeleteMap->setText(QCoreApplication::translate("MainWindow", "DeleteMap", nullptr));
        RightMap->setText(QCoreApplication::translate("MainWindow", "RightMap", nullptr));
        BothLine->setText(QCoreApplication::translate("MainWindow", "BothLine", nullptr));
        LeftLine->setText(QCoreApplication::translate("MainWindow", "LeftLine", nullptr));
        RightLine->setText(QCoreApplication::translate("MainWindow", "RightLine", nullptr));
        InsideMap->setText(QCoreApplication::translate("MainWindow", "InsideMap", nullptr));
        AllMap->setText(QCoreApplication::translate("MainWindow", "AllMap", nullptr));
        forward->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        backward->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        pause->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
