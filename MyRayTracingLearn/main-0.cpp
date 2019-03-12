#include "mainwindow.h"
#include <QApplication>

#include <iostream>
#include <fstream>
#include <random>
#include "vec3.h"
//射线类
class Ray{
public:
    Ray(){}
    Ray(const vec3& ori,const vec3& dir){
        m_ori = ori;m_dir = dir;
    }
    vec3 PointT(double t){
        return m_ori+t*m_dir;
    }
    vec3 m_ori;//起点
    vec3 m_dir;//方向
};
class material;
//交点信息
struct hit_point{
    double t;
    vec3 p;
    vec3 normal;
    material* pmat;
};
//所有物体的父类
class hitable{
public:
    virtual bool hit(const Ray& r,double t_min,double t_max,hit_point& hp) const = 0;
};
//所有物体集合
class hitable_list:public hitable{
public:
    hitable_list(){}
    hitable_list(hitable** l,int n){
        list = l;list_size = n;
    }
    virtual bool hit(const Ray& r,double tmin,double tmax,hit_point& hp) const;

    hitable** list;
    int list_size;
};
bool hitable_list::hit(const Ray& r,double tmin,double tmax,hit_point& hp) const{
    hit_point temphp;
    bool hit_any = false;
    double closest_so_far = tmax;
    for(int i=0; i<list_size;i++){
        if(list[i]->hit(r,tmin,closest_so_far,temphp)){
            hit_any = true;
            closest_so_far = temphp.t;
            hp = temphp;
        }
    }
    return hit_any;
}
//球
class sphere:public hitable{
public:
    sphere(){}
    sphere(vec3 cen,double r,material* m):center(cen),radius(r),pmat(m){}
    virtual bool hit(const Ray &r, double t_min, double t_max, hit_point &hp) const;
    vec3 center;
    double radius;
    material* pmat;
};
bool sphere::hit(const Ray &r, double t_min, double t_max, hit_point &hp) const{
    vec3 oc = r.m_ori - center;
    double a = dot(r.m_dir,r.m_dir);
    double b = dot(oc,r.m_dir);
    double c = dot(oc,oc)-radius*radius;
    double dd = b*b-a*c;
    if(dd>0){
        double temp = (-b - sqrt(dd))/a;
        if(temp<t_max && temp>t_min){
            hp.t = temp;
            hp.p = r.m_ori+temp*r.m_dir;
            hp.normal = (hp.p-center)/radius;
            hp.pmat = pmat;
            return true;
        }
        temp = (-b + sqrt(dd))/a;
        if(temp<t_max && temp>t_min){
            hp.t = temp;
            hp.p = r.m_ori+temp*r.m_dir;
            hp.normal = (hp.p-center)/radius;
            hp.pmat = pmat;
            return true;
        }
    }
    return false;
}
///材质
class material{
public:
    virtual bool scatter(const Ray& r_in,const hit_point& hp,vec3& attenuation,Ray& scattered) const =0;
};
//反射
vec3 reflect(const vec3& v,const vec3& n){
    return v-2*dot(v,n)*n;
}
//菲涅尔反射
double schlick(double cosine,double ref_idx){
    double r0=(1-ref_idx)/(1+ref_idx);
    r0=r0*r0;
    return r0+(1-r0)*pow((1-cosine),5);
}
//折射
bool refract(const vec3& v,const vec3& n,double ni_over_nt,vec3& refracted){
    vec3 uv = unit_vector(v);
    double dt = dot(uv,n);
    double discriminant = 1.0-ni_over_nt*ni_over_nt*(1-dt*dt);
    if(discriminant>0){
        refracted = ni_over_nt*(uv-n*dt)-n*sqrt(discriminant);
        return true;
    }else{
        return false;
    }
}
//透明折射
class dielectric:public material{
public:
    double ref_idx;
    dielectric(double ri):ref_idx(ri){}
    virtual bool scatter(const Ray &r_in, const hit_point &hp, vec3 &attenuation, Ray &scattered) const{
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.m_dir,hp.normal);
        double ni_over_nt;
        attenuation = vec3(1.0,1.0,1.0);
        vec3 refracted;
        double reflect_prob;
        double cosine;
        if(dot(r_in.m_dir,hp.normal)>0){
            outward_normal = -hp.normal;
            ni_over_nt = ref_idx;
            cosine = dot(r_in.m_dir,hp.normal)/r_in.m_dir.length();
            cosine = sqrt(1-ref_idx*ref_idx*(1-cosine*cosine));
        }else{
            outward_normal = hp.normal;
            ni_over_nt = 1.0/ref_idx;
            cosine = -dot(r_in.m_dir,hp.normal)/r_in.m_dir.length();
        }
        if(refract(r_in.m_dir,outward_normal,ni_over_nt,refracted)){
            reflect_prob = schlick(cosine,ref_idx);
        }else{
            reflect_prob = 1.0;
        }
        if((rand()%100)/(float)100 < reflect_prob)
            scattered = Ray(hp.p,reflected);
        else
            scattered = Ray(hp.p,refracted);
    }
};


