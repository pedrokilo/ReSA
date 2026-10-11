extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)
__declspec(noinline) double addition(double a,double b){volatile double x=a+b;return x;}
__declspec(noinline) double subtraction(double a,double b){volatile double x=a-b;return x;}
__declspec(noinline) double multiplication(double a,double b){volatile double x=a*b;return x;}
__declspec(noinline) double root(double a){return ::sqrt(a);}
__declspec(noinline) bool ordered_less(double a,double b){return a<b;}
