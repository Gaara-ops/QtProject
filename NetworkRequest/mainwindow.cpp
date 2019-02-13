#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->UrlLineEdit->setText("http://192.168.1.68:8888/GetUserAIInfo");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_StartBtn_clicked()
{
    NetworkOpe netope;

    QString strurl = ui->UrlLineEdit->text();
    QString method = ui->comboBox->currentText();
    QString postdata = ui->lineEdit->text();
    QByteArray resdata;
    if(method == "Post"){
        ui->textEdit_Error->append("post request!");
        netope.StartRequest(strurl,2,postdata,resdata);
    }else if(method == "Get"){
        ui->textEdit_Error->append("get request!");
        netope.StartRequest(strurl,3,postdata,resdata);
    }
    ui->textEdit->setText(resdata);
    ui->textEdit->update();
    ui->textEdit_Error->update();
}

void MainWindow::on_ClearBtn_clicked()
{
    ui->textEdit_Error->clear();
}
