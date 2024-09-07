#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern DataHandler datahandler;
extern ImageHandler imagehandler;
extern Controller controller;
extern SpeedHandler speedhandler;
extern MapHandler maphandler;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showAllMaps(QPoint point) {
    // if (maphandler.imgSize == _188_180) {
    //     maphandler.showImgGray(imagehandler.imgGray, ui->imgGray_label,
    //     point);
    // }

    maphandler.showMap(imagehandler.allmap, ui->AllMap, point);
    maphandler.showMap(imagehandler.basemap, ui->BaseMap, point);
    maphandler.showMap(imagehandler.linemap, ui->LineMap, point);
    maphandler.showMap(imagehandler.leftmap, ui->LeftMap, point);
    maphandler.showMap(imagehandler.rightmap, ui->RightMap, point);
    maphandler.showMap(imagehandler.insidemap, ui->InsideMap, point);
    maphandler.showMap(imagehandler.deletemap, ui->DeleteMap, point);
    maphandler.showRealMap(ui->RealMap, point);
}

QString MainWindow::getReadPathText() const {
    return ui->textEdit_readpath->toPlainText();
}

void MainWindow::on_pushButton_browserreadpath_clicked() {
    QString dirPath = QFileDialog::getOpenFileName(this, "选择目录", "");

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
    QString readpath = getReadPathText();
    bool status = datahandler.readdata(readpath);
    status = imagehandler.standard();
}
