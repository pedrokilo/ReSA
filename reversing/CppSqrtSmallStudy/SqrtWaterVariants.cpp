extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)
extern "C" __declspec(noinline) double WaterVolatileParameters(volatile double x,volatile double y) {
    const double yy=y,xx=x;
    volatile double result=sqrt(yy*yy+xx*xx);
    return result;
}
extern "C" __declspec(noinline) double WaterAlign8(double x,double y) {
    alignas(8) volatile double result=sqrt(y*y+x*x);
    return result;
}
#pragma optimize("g",off)
extern "C" __declspec(noinline) double WaterNoGlobalOptimization(double x,double y) {
    volatile double result=sqrt(y*y+x*x);
    return result;
}
#pragma optimize("",on)
