extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)
struct Vec3 {float x,y,z;};
extern "C" __declspec(noinline) double SqrtFloatRounded64(float x) {
    volatile double result=sqrt(double(x));
    return result;
}
extern "C" __declspec(noinline) double WaterHypotXYRounded64(double x,double y) {
    volatile double result=sqrt(y*y+x*x);
    return result;
}
extern "C" __declspec(noinline) float CoverDistance2DRounded32(const Vec3* a,const Vec3* b) {
    const double dx=double(a->x)-b->x,dy=double(a->y)-b->y;
    volatile float result=float(sqrt(dy*dy+dx*dx));
    return result;
}
