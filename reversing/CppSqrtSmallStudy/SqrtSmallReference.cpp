// Isolated instrumentation reference copied from current helpers, not production.
struct Vec3 {float x,y,z;};
extern "C" __declspec(noinline) double ReferenceShadowExtendedSqrt(float value) {
    double result;
    __asm { fld value }
    __asm { fsqrt }
    __asm { fstp result }
    return result;
}
extern "C" __declspec(noinline) double ReferenceWaterNearestDistance(double x,double y) {
    double result;
    __asm {
        fld y
        fmul st,st
        fld x
        fmul st,st
        faddp st(1),st
        fsqrt
        fstp result
    }
    return result;
}
extern "C" __declspec(noinline) float ReferenceCoverDistance2D(const Vec3* a,const Vec3* b) {
    const float firstX=a->x,firstY=a->y,secondX=b->x,secondY=b->y;
    float result;
    __asm {
        fld firstX
        fsub secondX
        fld firstY
        fsub secondY
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fsqrt
        fstp st(2)
        fstp st(0)
        fstp result
    }
    return result;
}
