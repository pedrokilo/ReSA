extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
#pragma intrinsic(sin, cos)
// Offline expressions: not accepted until actual x87 sequence is inspected.
__declspec(noinline) float __cdecl TrigCompoundScaledSinMulAdd(float angle, float k, float mult, float add) {
    return float(sin(double(angle) * double(k)) * double(mult) + double(add));
}
__declspec(noinline) float __cdecl TrigCompoundSinMulAdd(float angle, float mult, float add) {
    return float(sin(double(angle)) * double(mult) + double(add));
}
__declspec(noinline) float __cdecl TrigCompoundCosMulAdd(float angle, float mult, float add) {
    return float(cos(double(angle)) * double(mult) + double(add));
}
