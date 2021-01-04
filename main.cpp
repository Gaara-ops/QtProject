#include "mainwindow.h"
#include <QApplication>

//#ifndef INITIAL_OPENGL
//#define INITIAL_OPENGL
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//#endif
#include <vtkInteractorStyleTrackballCamera.h>
/*vtkNew<vtkInteractorStyleTrackballCamera> sty;
  iren->SetInteractorStyle(sty);*/
#include <QDebug>

#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"

//体信息
struct VolumeInfo{
public:
    int dim[3];
    double spacing[3];
    double scalerange[2];
    double imagebound[6];
};
vtkDataArray* m_imagearr = NULL;
//相机相关信息
struct CameraInfo{
public:
    double focalpos[3];
    double camerapos[3];
    double viewup[3];
    double direction[3];
    double foctocam[3];
    double viewleft[3];
    double cliprange[2];
    double parallelscale;

    int winsize[2];
    int viewport[4];

    double modelviewMatrix[16];
    double projectionMatrix[16];
    double compositeMatrix[16];
};
void initModelViewMatrix(CameraInfo& caminfo){
    caminfo.modelviewMatrix[0]=caminfo.viewleft[0];caminfo.modelviewMatrix[1]=caminfo.viewleft[1]; caminfo.modelviewMatrix[2]=caminfo.viewleft[2]; caminfo.modelviewMatrix[3]=0;
    caminfo.modelviewMatrix[4]=caminfo.viewup[0]; caminfo.modelviewMatrix[5]=caminfo.viewup[1]; caminfo.modelviewMatrix[6]=caminfo.viewup[2]; caminfo.modelviewMatrix[7]=0;
    caminfo.modelviewMatrix[8]=caminfo.foctocam[0]; caminfo.modelviewMatrix[9]=caminfo.foctocam[1]; caminfo.modelviewMatrix[10]=caminfo.foctocam[2];caminfo.modelviewMatrix[11]=0;
    caminfo.modelviewMatrix[12]=0;caminfo.modelviewMatrix[13]=0;caminfo.modelviewMatrix[14]=0;caminfo.modelviewMatrix[15]=1;

    caminfo.modelviewMatrix[3] =  caminfo.viewleft[0]*(-caminfo.camerapos[0])+caminfo.viewleft[1]*(-caminfo.camerapos[1])+caminfo.viewleft[2]*(-caminfo.camerapos[2]);
    caminfo.modelviewMatrix[7] =  caminfo.viewup[0]*(-caminfo.camerapos[0])+caminfo.viewup[1]*(-caminfo.camerapos[1])+caminfo.viewup[2]*(-caminfo.camerapos[2]);
    caminfo.modelviewMatrix[11] = caminfo.foctocam[0]*(-caminfo.camerapos[0])+caminfo.foctocam[1]*(-caminfo.camerapos[1])+caminfo.foctocam[2]*(-caminfo.camerapos[2]);
}
void initCompositeMatrix(CameraInfo& caminfo){
    caminfo.projectionMatrix[0]=1; caminfo.projectionMatrix[1]=0; caminfo.projectionMatrix[2]=0;   caminfo.projectionMatrix[3]=0;
    caminfo.projectionMatrix[4]=0; caminfo.projectionMatrix[5]=1; caminfo.projectionMatrix[6]=0;   caminfo.projectionMatrix[7]=0;
    caminfo.projectionMatrix[8]=0; caminfo.projectionMatrix[9]=0; caminfo.projectionMatrix[10]=0.5;caminfo.projectionMatrix[11]=0.5;
    caminfo.projectionMatrix[12]=0;caminfo.projectionMatrix[13]=0;caminfo.projectionMatrix[14]=0;  caminfo.projectionMatrix[15]=1;

    double aspect = caminfo.winsize[0]/caminfo.winsize[1];
    double vwidth = caminfo.parallelscale*aspect;
    double vheight = caminfo.parallelscale;
    caminfo.projectionMatrix[0]=1.0/vwidth;
    caminfo.projectionMatrix[5]=1.0/vheight;
    caminfo.projectionMatrix[10] *= (-2.0/(caminfo.cliprange[1]-caminfo.cliprange[0]));
    caminfo.projectionMatrix[11]= caminfo.projectionMatrix[10]*(-(caminfo.cliprange[1]+caminfo.cliprange[0])/(caminfo.cliprange[1]-caminfo.cliprange[0]))+0.5;


    caminfo.compositeMatrix[0] =  caminfo.projectionMatrix[0]*caminfo.modelviewMatrix[0]+caminfo.projectionMatrix[1]*caminfo.modelviewMatrix[4]
            +caminfo.projectionMatrix[2]*caminfo.modelviewMatrix[8]+caminfo.projectionMatrix[3]*caminfo.modelviewMatrix[12];
    caminfo.compositeMatrix[1] =  caminfo.projectionMatrix[0]*caminfo.modelviewMatrix[1]+caminfo.projectionMatrix[1]*caminfo.modelviewMatrix[5]
            +caminfo.projectionMatrix[2]*caminfo.modelviewMatrix[9]+caminfo.projectionMatrix[3]*caminfo.modelviewMatrix[13];
    caminfo.compositeMatrix[2] =  caminfo.projectionMatrix[0]*caminfo.modelviewMatrix[2]+caminfo.projectionMatrix[1]*caminfo.modelviewMatrix[6]
            +caminfo.projectionMatrix[2]*caminfo.modelviewMatrix[10]+caminfo.projectionMatrix[3]*caminfo.modelviewMatrix[14];
    caminfo.compositeMatrix[3] =  caminfo.projectionMatrix[0]*caminfo.modelviewMatrix[3]+caminfo.projectionMatrix[1]*caminfo.modelviewMatrix[7]
            +caminfo.projectionMatrix[2]*caminfo.modelviewMatrix[11]+caminfo.projectionMatrix[3]*caminfo.modelviewMatrix[15];

    caminfo.compositeMatrix[4] =  caminfo.projectionMatrix[4]*caminfo.modelviewMatrix[0]+caminfo.projectionMatrix[5]*caminfo.modelviewMatrix[4]
            +caminfo.projectionMatrix[6]*caminfo.modelviewMatrix[8]+caminfo.projectionMatrix[7]*caminfo.modelviewMatrix[12];
    caminfo.compositeMatrix[5] =  caminfo.projectionMatrix[4]*caminfo.modelviewMatrix[1]+caminfo.projectionMatrix[5]*caminfo.modelviewMatrix[5]
            +caminfo.projectionMatrix[6]*caminfo.modelviewMatrix[9]+caminfo.projectionMatrix[7]*caminfo.modelviewMatrix[13];
    caminfo.compositeMatrix[6] =  caminfo.projectionMatrix[4]*caminfo.modelviewMatrix[2]+caminfo.projectionMatrix[5]*caminfo.modelviewMatrix[6]
            +caminfo.projectionMatrix[6]*caminfo.modelviewMatrix[10]+caminfo.projectionMatrix[7]*caminfo.modelviewMatrix[14];
    caminfo.compositeMatrix[7] =  caminfo.projectionMatrix[4]*caminfo.modelviewMatrix[3]+caminfo.projectionMatrix[5]*caminfo.modelviewMatrix[7]
            +caminfo.projectionMatrix[6]*caminfo.modelviewMatrix[11]+caminfo.projectionMatrix[7]*caminfo.modelviewMatrix[15];

    caminfo.compositeMatrix[8] =  caminfo.projectionMatrix[8]*caminfo.modelviewMatrix[0]+caminfo.projectionMatrix[9]*caminfo.modelviewMatrix[4]
            +caminfo.projectionMatrix[10]*caminfo.modelviewMatrix[8]+caminfo.projectionMatrix[11]*caminfo.modelviewMatrix[12];
    caminfo.compositeMatrix[9] =  caminfo.projectionMatrix[8]*caminfo.modelviewMatrix[1]+caminfo.projectionMatrix[9]*caminfo.modelviewMatrix[5]
            +caminfo.projectionMatrix[10]*caminfo.modelviewMatrix[9]+caminfo.projectionMatrix[11]*caminfo.modelviewMatrix[13];
    caminfo.compositeMatrix[10] = caminfo.projectionMatrix[8]*caminfo.modelviewMatrix[2]+caminfo.projectionMatrix[9]*caminfo.modelviewMatrix[6]
            +caminfo.projectionMatrix[10]*caminfo.modelviewMatrix[10]+caminfo.projectionMatrix[11]*caminfo.modelviewMatrix[14];
    caminfo.compositeMatrix[11] = caminfo.projectionMatrix[8]*caminfo.modelviewMatrix[3]+caminfo.projectionMatrix[9]*caminfo.modelviewMatrix[7]
            +caminfo.projectionMatrix[10]*caminfo.modelviewMatrix[11]+caminfo.projectionMatrix[11]*caminfo.modelviewMatrix[15];

    caminfo.compositeMatrix[12] = caminfo.projectionMatrix[12]*caminfo.modelviewMatrix[0]+caminfo.projectionMatrix[13]*caminfo.modelviewMatrix[4]
            +caminfo.projectionMatrix[14]*caminfo.modelviewMatrix[8]+caminfo.projectionMatrix[15]*caminfo.modelviewMatrix[12];
    caminfo.compositeMatrix[13] = caminfo.projectionMatrix[12]*caminfo.modelviewMatrix[1]+caminfo.projectionMatrix[13]*caminfo.modelviewMatrix[5]
            +caminfo.projectionMatrix[14]*caminfo.modelviewMatrix[9]+caminfo.projectionMatrix[15]*caminfo.modelviewMatrix[13];
    caminfo.compositeMatrix[14] = caminfo.projectionMatrix[12]*caminfo.modelviewMatrix[2]+caminfo.projectionMatrix[13]*caminfo.modelviewMatrix[6]
            +caminfo.projectionMatrix[14]*caminfo.modelviewMatrix[10]+caminfo.projectionMatrix[15]*caminfo.modelviewMatrix[14];
    caminfo.compositeMatrix[15] = caminfo.projectionMatrix[12]*caminfo.modelviewMatrix[3]+caminfo.projectionMatrix[13]*caminfo.modelviewMatrix[7]
            +caminfo.projectionMatrix[14]*caminfo.modelviewMatrix[11]+caminfo.projectionMatrix[15]*caminfo.modelviewMatrix[15];
}
void Get2DPosByViewportPos(CameraInfo& caminfo,double view3d[3],double pos2d[3]){
    double dx = (view3d[0]+1.0)*(caminfo.winsize[0]*(caminfo.viewport[2]-caminfo.viewport[0]))/2 + caminfo.winsize[0]*caminfo.viewport[0];
    double dy = (view3d[1]+1.0)*(caminfo.winsize[1]*(caminfo.viewport[3]-caminfo.viewport[1]))/2 + caminfo.winsize[1]*caminfo.viewport[1];
    pos2d[0]=(dx);
    pos2d[1]=(dy);
    pos2d[2]=(view3d[2]);
}
void Get2DPosBy3DPos(CameraInfo& caminfo,double vec3d[3],double pos2d[3]){
    double viewpoint[3];
    viewpoint[0] = caminfo.compositeMatrix[0]*vec3d[0]+caminfo.compositeMatrix[1]*vec3d[1]
            +caminfo.compositeMatrix[2]*vec3d[2]+caminfo.compositeMatrix[3]*1;
    viewpoint[1] = caminfo.compositeMatrix[4]*vec3d[0]+caminfo.compositeMatrix[5]*vec3d[1]
            +caminfo.compositeMatrix[6]*vec3d[2]+caminfo.compositeMatrix[7]*1;
    viewpoint[2] = caminfo.compositeMatrix[8]*vec3d[0]+caminfo.compositeMatrix[9]*vec3d[1]
            +caminfo.compositeMatrix[10]*vec3d[2]+caminfo.compositeMatrix[11]*1;

    Get2DPosByViewportPos(caminfo,viewpoint,pos2d);
}
void Get3DPosBy2DPos(CameraInfo& caminfo,double pos2d[3],double vec3d[3],double elem[16]){

    double vx = 2.0 * (pos2d[0] - caminfo.winsize[0]*caminfo.viewport[0])/
      (caminfo.winsize[0]*(caminfo.viewport[2]-caminfo.viewport[0])) - 1.0;
    double vy = 2.0 * (pos2d[1] - caminfo.winsize[1]*caminfo.viewport[1])/
      (caminfo.winsize[1]*(caminfo.viewport[3]-caminfo.viewport[1])) - 1.0;
    double vz = pos2d[2];

    double v1=vx,v2=vy,v3=vz,v4=1;
    double out[4];
    out[0] = v1*elem[0]  + v2*elem[1]  + v3*elem[2]  + v4*elem[3];
    out[1] = v1*elem[4]  + v2*elem[5]  + v3*elem[6]  + v4*elem[7];
    out[2] = v1*elem[8]  + v2*elem[9]  + v3*elem[10] + v4*elem[11];
    out[3] = v1*elem[12] + v2*elem[13] + v3*elem[14] + v4*elem[15];

    vec3d[0]=(out[0]);
    vec3d[1]=(out[1]);
    vec3d[2]=(out[2]);
}


