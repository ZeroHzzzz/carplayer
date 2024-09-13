#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ctrl.h>
#include <datahandler.h>
#include <imagehandler.h>
#include <speed.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include "motor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define II imagehandler.imgInfo
#define IF imagehandler.imgFlag

#define SI speedhandler.speedInfo
#define SP speedhandler.speedParam
#define TDP speedhandler.trackDectionParam

#define DP controller.deviationParam
#define LSMI controller.leastSquareMethodInfo
#define DIRP controller.directionParam
#define LDIRP controller.lastDirectionParam

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initProgressBar(int);
    void initDeviceCombobox();
    void changePlayerStatus(bool);
    QString getReadDevice() const;
    QString getReadPathText() const;
    QString getReadTypeText() const;
    void showLCDNumber(int);
    void showAllMaps(QPoint point = QPoint(-1, -1));
    bool eventFilter(QObject* object, QEvent* event);
    void mapEvent(uint8_t map[YM][XM], QLabel* label, uint8_t multiple);
    void map_installEventFilter();
    QPoint adjustPixelInfo(QPoint relativePoint,
                           uint8_t mapRow,
                           uint8_t mapCol,
                           uint8_t multiple);
    void onPlaybackTimeout();
    void goToData(int);
    void changeSliderIndex(int index);
    void showVar();

    Ui::MainWindow* ui;
    QTimer* playbackTimer;   // 定时器用于控制播放
    int timerInterval = 20;  // 每帧间隔时间（单位：毫秒）

   private slots:
    void on_pushButton_browserreadpath_clicked();

    void on_ReadData_clicked();

    void on_textEdit_readpath_textChanged();

    void on_backward_clicked();

    void on_forward_clicked();

    void on_pause_clicked();

    void on_slider_valueChanged(int value);

    void on_readType_combobox_currentIndexChanged(int index);

    void on_ReadSDData_clicked();

   private:
};
#endif  // MAINWINDOW_H
