#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTextEdit>

//使用c++将数据写入到文件
#include <iostream>
#include <fstream>
std::ofstream g_save;
int g_lineLen=0;
int g_step = sizeof(int)+2;
int g_lineLimit = 160;
int g_lineNum = 0;
//获取内存信息
#include <windows.h>
#include <sysinfoapi.h>
#define MB (1024*1024)
MEMORYSTATUSEX g_MemoryInfo;//in sysinfoapi
//获取当前时间
#include <QTime>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(800,500);
    ui->textEdit->setVisible(1);
    ui->webView->setVisible(0);
    /*QUrl strurl("file:///E:/workspace/MemoryShow/res/mytest.html");
    ui->webView->setUrl(strurl);
    ui->webView->setHtml();*/

    //定时器初始化
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotGetMemoryInfo()));
    m_timer->start(50);//每xx ms执行一次slotGetMemoryInfo函数
    //创建文件
    CreateMemoryFile();
}

MainWindow::~MainWindow()
{
    delete ui;
    g_save.close();
}

void MainWindow::CreateMemoryFile()
{
    //保存文件初始化
    //当前的时间
    //QString nowtime = QTime::currentTime().toString("hh_mm_ss");
    //当前的年月日和时间
    QString nowdatetime = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString strFileName = "./" +nowdatetime + "_memoryinfo.txt";
    std::string fileName = strFileName.toStdString();
    g_save.open(fileName, std::ofstream::out);
}

void MainWindow::CreateNewMemoryFile()
{
    if(g_lineNum*g_lineLimit > MB){
        g_save.close();
        CreateMemoryFile();
    }
}
#include<QScrollBar>
void MainWindow::slotGetMemoryInfo()
{
    //获取内存信息
    g_MemoryInfo.dwLength = sizeof(g_MemoryInfo);
    GlobalMemoryStatusEx(&g_MemoryInfo);
    DWORD mload = g_MemoryInfo.dwMemoryLoad;
    DWORDLONG tphys = g_MemoryInfo.ullTotalPhys;
    int memorySize = tphys*(mload*0.01)/MB;
    //将内存信息保存到文件
    g_save << memorySize << "  ";
    g_lineLen += g_step;
    if(g_lineLen >= g_lineLimit){
        g_save << std::endl;
        g_lineLen = 0;
        ++g_lineNum;
    }
    /**
      将内存信息显示在textEdit中*/
    //ui->textEdit->append();//换行添加文件
    //在一行的末尾追加，若一行已满则添加至下一行
    ui->textEdit->insertPlainText(QString::number(memorySize)+"  ");
    QScrollBar* scroll = ui->textEdit->verticalScrollBar();
    //使滚动条滑动到最下面
    scroll->setSliderPosition(scroll->maximum());
}
