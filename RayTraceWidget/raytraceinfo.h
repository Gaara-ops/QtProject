#ifndef RAYTRACEINFO_H
#define RAYTRACEINFO_H

class vtkDataArray;
class vtkDICOMImageReader;

class RayTraceInfo
{
public:
    RayTraceInfo();
    ~RayTraceInfo();
    void printinfo();
    //更新图片
    void updateImage(double direction[3]);
    //对相机进行操作
    void AzimuthRayInfo(double angle);
    void ElevationRayInfo(double angle);

    //获取透明度
    void GetCTOpacity(double ctvalue,double& opacity);
    //获取颜色
    void GetCTColor(double ctvalue,double resrgb[3]);
    //点是否在包围盒内
    bool PointInBounding(double point[3],double bounding[6]);
    //获取对应点应显示的颜色
    void GetShowColor(double ori[3],double direction[3],double rgbvalue[3]);

    //viewport窗口坐标转窗口坐标
    void Get2DPosByViewportPos(double view3d[3],double pos2d[3]);
    //三维坐标转二维坐标
    void Get2DPosBy3DPos(double vec3d[3],double pos2d[3]);
    //二维坐标转三维坐标
    void Get3DPosBy2DPos(double pos2d[3], double vec3d[3], double elem[16]);


    //初始化矩阵
    void initModelviewMatrix(double viewleft[3],double foctocam[3]);
    void initProjecyAndCompositeMatrix();
    void updateCompositeMatrix();
    void updateWinSize();

    void updateImageBoundind();//暂不使用
public:
    //体信息
    vtkDICOMImageReader* dicomReader;
    int m_dim[3];
    double m_spacing[3];
    vtkDataArray* m_imagearr;
    double* m_imagedataptr;
    double m_scalerange[2];
    double m_imagebound[6];
    //相机相关信息
    double m_camerapos[3];
    double m_focalpos[3];
    double m_viewup[3];
    double m_direction[3];
    double m_viewleft[3];

    double m_cliprange[2];
    int m_viewport[4];
    double m_parallelScale;

    double *m_modelviewMatrix;
    double *m_projectionMatrix;
    double *m_compositeMatrix;
    //窗口信息
    double m_winsize[2];
    //透明度和颜色信息
    int m_colorindexsize;
    double *m_colorvalue;
    double *m_colorindex;
    double *m_coloropacity;
    //图片信息
    int* m_imageptr;
};

#endif // RAYTRACEINFO_H
