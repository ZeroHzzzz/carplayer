#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern DataHandler datahandler;
extern ImageHandler imagehandler;
extern Controller controller;
extern SpeedHandler speedhandler;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

QString MainWindow::getSavePathText() const {
    return ui->textEdit_savepath->toPlainText();
}

void MainWindow::on_pushButton_browsersavepath_clicked() {
    QString dirPath = QFileDialog::getOpenFileName(this, "选择目录", "");

    if (!dirPath.isEmpty()) {
        ui->textEdit_savepath->setText(dirPath);
    }
}

void MainWindow::on_textEdit_savepath_textChanged() {
    QString savePath = ui->textEdit_savepath->toPlainText();
    if (!QFileInfo::exists(savePath)) {
        QMessageBox::warning(this, "路径错误",
                             "读取路径无效，请选择有效的文件路径。");
    }
}

void MainWindow::on_ReadData_clicked() {}
