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
__declspec(noinline) float __cdecl TrigCompoundNegSinDiv(float angle, float divisor) {
    return float((-sin(double(angle))) / double(divisor));
}
// Prevent factoring half*(cos(angle)+1), pending intrinsic/codegen verification.
#pragma float_control(precise, on, push)
__declspec(noinline) float __cdecl TrigCompoundCosHalf(float angle, float half) {
    return float(cos(double(angle)) * double(half) + double(half));
}
#pragma float_control(pop)
__declspec(noinline) float __cdecl TrigCompoundSubSin(float z, float angle) {
    return float(double(z) - sin(double(angle)));
}
