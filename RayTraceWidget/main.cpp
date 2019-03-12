#include "mainwindow.h"
#include <QApplication>

#include"raytraceinfo.h"
#include"showimage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();


    RayTraceInfo ray;
    ray.updateImage(ray.m_direction);

    ShowImage label;
    label.setWindowTitle("label");
    label.rayinfo = &ray;
    label.updateLabel(ray.m_winsize[0],ray.m_winsize[1],ray.m_imageptr);
    label.show();
    label.showready = true;

    return a.exec();
}
