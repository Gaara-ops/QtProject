#include "mainwindow.h"
#include <QApplication>
#include"globeinclude.h"

void GetInterFaceName(QByteArray line,QString& name){
    QList<QByteArray> list = line.split('|');
    if(list.size() < 3){
        return;
    }
    name = QString(list.at(list.size()-2)).trimmed();
}

bool WriteToFile(QByteArray& line,QByteArray& flag,int& timeindex,
                 QTextStream& out,QString& strtime,QString& name){
    bool result = false;
    if(line.contains(flag)){
        int startIndex = line.indexOf(flag);
        strtime = line.mid(1,23);
        GetInterFaceName(line.right(line.length()-startIndex),name);
        QString linestr = line.left(timeindex)+"--";
        linestr += line.right(line.length()-startIndex);
        out << linestr;
        result = true;
    }
    return result;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString strFormat = "yyyy-MM-dd HH:mm:ss:zzz";
    QString nowdatetime = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString strFileName = "./" +nowdatetime + "_logAnalysis.txt";
    QFile outFile(strFileName);
    outFile.open( QIODevice::ReadWrite );//| QIODevice::Append
    QTextStream out(&outFile);

    QString preDir = "G:/iMAGESServer/branches/Version2/Bin/Win32/Server/MinGW/Debug/SpiderSightServer/Log/";
    QString inputName = preDir + "log.log";
    QByteArray flag1 = "start";
    QByteArray flag2 = "finish";
    QString strTime1="",strTime2="";
    QString strName1="",strName2="";
    QString lastTime="",lastName="";
    int timeindex = 25;
    int allTimeSpace=0,allTime=0;
    QFile inFile(inputName);
    inFile.open( QIODevice::ReadOnly);
    while(!inFile.atEnd()){
        QByteArray line = inFile.readLine();
        bool res1 = WriteToFile(line,flag1,timeindex,out,strTime1,strName1);
        bool res2 = WriteToFile(line,flag2,timeindex,out,strTime2,strName2);
        if(strTime1!="" && lastTime!="" && res1){
            QDateTime time1 = QDateTime::fromString(lastTime,strFormat);
            QDateTime time2 = QDateTime::fromString(strTime1,strFormat);
            int timespace = time2.toMSecsSinceEpoch()-time1.toMSecsSinceEpoch();
            allTimeSpace+=timespace;
            //打印比较耗时的接口
            if(timespace > 100){
                qDebug() << strName1 << "->" << lastName << "--" << timespace;
            }
        }
        if((strName1 == strName2)&& (strName1!="") && res2){
            QDateTime time1 = QDateTime::fromString(strTime1,strFormat);
            QDateTime time2 = QDateTime::fromString(strTime2,strFormat);
            int timespace = time2.toMSecsSinceEpoch()-time1.toMSecsSinceEpoch();
            allTime+=timespace;
            //打印比较耗时的接口
            if(timespace > 100){
                qDebug() << strName1 << "--" << timespace;
            }
            out << strName1 << "--" << timespace << "\n";
            lastTime = strTime2;lastName = strName2;
            strTime1=strTime2=strName1=strName2="";
        }
    }
    out << "allTime:" << allTime+allTimeSpace << ",  "
             << "interface space:" << allTimeSpace << ",  "
             << "interface time:" << allTime;
    inFile.close();
    outFile.close();
    qDebug() << "end";
    return a.exec();
}
