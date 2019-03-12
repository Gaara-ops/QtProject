#include "showimage.h"

#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QDebug>
#include <QTime>
#include <omp.h>

#include"raytraceinfo.h"

ShowImage::ShowImage()
{
    leftBtnDown=false;
    lastposx=0;
    lastposy=0;
    nowposx = 0;
    nowposy = 0;
    rayinfo = NULL;
    readytocal = true;
    showready = false;
}

void ShowImage::updateLabel(int width,int height,int* colorptr)
{
    QImage image( width, height, QImage::Format_RGB32 );
    QRgb *rgbPtr =
      reinterpret_cast<QRgb *>( image.bits() );// + width * ( height - 1 );
    // Loop over the vtkImageData contents.

    #pragma omp parallel for
    for ( int row = 0; row < height; row++ )
    {
      for ( int col = 0; col < width; col++ )
      {
          int imindex = (row*width+col)*3;
        // Swap the vtkImageData RGB values with an equivalent QColor
          //*( rgbPtr++ )
        rgbPtr[row*width+col] = QColor( colorptr[imindex],colorptr[imindex+1],colorptr[imindex+2]).rgb();
      }
//      rgbPtr -= width * 2;
    }

    QPixmap pixmapToShow = QPixmap::fromImage( image );
    this->setPixmap(pixmapToShow);
}

void ShowImage::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        lastposx = e->x();
        lastposy = e->y();
        leftBtnDown = true;
    }
}

void ShowImage::mouseMoveEvent(QMouseEvent *e)
{
    nowposx = e->x();
    nowposy = e->y();
    if (leftBtnDown&&readytocal)
    {
        readytocal = false;
        int dx = nowposx - lastposx;
        int dy = nowposy - lastposy;

        int size[2] = {rayinfo->m_winsize[0],rayinfo->m_winsize[1]};

        double delta_elevation = -20.0 / size[1];
        double delta_azimuth   = -20.0 / size[0];

        double rxf = dx * delta_azimuth   *10.0;
        double ryf = dy * delta_elevation *10.0;

        QTime timetest;
        timetest.start();
        rayinfo->AzimuthRayInfo(-rxf);
        //qDebug() << "Azimuth:"<<timetest.elapsed()/1000.0;
        rayinfo->ElevationRayInfo(ryf);
        //qDebug() << "Elevation:"<<timetest.elapsed()/1000.0;
        rayinfo->updateImage(rayinfo->m_direction);
        //qDebug() << "updateImage:"<<timetest.elapsed()/1000.0;
        updateLabel(rayinfo->m_winsize[0],rayinfo->m_winsize[1],rayinfo->m_imageptr);
        //qDebug() << "updateLabel:"<<timetest.elapsed()/1000.0;
        readytocal = true;
    }
    lastposx = e->x();
    lastposy = e->y();
}

void ShowImage::mouseReleaseEvent(QMouseEvent *e)
{
    leftBtnDown = false;
}

void ShowImage::mouseDoubleClickEvent(QMouseEvent *e)
{

}

void ShowImage::resizeEvent(QResizeEvent *e)
{
    if(e->size() == e->oldSize())
        return;
    if(!showready)
        return;
    int width = e->size().width();
    int height = e->size().height();
    if(rayinfo!=NULL && readytocal){
        readytocal = false;
        rayinfo->m_winsize[0]=width;
        rayinfo->m_winsize[1]=height;
        rayinfo->updateWinSize();
        updateLabel(rayinfo->m_winsize[0],rayinfo->m_winsize[1],rayinfo->m_imageptr);
        readytocal = true;
    }
}
