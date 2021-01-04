#include "mainwindow.h"
#include <QApplication>
#include "globeinclude.h"
#include <QtConcurrent>
#include <QRgb>
#include <QPainter>
#include <random>
#include <QByteArray>
#include <limits>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    return a.exec();
}
