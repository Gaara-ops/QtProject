#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	setWindowTitle(tr("Qt Test"));
	m_timer = new QTimer(this);
	connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString a1 = ui->lineEdit_1->text();
    QString a2 = ui->lineEdit_2->text();
    QStringList list1 = a1.split("/");
    QStringList list2 = a2.split("/");
    if(list1.size() != 2 || list2.size() != 2){
        QString res = "input error!!";
        ui->lineEdit_result->setText(res);
        return;
    }
    double num11 = list1[0].toDouble();
    double num12 = list1[1].toDouble();
    if(num12 == 0){
        QString res = "the denominator is zero!!";
        ui->lineEdit_result->setText(res);
        return;
    }
    double num21 = list2[0].toDouble();
    double num22 = list2[1].toDouble();
    if(num22 == 0){
        QString res = "the denominator is zero!!";
        ui->lineEdit_result->setText(res);
        return;
    }
    if(num22 == num12){
        if(num11 == num21){
            ui->lineEdit_result->setText("A=B");
            return;
        }else if(num11 > num21){
            ui->lineEdit_result->setText("A>B");
            return;
        }else{
            ui->lineEdit_result->setText("A<B");
            return;
        }
    }else{
        //double denominator = num22*num12;
        int tmpa1 = num11*num22;
        int tmpa2 = num21*num12;
        if(tmpa1 == tmpa2){
            ui->lineEdit_result->setText("A=B");
            return;
        }else if(tmpa1 > tmpa2){
            ui->lineEdit_result->setText("A>B");
            return;
        }else{
            ui->lineEdit_result->setText("A<B");
            return;
        }
    }
}

void MainWindow::slotTimeOut()
{

}
