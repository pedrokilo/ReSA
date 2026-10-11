extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
#pragma intrinsic(sin, cos)
// Separate translation unit: callers retain their existing compiler settings.
// Acceptance requires emitted FSIN/FCOS and no intervening float32 store.
__declspec(noinline) float __cdecl CloudsSinX87(float angle) { return float(sin(double(angle))); }
__declspec(noinline) float __cdecl CloudsCosX87(float angle) { return float(cos(double(angle))); }
__declspec(noinline) float __cdecl CloudsSinMulX87(float angle, float k) { return float(sin(double(angle)) * double(k)); }
