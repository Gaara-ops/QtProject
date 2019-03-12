#include "raytraceinfo.h"

#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"


#include"mymath.h"
#include"myhead.h"

RayTraceInfo::RayTraceInfo()
{
    //获取体信息
    char *dirname = "../DICOM/20ceng";
    dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(dirname);
    dicomReader->Update();
    dicomReader->GetOutput()->GetDimensions(m_dim);
    dicomReader->GetOutput()->GetSpacing(m_spacing);
    dicomReader->GetOutput()->GetScalarRange(m_scalerange);
    m_imagebound[0]=m_imagebound[2]=m_imagebound[4]=0;
    m_imagebound[1] = m_imagebound[3] = (m_dim[0]-1)*m_spacing[0];
    m_imagebound[5] = (m_dim[2]-1)*m_spacing[2];
    m_imagearr = dicomReader->GetOutput()->GetPointData()->GetScalars();
    int numtuples = dicomReader->GetOutput()->GetPointData()->GetNumberOfTuples();
    m_imagedataptr = new double[numtuples];
    for(int i=0;i<numtuples;i++){
        m_imagedataptr[i] = dicomReader->GetOutput()->GetPointData()->GetScalars()->GetComponent(i,0);
    }
    dicomReader->Delete();
    dicomReader = NULL;

    //透明度和颜色信息
    m_colorindexsize = 4;
    m_colorindex = new double[m_colorindexsize];
    m_colorindex[0]=-2048;m_colorindex[1]=-16;
    m_colorindex[2]=641;m_colorindex[3]=3071;
    m_coloropacity = new double[m_colorindexsize];
    m_coloropacity[0]=0;m_coloropacity[1]=0;
    m_coloropacity[2]=0.72;m_coloropacity[3]=0.71;
    m_colorvalue = new double[m_colorindexsize*4];
    m_colorvalue[0]=0;m_colorvalue[1]=0;m_colorvalue[2]=0;
    m_colorvalue[3]=0.73;m_colorvalue[4]=0.25;m_colorvalue[5]=0.3;
    m_colorvalue[6]=0.9;m_colorvalue[7]=0.82;m_colorvalue[8]=0.56;
    m_colorvalue[9]=1;m_colorvalue[10]=1;m_colorvalue[11]=1;
    //窗口信息
    m_winsize[0] = 512;m_winsize[1] = 512;
    //相机信息
    m_focalpos[0] = m_focalpos[1] = m_imagebound[1]/2;
    m_focalpos[2] = m_imagebound[5]/2;
    m_camerapos[0] = m_focalpos[0];
    m_camerapos[1] = m_focalpos[1];
    m_camerapos[2] = m_focalpos[2] + 500;
    m_viewup[0] = 0;m_viewup[1] = 1;m_viewup[2] = 0;
    double foctocam[3] = {m_camerapos[0]-m_focalpos[0],m_camerapos[1]-m_focalpos[1],m_camerapos[2]-m_focalpos[2]};
    double foctocamlen = sqrt(foctocam[0]*foctocam[0]+foctocam[1]*foctocam[1]+foctocam[2]*foctocam[2]);
    foctocam[0]/=foctocamlen;foctocam[1]/=foctocamlen;foctocam[2]/=foctocamlen;
    Cross(m_viewup,foctocam,m_viewleft);//viewup*foctocam
    m_direction[0]=-foctocam[0];m_direction[1]=-foctocam[1];m_direction[2]=-foctocam[2];

    m_parallelScale = (sqrt(m_imagebound[1]*m_imagebound[1]*2+m_imagebound[5]*m_imagebound[5]))/2;
    m_cliprange[0]=0.01;m_cliprange[1]=1000.01;
    m_viewport[0]=m_viewport[1]=0;
    m_viewport[2]=m_viewport[3]=1;
    //初始化矩阵
    m_modelviewMatrix = new double[16];
    m_projectionMatrix = new double[16];
    m_compositeMatrix = new double[16];
    for(int i=0;i<16;i++){
        m_modelviewMatrix[i]=0;
        m_projectionMatrix[i]=0;
        m_compositeMatrix[i]=0;
    }
    initModelviewMatrix(m_viewleft,foctocam);
    initProjecyAndCompositeMatrix();
    //图片信息
    m_imageptr = NULL;
    printinfo();
}

