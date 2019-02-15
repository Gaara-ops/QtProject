#include "mainwindow.h"
#include <QApplication>
#include "globeinclude.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow m;
    m.show();
    QJsonObject obj;
    obj.insert("kk",1);
    obj.insert("kkf",0);
    qDebug()<<obj["kk"].toBool() << obj["kk"].toInt();
    qDebug()<<obj["kkf"].toBool() << obj["kkf"].toInt();

    return a.exec();
}