vec3 randomVec(){
    vec3 p;
    do{
        p=2.0f*vec3((rand()%100)/float(100),
                (rand()%100)/float(100),
                (rand()%100)/float(100))- vec3(1.0f, 1.0f, 1.0f);
    }while (p.squared_length()>=1.0);
    return p;
}

//漫反射材质
class lambertian:public material{
public:
    lambertian(const vec3& a):albedo(a){}
    virtual bool scatter(const Ray &r_in, const hit_point &hp, vec3 &attenuation, Ray &scattered) const{
        vec3 target = hp.p+hp.normal+randomVec();
        scattered = Ray(hp.p,target-hp.p);
        attenuation = albedo;
        return true;
    }
    vec3 albedo;
};
//镜面反射材质
class metal:public material{
public:
    metal(const vec3& a,double f):albedo(a){
        if(f<1)
            fuzz = f;
        else
            fuzz = 1;
    }
    virtual bool scatter(const Ray &r_in, const hit_point &hp, vec3 &attenuation, Ray &scattered) const{
        const vec3 tempu = unit_vector(r_in.m_dir);
        vec3 reflect11 = reflect(tempu,hp.normal);
        scattered = Ray(hp.p,reflect11+fuzz*randomVec());
        attenuation = albedo;
        return (dot(scattered.m_dir,hp.normal)>0);
    }
    vec3 albedo;
    double fuzz;
};
#define M_PI 3.14159265358979323846
//相机类
vec3 random_in_unit_disk(){
    vec3 p;
    do{
        p=2.0*vec3((rand()%(100)/(float)(100)),(rand()%(100)/(float)(100)),0)-vec3(1,1,0);
    }while(dot(p,p) >= 1.0);
    return p;
}

class camera{
public:
    camera(){
        lower_left_corner = vec3(-2.0f,-1.0f,-1.0f);
        horizontal = vec3(4.0f,0.0f,0.0f);
        vertical = vec3(0.0f,2.0f,0.0f);
        origin = vec3(0.0f,0.0f,0.0f);
    }
    camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect){

        float theta = vfov*M_PI/180;
        float half_height = tan(theta/2);
        float half_width = aspect*half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom-lookat);
        u = unit_vector(cross(vup,w));
        v = cross(w,u);

        lower_left_corner = origin-half_width*u - half_height*v - w;
        horizontal = 2*half_width*u;
        vertical = 2*half_height*v;
    }
    camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect,float aperture,float focus_dist){
        lens_radius = aperture/2;
        float theta = vfov*M_PI/180;
        float half_height = tan(theta/2);
        float half_width = aspect*half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom-lookat);
        u = unit_vector(cross(vup,w));
        v = cross(w,u);

        lower_left_corner = origin-half_width*focus_dist*u - half_height*focus_dist*v - w*focus_dist;
        horizontal = 2*half_width*focus_dist*u;
        vertical = 2*half_height*focus_dist*v;
    }

    Ray getray(float s,float t){
//        return Ray(origin,lower_left_corner+u*horizontal+v*vertical-origin);
        vec3 rd = lens_radius*random_in_unit_disk();
        vec3 offset = u*rd.x() + v*rd.y();
        return Ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u,v,w;
    float lens_radius;
};