RayTraceInfo::~RayTraceInfo()
{
    if(dicomReader != NULL){
        dicomReader->Delete();
    }
    delete[] m_imagedataptr;
    delete[] m_modelviewMatrix;
    delete[] m_compositeMatrix;
    delete[] m_projectionMatrix;
    delete[] m_colorindex;
    delete[] m_coloropacity;
    delete[] m_colorvalue;
    delete[] m_imageptr;
}

void RayTraceInfo::printinfo()
{
    qDebug()<<"m_dim:"<<m_dim[0]<<m_dim[1]<<m_dim[2];
    qDebug()<<"m_spacing:"<<m_spacing[0]<<m_spacing[1]<<m_spacing[2];
    qDebug()<<"m_scalerange:"<<m_scalerange[0]<<m_scalerange[1];
    qDebug()<<"m_imagebound:"<<m_imagebound[0]<<m_imagebound[1]<<m_imagebound[2]
            <<m_imagebound[3]<<m_imagebound[4]<<m_imagebound[5];
    qDebug()<<"m_focalpos:"<<m_focalpos[0]<<m_focalpos[1]<<m_focalpos[2];
    qDebug()<<"m_camerapos:"<<m_camerapos[0]<<m_camerapos[1]<<m_camerapos[2];
    qDebug()<<"m_direction:"<<m_direction[0]<<m_direction[1]<<m_direction[2];
    qDebug()<<"m_viewup:"<<m_viewup[0]<<m_viewup[1]<<m_viewup[2];
    qDebug()<<"m_parallelScale:"<<m_parallelScale;
}

void RayTraceInfo::updateImage(double direction[3])
{
    double camera2dpos[3];
    Get2DPosBy3DPos(m_camerapos,camera2dpos);

    double elem[16];
    Invert(m_compositeMatrix,elem);


    int nx = m_winsize[0],ny=m_winsize[1];
    if(m_imageptr == NULL){
        m_imageptr = new int[ny*nx*3];
    }else{
        delete[] m_imageptr;
        m_imageptr = new int[ny*nx*3];
    }
    #pragma omp parallel for
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            double ori[3];
            double rgbvalue[3] = {1};
            double iamgepos2d[3] = {0,0,camera2dpos[2]};
            iamgepos2d[0]=i;iamgepos2d[1]=j;
            Get3DPosBy2DPos(iamgepos2d,ori,elem);
            GetShowColor(ori,direction,rgbvalue);
            int ir = rgbvalue[0]*255;
            int ig = rgbvalue[1]*255;
            int ib = rgbvalue[2]*255;
            int imageposindex = ((ny-j-1)*nx+i)*3;
            m_imageptr[imageposindex] = ir;
            m_imageptr[imageposindex+1] = ig;
            m_imageptr[imageposindex+2] = ib;
        }
    }
}

void RayTraceInfo::AzimuthRayInfo(double angle)
{
    double rotatematrix[16] = {0};
    rotatematrix[0]=rotatematrix[5]=rotatematrix[10]=rotatematrix[15]=1;
    Azimuth(angle,m_viewup,m_camerapos,m_focalpos,rotatematrix);
    double foctocam[3];
    foctocam[0] = m_camerapos[0]-m_focalpos[0];
    foctocam[1] = m_camerapos[1]-m_focalpos[1];
    foctocam[2] = m_camerapos[2]-m_focalpos[2];
    double tmplen = sqrt(foctocam[0]*foctocam[0] + foctocam[1]*foctocam[1] + foctocam[2]*foctocam[2]);
    if(tmplen != 0){
        foctocam[0] /= tmplen;foctocam[1] /= tmplen;foctocam[2] /= tmplen;
    }
    Cross(m_viewup,foctocam,m_viewleft);
    m_direction[0]=-foctocam[0];m_direction[1]=-foctocam[1];m_direction[2]=-foctocam[2];

    //updateImageBoundind();

    initModelviewMatrix(m_viewleft,foctocam);
    updateCompositeMatrix();
}

void RayTraceInfo::ElevationRayInfo(double angle)
{
    double rotatematrix[16] = {0};
    rotatematrix[0]=rotatematrix[5]=rotatematrix[10]=rotatematrix[15]=1;
    double elevationaxial[3]={-m_viewleft[0],-m_viewleft[1],-m_viewleft[2]};
    Azimuth(angle,elevationaxial,m_camerapos,m_focalpos,rotatematrix);
    double foctocam[3];
    foctocam[0] = m_camerapos[0]-m_focalpos[0];
    foctocam[1] = m_camerapos[1]-m_focalpos[1];
    foctocam[2] = m_camerapos[2]-m_focalpos[2];
    double tmplen = sqrt(foctocam[0]*foctocam[0] + foctocam[1]*foctocam[1] + foctocam[2]*foctocam[2]);
    if(tmplen != 0){
        foctocam[0] /= tmplen;foctocam[1] /= tmplen;foctocam[2] /= tmplen;
    }
    Cross(foctocam,m_viewleft,m_viewup);
    m_direction[0]=-foctocam[0];m_direction[1]=-foctocam[1];m_direction[2]=-foctocam[2];
    //updateImageBoundind();
    initModelviewMatrix(m_viewleft,foctocam);
    updateCompositeMatrix();
}