double Determinant2x2(double a, double b, double c, double d) {
    return (a * d - b * c);
}

double Determinant3x3(double a1, double a2, double a3,
                                      double b1, double b2, double b3,
                                      double c1, double c2, double c3)
{
    return ( a1 * Determinant2x2( b2, b3, c2, c3 )
           - b1 * Determinant2x2( a2, a3, c2, c3 )
           + c1 * Determinant2x2( a2, a3, b2, b3 ) );
}

double Determinant(const double elem[16])
{
  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

  // assign to individual variable names to aid selecting
  //  correct elements

  a1 = elem[0];  b1 = elem[1];  c1 = elem[2];  d1 = elem[3];
  a2 = elem[4];  b2 = elem[5];  c2 = elem[6];  d2 = elem[7];
  a3 = elem[8];  b3 = elem[9];  c3 = elem[10]; d3 = elem[11];
  a4 = elem[12]; b4 = elem[13]; c4 = elem[14]; d4 = elem[15];

  return a1 * Determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
       - b1 * Determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
       + c1 * Determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
       - d1 * Determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

void Adjoint(const double elem[16], double outElem[16])
{
  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
  a1 = elem[0];  b1 = elem[1];  c1 = elem[2];  d1 = elem[3];
  a2 = elem[4];  b2 = elem[5];  c2 = elem[6];  d2 = elem[7];
  a3 = elem[8];  b3 = elem[9];  c3 = elem[10]; d3 = elem[11];
  a4 = elem[12]; b4 = elem[13]; c4 = elem[14]; d4 = elem[15];
  outElem[0]  =
    Determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
  outElem[4]  =
    - Determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
  outElem[8]  =
    Determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
  outElem[12]  =
    - Determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

  outElem[1]  =
    - Determinant3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
  outElem[5]  =
    Determinant3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
  outElem[9]  =
    - Determinant3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
  outElem[13]  =
    Determinant3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

  outElem[2]  =
    Determinant3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
  outElem[6]  =
    - Determinant3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
  outElem[10]  =
    Determinant3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
  outElem[14]  =
    - Determinant3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

  outElem[3]  =
    - Determinant3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
  outElem[7]  =
    Determinant3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
  outElem[11]  =
    - Determinant3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
  outElem[15]  =
    Determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

void Invert(const double inElements[16],
                          double outElements[16])
{
  double det = Determinant(inElements);
  if (det == 0.0)
  {
    return;
  }

  Adjoint(inElements, outElements);

  for (int i = 0; i < 16; i++)
  {
    outElements[i] /= det;
  }
}
#define VTK_RIGHT 0
#define VTK_LEFT 1
#define VTK_MIDDLE 2

int g_num1=0,g_num2=0;
char IntersectBox (double bounds[6], double origin[3], double dir[3],
                           double coord[3], double& t)
{
  bool    inside=true;
  char    quadrant[3];
  int     i, whichPlane=0;
  double  maxT[3], candidatePlane[3];
  //  First find closest planes
  for (i=0; i<3; i++)
  {
    if ( origin[i] < bounds[2*i] )
    {
      quadrant[i] = VTK_LEFT;
      candidatePlane[i] = bounds[2*i];
      inside = false;
    }
    else if ( origin[i] > bounds[2*i+1] )
    {
      quadrant[i] = VTK_RIGHT;
      candidatePlane[i] = bounds[2*i+1];
      inside = false;
    }
    else
    {
      quadrant[i] = VTK_MIDDLE;
    }
  }
  //  Check whether origin of ray is inside bbox
  if (inside)
  {
    coord[0] = origin[0];
    coord[1] = origin[1];
    coord[2] = origin[2];
    t = 0;
    return 1;
  }
  //  Calculate parametric distances to plane
  for (i=0; i<3; i++)
  {
    if ( quadrant[i] != VTK_MIDDLE && dir[i] != 0.0 )
    {
      maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
    }
    else
    {
      maxT[i] = -1.0;
    }
  }
  //  Find the largest parametric value of intersection
  for (i=0; i<3; i++)
  {
    if ( maxT[whichPlane] < maxT[i] )
    {
      whichPlane = i;
    }
  }
  //  Check for valid intersection along line
//  if ( maxT[whichPlane] > 1.0 || maxT[whichPlane] < 0.0 )
  if ( maxT[whichPlane] < 0.0 )
  {
      ++g_num1;
    return 0;
  }
  else
  {
    t = maxT[whichPlane];
  }

  for (i=0; i<3; i++)
  {
    if (whichPlane != i)
    {
      coord[i] = origin[i] + maxT[whichPlane]*dir[i];
      if ( coord[i] < bounds[2*i] || coord[i] > bounds[2*i+1] )
      {
          ++g_num2;
        return 0;
      }
    }
    else
    {
      coord[i] = candidatePlane[i];
    }
  }

  return 1;
}


int main(int argc, char *argv[])
{
    QApplication a11(argc, argv);
    //    MainWindow m;
    //    m.resize(1024+22,768+108);
    //    m.show();
    VolumeInfo volumeinfo;
    CameraInfo camerainfo;

    qDebug() << "begin!!!!!";
    char *dirname = "../../DICOM/20ceng";
    vtkDICOMImageReader* dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(dirname);
    dicomReader->Update();
    dicomReader->GetOutput()->GetDimensions(volumeinfo.dim);
    dicomReader->GetOutput()->GetSpacing(volumeinfo.spacing);
    dicomReader->GetOutput()->GetScalarRange(volumeinfo.scalerange);
    m_imagearr=dicomReader->GetOutput()->GetPointData()->GetScalars();
    volumeinfo.imagebound[0]=volumeinfo.imagebound[2]=volumeinfo.imagebound[4]=0;
    volumeinfo.imagebound[1]=volumeinfo.imagebound[3]=(volumeinfo.dim[0]-1)*volumeinfo.spacing[0];
    volumeinfo.imagebound[5]=(volumeinfo.dim[2]-1)*volumeinfo.spacing[2];

    int ny=512,nx=512;
    camerainfo.winsize[0]=nx;
    camerainfo.winsize[1]=ny;
    camerainfo.viewport[0]=camerainfo.viewport[1]=0;
    camerainfo.viewport[2]=camerainfo.viewport[3]=1;
    for(int i=0;i<16;i++){
        camerainfo.modelviewMatrix[i]=0;
        camerainfo.projectionMatrix[i]=0;
        camerainfo.compositeMatrix[i]=0;
    }

    camerainfo.focalpos[0]=camerainfo.focalpos[1]=volumeinfo.imagebound[1]/2;
    camerainfo.focalpos[2]=volumeinfo.imagebound[5]/2;
    camerainfo.camerapos[0]=camerainfo.focalpos[0];
    camerainfo.camerapos[1]=camerainfo.focalpos[1];
    camerainfo.camerapos[2]=camerainfo.focalpos[2]+500;
    camerainfo.viewup[0]=0;
    camerainfo.viewup[1]=1;
    camerainfo.viewup[2]=0;

    camerainfo.foctocam[0]=0;camerainfo.foctocam[1]=0;camerainfo.foctocam[2]=1;
    camerainfo.viewleft[0]=1;camerainfo.viewleft[1]=0;camerainfo.viewleft[2]=0;
    camerainfo.direction[0] = -camerainfo.foctocam[0];
    camerainfo.direction[1] = -camerainfo.foctocam[1];
    camerainfo.direction[2] = -camerainfo.foctocam[2];

    camerainfo.cliprange[0]=0.01;
    camerainfo.cliprange[1]=1000.01;
    camerainfo.parallelscale=(sqrt(volumeinfo.imagebound[1]*volumeinfo.imagebound[1]*2+volumeinfo.imagebound[5]*volumeinfo.imagebound[5]))/2;
    initModelViewMatrix(camerainfo);
    initCompositeMatrix(camerainfo);

    double temp2d[3];
    Get2DPosBy3DPos(camerainfo,camerainfo.camerapos,temp2d);
    qDebug() << "temp2d:" << temp2d[0]<<temp2d[1]<<temp2d[2];

    double elem[16];
    Invert(camerainfo.compositeMatrix,elem);
    double temp2d2[3]={0,0,temp2d[2]};


    int imageindex = 0;
    ofstream outfile;
    QString strname = "index" +QString::number(imageindex) + ".ppm";
    outfile.open(strname.toStdString().c_str());
    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    double ori[3];double coord[3]; double t;
//    double rgbvalue[3] = {1};
    int rgbvalue=255;
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            temp2d2[0]=(i);temp2d2[1]=(j);
            Get3DPosBy2DPos(camerainfo,temp2d2,ori,elem);


            char hit = IntersectBox(volumeinfo.imagebound,ori,camerainfo.direction,coord,t);
            if(hit)
            {
                int tempdim[3] = {coord[0]/volumeinfo.spacing[0],coord[1]/volumeinfo.spacing[1],coord[2]/volumeinfo.spacing[2]};

                int tupleidx = tempdim[2]*volumeinfo.dim[1]*volumeinfo.dim[0]+tempdim[1]*volumeinfo.dim[0] + tempdim[0];
                double ctvalue = m_imagearr->GetComponent(tupleidx,0);
                rgbvalue = ((ctvalue-volumeinfo.scalerange[0])/(volumeinfo.scalerange[1]-volumeinfo.scalerange[0]))*255;
            }else{
                rgbvalue = 255;
            }

            int ir = rgbvalue;
            int ig = rgbvalue;
            int ib = rgbvalue;
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile.close();
    qDebug() << "end!!!!!";
    return a11.exec();
}
