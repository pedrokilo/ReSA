extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
#pragma intrinsic(sin, cos)
// Separate translation unit: callers retain their existing compiler settings.
// Acceptance requires emitted FSIN/FCOS and no intervening float32 store.
__declspec(noinline) float __cdecl CloudsSinX87(float angle) { return float(sin(double(angle))); }
__declspec(noinline) float __cdecl CloudsCosX87(float angle) { return float(cos(double(angle))); }
__declspec(noinline) float __cdecl CloudsSinMulX87(float angle, float k) { return float(sin(double(angle)) * double(k)); }

// Keep comparisons ordered and let the compiler emit x87 condition codes.
// The emitted comparison uses second against first; verify each object build.
#pragma float_control(precise, on, push)
__declspec(noinline) bool __cdecl CloudsEqualOrderedCpp(float first, float second) {
    return first <= second && first >= second;
}
__declspec(noinline) bool __cdecl CloudsLessOrderedCpp(float first, float second) {
    return first < second;
}
#pragma float_control(pop)