void RayTraceInfo::GetCTOpacity(double ctvalue, double &opacity)
{
    int tempctindex = 0;
    for(int i=0;i<m_colorindexsize;i++){
        if(ctvalue<m_colorindex[i]){
            tempctindex = i;
            break;
        }
    }
    if(tempctindex==0){
        opacity=m_coloropacity[0];
        return;
    }
    double tempCIrange = m_colorindex[tempctindex]-m_colorindex[tempctindex-1];
    double tempCTratio = (ctvalue-m_colorindex[tempctindex-1])/tempCIrange;
    int maxCVindex = tempctindex;
    int minCVindex = (tempctindex-1);
    opacity=m_coloropacity[minCVindex]+(m_coloropacity[maxCVindex]-m_coloropacity[minCVindex])*tempCTratio;
}

void RayTraceInfo::GetCTColor(double ctvalue, double resrgb[])
{
    int tempctindex = 0;
    for(int i=0;i<m_colorindexsize;i++){
        if(ctvalue<m_colorindex[i]){
            tempctindex = i;
            break;
        }
    }
    if(tempctindex==0){
        resrgb[0]=m_colorvalue[0];resrgb[1]=m_colorvalue[1];resrgb[2]=m_colorvalue[2];
        return;
    }
    double tempCIrange = m_colorindex[tempctindex]-m_colorindex[tempctindex-1];
    double tempCTratio = (ctvalue-m_colorindex[tempctindex-1])/tempCIrange;
    int maxCVindex = tempctindex*3;
    int minCVindex = (tempctindex-1)*3;
    for(int i=0;i<3;i++){
        resrgb[i] = m_colorvalue[minCVindex]+(m_colorvalue[maxCVindex]-m_colorvalue[minCVindex])*tempCTratio;
        ++maxCVindex;++minCVindex;
    }
}

bool RayTraceInfo::PointInBounding(double point[], double bounding[])
{
    if(point[0]<bounding[0] || point[0]>bounding[1] ||
            point[1]<bounding[2] || point[1]>bounding[3] ||
            point[2]<bounding[4] || point[2]>bounding[5]){
        return 0;
    }
    return 1;
}

void RayTraceInfo::GetShowColor(double ori[], double direction[], double rgbvalue[])
{
    double coord[3]; double t;
    char hit = IntersectBox(m_imagebound,ori,direction,coord,t);
    if(hit)
    {
        int tempdim[3] = {coord[0]/m_spacing[0],coord[1]/m_spacing[1],coord[2]/m_spacing[2]};

        int tupleidx = tempdim[2]*m_dim[1]*m_dim[0]+tempdim[1]*m_dim[0] + tempdim[0];
//        double ctvalue = m_imagearr->GetComponent(tupleidx,0);
        double ctvalue = m_imagedataptr[tupleidx];

        /*double temprgbvalue = ((ctvalue-m_scalerange[0])/(m_scalerange[1]-m_scalerange[0]));
        rgbvalue[0] = temprgbvalue;
        rgbvalue[1] = temprgbvalue;
        rgbvalue[2] = temprgbvalue;*/

        double tempopacity=0.0;
        GetCTOpacity(ctvalue,tempopacity);
        int tvalue = 1;
        double temprgb[3]={0};
        if(tempopacity > myzero){
            GetCTColor(ctvalue,temprgb);
        }
        double allopacity = tempopacity;
        double allcolor[3] = {temprgb[0],temprgb[1],temprgb[2]};
        while(allopacity < 0.95){
            double tempcoord[3] = {(coord[0]+tvalue*direction[0]),
                              (coord[1]+tvalue*direction[1]),
                              (coord[2]+tvalue*direction[2])};
            if(!PointInBounding(tempcoord,m_imagebound)){
                break;
            }
            tempdim[0]=tempcoord[0]/m_spacing[0];
            tempdim[1]=tempcoord[1]/m_spacing[1];
            tempdim[2]=tempcoord[2]/m_spacing[2];
            tupleidx = tempdim[2]*m_dim[1]*m_dim[0]+tempdim[1]*m_dim[0] + tempdim[0];
            //ctvalue = m_imagearr->GetComponent(tupleidx,0);
            ctvalue = m_imagedataptr[tupleidx];
            double tempopacity2=0.0;
            GetCTOpacity(ctvalue,tempopacity2);
            allopacity = 1-(1-tempopacity)*(1-tempopacity2);
            if(allopacity<myzero){
                ++tvalue;
                continue;
            }
            double temprgb2[3]={0};
            GetCTColor(ctvalue,temprgb2);
            allcolor[0] = (tempopacity*temprgb[0]+(1-tempopacity)*tempopacity2*temprgb2[0])/allopacity;
            allcolor[1] = (tempopacity*temprgb[1]+(1-tempopacity)*tempopacity2*temprgb2[1])/allopacity;
            allcolor[2] = (tempopacity*temprgb[2]+(1-tempopacity)*tempopacity2*temprgb2[2])/allopacity;

            tempopacity = allopacity;
            temprgb[0]=allcolor[0];temprgb[1]=allcolor[1];temprgb[2]=allcolor[2];
            ++tvalue;
        }
        rgbvalue[0]=allcolor[0];rgbvalue[1]=allcolor[1];rgbvalue[2]=allcolor[2];
    }else{
        rgbvalue[0]=rgbvalue[1]=rgbvalue[2]=0;
    }
}

