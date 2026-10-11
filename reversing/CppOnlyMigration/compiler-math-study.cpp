extern "C" double __cdecl sqrt(double);
extern "C" float __cdecl sqrtf(float);
extern "C" double __cdecl sin(double);
#pragma intrinsic(sqrt, sqrtf, sin)
float root_float(float x) { return sqrtf(x); }
float root_double(float x) { return float(sqrt(x)); }
float sine(float x) { return float(sin(x)); }
