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
#include "maphandler.h"
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

    QString getReadPathText() const;
    void showAllMaps(QPoint point);

    Ui::MainWindow* ui;

   private slots:
    void on_pushButton_browserreadpath_clicked();

    void on_ReadData_clicked();

    void on_textEdit_readpath_textChanged();

   private:
};
#endif  // MAINWINDOW_H