void RayTraceInfo::Get2DPosByViewportPos(double view3d[], double pos2d[])
{
    double dx = (view3d[0]+1.0)*(m_winsize[0]*(m_viewport[2]-m_viewport[0]))/2 + m_winsize[0]*m_viewport[0];
    double dy = (view3d[1]+1.0)*(m_winsize[1]*(m_viewport[3]-m_viewport[1]))/2 + m_winsize[1]*m_viewport[1];
    pos2d[0]=(dx);
    pos2d[1]=(dy);
    pos2d[2]=(view3d[2]);
}

void RayTraceInfo::Get2DPosBy3DPos(double vec3d[], double pos2d[])
{
    double viewpoint[3];
    viewpoint[0] = m_compositeMatrix[0]*vec3d[0]+m_compositeMatrix[1]*vec3d[1]
            +m_compositeMatrix[2]*vec3d[2]+m_compositeMatrix[3]*1;
    viewpoint[1] = m_compositeMatrix[4]*vec3d[0]+m_compositeMatrix[5]*vec3d[1]
            +m_compositeMatrix[6]*vec3d[2]+m_compositeMatrix[7]*1;
    viewpoint[2] = m_compositeMatrix[8]*vec3d[0]+m_compositeMatrix[9]*vec3d[1]
            +m_compositeMatrix[10]*vec3d[2]+m_compositeMatrix[11]*1;

    double tempvp[3] ={viewpoint[0],viewpoint[1],viewpoint[2]};
    Get2DPosByViewportPos(tempvp,pos2d);
}

