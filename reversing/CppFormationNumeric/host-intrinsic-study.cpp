#include <cmath>
#pragma intrinsic(sqrt)
__declspec(noinline) double add(double a,double b){volatile double x=a+b;return x;}
__declspec(noinline) double sub(double a,double b){volatile double x=a-b;return x;}
__declspec(noinline) double mul(double a,double b){volatile double x=a*b;return x;}
__declspec(noinline) double root(double a){return ::sqrt(a);}
__declspec(noinline) bool ordered_less(double a,double b){return a<b;}
__declspec(noinline) float stored_single(double a){volatile float out=static_cast<float>(a);return out;}
