extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
#pragma intrinsic(sin,cos)
#define FN __declspec(noinline) float __cdecl
FN N1(float a,float d){ double t=sin(double(a)); t=-t; return float(t/d); }
FN N2(float a,float d){ return float(sin(double(a)) / -double(d)); }
FN N3(float a,float d){ double t=-sin(double(a)); return float(t * (1.0/d)); }
FN C1(float a,float h){ double t=cos(double(a)); t*=h; t+=h; return float(t); }
FN C2(float a,float h){ return float(double(h)+cos(double(a))*double(h)); }
FN S1(float z,float a){ double t=sin(double(a)); return float(z-t); }
FN S2(float z,float a){ return float(-(sin(double(a))-z)); }
#pragma float_control(precise,on,push)
FN NPrec(float a,float d){return float(-sin(double(a))/d);}
FN CPrec(float a,float h){double t=cos(double(a));t*=h;t+=h;return float(t);}
FN SPrec(float z,float a){return float(z-sin(double(a)));}
#pragma float_control(pop)

FN CV1(float a,volatile float h){ double t=cos(double(a)); t*=double(h); t+=double(h); return float(t); }
FN CV2(float a,volatile float h){ return float(cos(double(a))*double(h)+double(h)); }
FN CV3(float a,float h){const volatile float* p=&h;double t=cos(double(a));t*=*p;t+=*p;return float(t);}