void RayTraceInfo::Get3DPosBy2DPos(double pos2d[], double vec3d[], double elem[])
{
    double vx = 2.0 * (pos2d[0] - m_winsize[0]*m_viewport[0])/
      (m_winsize[0]*(m_viewport[2]-m_viewport[0])) - 1.0;
    double vy = 2.0 * (pos2d[1] - m_winsize[1]*m_viewport[1])/
      (m_winsize[1]*(m_viewport[3]-m_viewport[1])) - 1.0;
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

void RayTraceInfo::initModelviewMatrix(double viewleft[3],double foctocam[3])
{
    m_modelviewMatrix[0]=viewleft[0];m_modelviewMatrix[1]=viewleft[1]; m_modelviewMatrix[2]=viewleft[2]; m_modelviewMatrix[3]=0;
    m_modelviewMatrix[4]=m_viewup[0]; m_modelviewMatrix[5]=m_viewup[1]; m_modelviewMatrix[6]=m_viewup[2]; m_modelviewMatrix[7]=0;
    m_modelviewMatrix[8]=foctocam[0]; m_modelviewMatrix[9]=foctocam[1]; m_modelviewMatrix[10]=foctocam[2];m_modelviewMatrix[11]=0;
    m_modelviewMatrix[12]=0;m_modelviewMatrix[13]=0;m_modelviewMatrix[14]=0;m_modelviewMatrix[15]=1;

    m_modelviewMatrix[3] =  viewleft[0]*(-m_camerapos[0])+viewleft[1]*(-m_camerapos[1])+viewleft[2]*(-m_camerapos[2]);
    m_modelviewMatrix[7] =  m_viewup[0]*(-m_camerapos[0])+m_viewup[1]*(-m_camerapos[1])+m_viewup[2]*(-m_camerapos[2]);
    m_modelviewMatrix[11] = foctocam[0]*(-m_camerapos[0])+foctocam[1]*(-m_camerapos[1])+foctocam[2]*(-m_camerapos[2]);
}

void RayTraceInfo::initProjecyAndCompositeMatrix()
{
    m_projectionMatrix[0]=1; m_projectionMatrix[1]=0; m_projectionMatrix[2]=0;   m_projectionMatrix[3]=0;
    m_projectionMatrix[4]=0; m_projectionMatrix[5]=1; m_projectionMatrix[6]=0;   m_projectionMatrix[7]=0;
    m_projectionMatrix[8]=0; m_projectionMatrix[9]=0; m_projectionMatrix[10]=0.5;m_projectionMatrix[11]=0.5;
    m_projectionMatrix[12]=0;m_projectionMatrix[13]=0;m_projectionMatrix[14]=0;  m_projectionMatrix[15]=1;
    double aspect = m_winsize[0]/m_winsize[1];
    double vwidth = m_parallelScale*aspect;
    double vheight = m_parallelScale;
    m_projectionMatrix[0]=1.0/vwidth;
    m_projectionMatrix[5]=1.0/vheight;
    m_projectionMatrix[10] *= (-2.0/(m_cliprange[1]-m_cliprange[0]));
    m_projectionMatrix[11]= m_projectionMatrix[10]*(-(m_cliprange[1]+m_cliprange[0])/(m_cliprange[1]-m_cliprange[0]))+0.5;
    updateCompositeMatrix();
}

void RayTraceInfo::updateCompositeMatrix()
{
    m_compositeMatrix[0] =  m_projectionMatrix[0]*m_modelviewMatrix[0]+m_projectionMatrix[1]*m_modelviewMatrix[4]
            +m_projectionMatrix[2]*m_modelviewMatrix[8]+m_projectionMatrix[3]*m_modelviewMatrix[12];
    m_compositeMatrix[1] =  m_projectionMatrix[0]*m_modelviewMatrix[1]+m_projectionMatrix[1]*m_modelviewMatrix[5]
            +m_projectionMatrix[2]*m_modelviewMatrix[9]+m_projectionMatrix[3]*m_modelviewMatrix[13];
    m_compositeMatrix[2] =  m_projectionMatrix[0]*m_modelviewMatrix[2]+m_projectionMatrix[1]*m_modelviewMatrix[6]
            +m_projectionMatrix[2]*m_modelviewMatrix[10]+m_projectionMatrix[3]*m_modelviewMatrix[14];
    m_compositeMatrix[3] =  m_projectionMatrix[0]*m_modelviewMatrix[3]+m_projectionMatrix[1]*m_modelviewMatrix[7]
            +m_projectionMatrix[2]*m_modelviewMatrix[11]+m_projectionMatrix[3]*m_modelviewMatrix[15];

    m_compositeMatrix[4] =  m_projectionMatrix[4]*m_modelviewMatrix[0]+m_projectionMatrix[5]*m_modelviewMatrix[4]
            +m_projectionMatrix[6]*m_modelviewMatrix[8]+m_projectionMatrix[7]*m_modelviewMatrix[12];
    m_compositeMatrix[5] =  m_projectionMatrix[4]*m_modelviewMatrix[1]+m_projectionMatrix[5]*m_modelviewMatrix[5]
            +m_projectionMatrix[6]*m_modelviewMatrix[9]+m_projectionMatrix[7]*m_modelviewMatrix[13];
    m_compositeMatrix[6] =  m_projectionMatrix[4]*m_modelviewMatrix[2]+m_projectionMatrix[5]*m_modelviewMatrix[6]
            +m_projectionMatrix[6]*m_modelviewMatrix[10]+m_projectionMatrix[7]*m_modelviewMatrix[14];
    m_compositeMatrix[7] =  m_projectionMatrix[4]*m_modelviewMatrix[3]+m_projectionMatrix[5]*m_modelviewMatrix[7]
            +m_projectionMatrix[6]*m_modelviewMatrix[11]+m_projectionMatrix[7]*m_modelviewMatrix[15];

    m_compositeMatrix[8] =  m_projectionMatrix[8]*m_modelviewMatrix[0]+m_projectionMatrix[9]*m_modelviewMatrix[4]
            +m_projectionMatrix[10]*m_modelviewMatrix[8]+m_projectionMatrix[11]*m_modelviewMatrix[12];
    m_compositeMatrix[9] =  m_projectionMatrix[8]*m_modelviewMatrix[1]+m_projectionMatrix[9]*m_modelviewMatrix[5]
            +m_projectionMatrix[10]*m_modelviewMatrix[9]+m_projectionMatrix[11]*m_modelviewMatrix[13];
    m_compositeMatrix[10] = m_projectionMatrix[8]*m_modelviewMatrix[2]+m_projectionMatrix[9]*m_modelviewMatrix[6]
            +m_projectionMatrix[10]*m_modelviewMatrix[10]+m_projectionMatrix[11]*m_modelviewMatrix[14];
    m_compositeMatrix[11] = m_projectionMatrix[8]*m_modelviewMatrix[3]+m_projectionMatrix[9]*m_modelviewMatrix[7]
            +m_projectionMatrix[10]*m_modelviewMatrix[11]+m_projectionMatrix[11]*m_modelviewMatrix[15];

    m_compositeMatrix[12] = m_projectionMatrix[12]*m_modelviewMatrix[0]+m_projectionMatrix[13]*m_modelviewMatrix[4]
            +m_projectionMatrix[14]*m_modelviewMatrix[8]+m_projectionMatrix[15]*m_modelviewMatrix[12];
    m_compositeMatrix[13] = m_projectionMatrix[12]*m_modelviewMatrix[1]+m_projectionMatrix[13]*m_modelviewMatrix[5]
            +m_projectionMatrix[14]*m_modelviewMatrix[9]+m_projectionMatrix[15]*m_modelviewMatrix[13];
    m_compositeMatrix[14] = m_projectionMatrix[12]*m_modelviewMatrix[2]+m_projectionMatrix[13]*m_modelviewMatrix[6]
            +m_projectionMatrix[14]*m_modelviewMatrix[10]+m_projectionMatrix[15]*m_modelviewMatrix[14];
    m_compositeMatrix[15] = m_projectionMatrix[12]*m_modelviewMatrix[3]+m_projectionMatrix[13]*m_modelviewMatrix[7]
            +m_projectionMatrix[14]*m_modelviewMatrix[11]+m_projectionMatrix[15]*m_modelviewMatrix[15];

}

void RayTraceInfo::updateWinSize()
{
    initProjecyAndCompositeMatrix();
    updateImage(m_direction);
}

void RayTraceInfo::updateImageBoundind()
{
    double a,b,c,d;
    a = m_direction[0];
    b = m_direction[1];
    c = m_direction[2];
    d = -(a*m_camerapos[0] + b*m_camerapos[1] + c*m_camerapos[2]);
    double range[2],dist;
    double* bounds = m_imagebound;
    range[0] = a*bounds[0] + b*bounds[2] + c*bounds[4] + d;
    range[1] = 1e-18;
    int     i, j, k;
    // Find the closest / farthest bounding box vertex
    for ( k = 0; k < 2; k++ )
    {
      for ( j = 0; j < 2; j++ )
      {
        for ( i = 0; i < 2; i++ )
        {
          dist = a*bounds[i] + b*bounds[2+j] + c*bounds[4+k] + d;
          range[0] = (dist<range[0])?(dist):(range[0]);
          range[1] = (dist>range[1])?(dist):(range[1]);
        }
      }
    }
    double minGap = 0.1*m_parallelScale;
    if (range[1] - range[0] < minGap)
    {
      minGap = minGap - range[1] + range[0];
      range[1] += minGap/2.0;
      range[0] -= minGap/2.0;
    }
    if (range[0] < 0.0)
    {
      range[0] = 0.0;
    }
    // Give ourselves a little breathing room
    range[0] = 0.99*range[0] - (range[1] - range[0])*0.5;
    range[1] = 1.01*range[1] + (range[1] - range[0])*0.5;

    // Make sure near is not bigger than far
    range[0] = (range[0] >= range[1])?(0.01*range[1]):(range[0]);

    double clippingtolerance = 0.001;
    if (range[0] < clippingtolerance*range[1])
    {
      range[0] = clippingtolerance*range[1];
    }
    m_cliprange[0]=range[0];
    m_cliprange[1]=range[1];
}

