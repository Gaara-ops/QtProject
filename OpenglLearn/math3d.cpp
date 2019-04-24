#include "math3d.h"


Vector3f Vector3f::Cross(const Vector3f &v) const
{

    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vector3f(_x, _y, _z);
}

Vector3f &Vector3f::Normalize()
{
    const float Length = sqrtf(x * x + y * y + z * z);

    x /= Length;
    y /= Length;
    z /= Length;

    return *this;
}

void Vector3f::Rotate(float Angle, const Vector3f &Axe)
{
    const float SinHalfAngle = sinf(ToRadian(Angle/2));
    const float CosHalfAngle = cosf(ToRadian(Angle/2));

    const float Rx = Axe.x * SinHalfAngle;
    const float Ry = Axe.y * SinHalfAngle;
    const float Rz = Axe.z * SinHalfAngle;
    const float Rw = CosHalfAngle;
    Quaternion RotationQ(Rx, Ry, Rz, Rw);

    Quaternion ConjugateQ = RotationQ.Conjugate();
  //  ConjugateQ.Normalize();
    Quaternion W = RotationQ * (*this) * ConjugateQ;

    x = W.x;
    y = W.y;
    z = W.z;
}


Vector3f Quaternion::ToDegrees()
{
    float f[3];

    f[0] = atan2(x * z + y * w, x * w - y * z);
    f[1] = acos(-x * x - y * y - z * z - w * w);
    f[2] = atan2(x * z - y * w, x * w + y * z);

    f[0] = ToDegree(f[0]);
    f[1] = ToDegree(f[1]);
    f[2] = ToDegree(f[2]);

    return Vector3f(f);
}


void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
    Matrix4f rx, ry, rz;

    const float x = ToRadian(RotateX);
    const float y = ToRadian(RotateY);
    const float z = ToRadian(RotateZ);

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    *this = rz * ry * rx;
}

void Matrix4f::InitCameraTransform(const Vector3f &Target, const Vector3f &Up)
{
    Vector3f N = Target;
    N.Normalize();
    Vector3f U = Up;
    U = U.Cross(N);
    U.Normalize();
    Vector3f V = N.Cross(U);

    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

Quaternion operator*(const Quaternion& l, const Quaternion& r)
{
    const float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
    const float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
    const float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
    const float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

    Quaternion ret(x, y, z, w);

    return ret;
}

Quaternion operator*(const Quaternion& q, const Vector3f& v)
{
    const float w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
    const float x =   (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
    const float y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
    const float z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

    Quaternion ret(x, y, z, w);

    return ret;
}