vec3 Color(const Ray& r,hitable* world,int depth){
    hit_point hp;
    if(world->hit(r,0.0001f,10000.0f,hp)){
        Ray scattered;
        vec3 attenuation;
        if(depth<50 && hp.pmat->scatter(r,hp,attenuation,scattered)){
            return attenuation*Color(scattered,world,depth+1);
        }else{
            return vec3(0.0f,0.0f,0.0f);
        }
    }else{
        vec3 unit_dir = unit_vector(r.m_dir);
        double t = 0.5f*(unit_dir.y()+1.0f);
        return (1.0f-t)*vec3(1.0f,1.0f,1.0f)+t*vec3(0.5f,0.7f,1.0f);
    }
}
hitable *RandomScene(){
    int n=500;
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0),1000,new lambertian(vec3(0.5,0.5,0.5)));
    int i=1;
    for(int a=-11;a<11;a++){
        for(int b=-11;b<11;b++){
            float choose_mat = (rand()%(100))/(float)(100);
            vec3 center(a + 0.9*(rand() % (100) / (float)(100)), 0.2,b + 0.9*(rand() % (100) / (float)(100)));
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                /*避免小球的位置和最前面的大球的位置太靠近*/
                if (choose_mat < 0.8) {
                    //diffuse
                    /*材料阀值小于0.8，则设置为漫反射球，漫反射球的衰减系数x,y,z都是（0，1）之间的随机数的平方*/
                    list[i++] = new sphere(center, 0.2,
                        new lambertian(vec3(
                        (rand() % (100) / (float)(100))*(rand() % (100) / (float)(100)),
                            (rand() % (100) / (float)(100))*(rand() % (100) / (float)(100)),
                            (rand() % (100) / (float)(100))*(rand() % (100) / (float)(100)))));
                }
                else if (choose_mat < 0.95) {
                    /*材料阀值大于等于0.8小于0.95，则设置为镜面反射球，镜面反射球的衰减系数x,y,z及模糊系数都是（0，1）之间的随机数加一再除以2*/
                    list[i++] = new sphere(center, 0.2,
                        new metal(vec3(0.5*(1 + (rand() % (100) / (float)(100))),
                            0.5*(1 + (rand() % (100) / (float)(100))),
                            0.5*(1 + (rand() % (100) / (float)(100)))),
                            0.5*(1 + (rand() % (100) / (float)(100)))));
                }
                else {
                    /*材料阀值大于等于0.95，则设置为介质球*/
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    /*定义三个大球*/
    return new hitable_list(list, i);
}

int main(int argc, char *argv[])
{
    std::ofstream imagefile;
    imagefile.open("./imagetrace.ppm");

    //窗口大小
    int width = 2000,height = 1000;
    //每个像素的采样次数
    int samplingnum = 100;

    imagefile<<"P3\n" << width << " " << height << "\n255\n";

    /*hitable *list[5];
    list[0] = new sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
    list[1] = new sphere(vec3(0.0f, -100.5f, -1.0f), 100.0f, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
    list[2] = new sphere(vec3(1.0f, 0.0f, -1.0f), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f));
    list[3] = new sphere(vec3(-1.0f, 0.0f, -1.0f), 0.5f, new dielectric(1.5f));
    list[4] = new sphere(vec3(-1.0f, 0.0f, -1.0f), 0.5f, new dielectric(1.5f));
    hitable *world = new hitable_list(list, 5);

//    camera cam(vec3(-2.0f,2.0f,1.0f), vec3(0.0f,0.0f,-1.0f), vec3(0.0f,1.0f,0.0f), 60, float(width)/float(height));
//    camera cam(vec3(0.0f,0.0f,5.0f), vec3(0.0f,0.0f,-1.0f), vec3(0.0f,1.0f,0.0f), 60, float(width)/float(height));
*/
    hitable *world = RandomScene();

    vec3 lookform(13.0f, 2.0f, 3.0f);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = (lookform - lookat).length();
    float aperture = 0.0f;

    camera cam(lookform, lookat, vec3(0, 1, 0), 20, float(width) / float(height), aperture, 0.7*dist_to_focus);

    std::default_random_engine reng;
    std::uniform_real_distribution<double> uni_dist(0.0f,1.0f);

    for(int j=height-1;j>=0;j--){
        for(int i=0;i<width;i++){
            vec3 col(0.0f,0.0f,0.0f);
            for(int s=0;s<samplingnum;s++){
                double u=double(i+uni_dist(reng))/(double)(width);
                double v=double(j+uni_dist(reng))/(double)(height);
                Ray r = cam.getray(u,v);
                col += Color(r,world,0);
            }
            col /=(double)(samplingnum);
            col = vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
            int ir = int(256*col[0]);
            int ig = int(256*col[1]);
            int ib = int(256*col[2]);
            imagefile << ir << " " <<ig<<" "<<ib<<"\n";
        }
    }
    imagefile.close();

    return 0;
}
