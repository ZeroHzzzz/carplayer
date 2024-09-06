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

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define II imagehandler.imgInfo
#define IF imagehandler.imgFlag

#define SI speed.speedInfo
#define SP speed.speedParam
#define TDP speed.trackDectionParam

#define DP controller.deviationParam
#define LSMI controller.leastSquareMethodInfo
#define DIRP controller.directionParam
#define LDIRP controller.lastDirectionParam

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    QString getSavePathText() const;

   private slots:
    void on_pushButton_browsersavepath_clicked();

    void on_ReadData_clicked();

    void on_textEdit_savepath_textChanged();

   private:
    Ui::MainWindow* ui;
};
#endif  // MAINWINDOW_H
