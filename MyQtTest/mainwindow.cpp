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

}

void MainWindow::slotTimeOut()
{

}
