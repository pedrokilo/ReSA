#pragma once
// Helpers x87 compartidos. Cada función conserva la secuencia de operaciones del EXE
// (sin guardados intermedios de float). Se compilan con /O2 /Oi /arch:IA32 /fp:fast (ver source/CMakeLists.txt).

// Seno y coseno: float(sin(double(angle)) [* k])
float __cdecl TrigSimpleSinCpp(float angle);
float __cdecl TrigSimpleCosCpp(float angle);
float __cdecl TrigSimpleSinMulCpp(float angle, float k);
float __cdecl TrigSimpleCosMulCpp(float angle, float k);

// Combinaciones de seno/coseno con producto y suma
float __cdecl TrigCompoundScaledSinMulAdd(float angle, float k, float mult, float add);
float __cdecl TrigCompoundSinMulAdd(float angle, float mult, float add);
float __cdecl TrigCompoundCosMulAdd(float angle, float mult, float add);
float __cdecl TrigCompoundNegSinDiv(float angle, float divisor);
float __cdecl TrigCompoundCosHalf(float angle, float half);
float __cdecl TrigCompoundSubSin(float z, float angle);

// Comparaciones ordenadas (false con NaN) que actualizan C0/C3 como el EXE
bool __cdecl CloudsEqualOrderedCpp(float first, float second);
bool __cdecl CloudsLessOrderedCpp(float first, float second);

// Sombras: raíz cuadrada y atenuación por distancia, devueltas como double (FSTP64 + FLD64)
extern "C" double __cdecl ShadowSqrtCpp(float value);
extern "C" double __cdecl ShadowDistanceFadeCpp(float squared, float distance);

// Animación: producto escalar de dos quaterniones en el orden del EXE ((w·w + z·z) + y·y) + x·x
// (los punteros apuntan a x, y, z, w consecutivos).
// AnimQuaternionDotNegativeCpp: true si el producto es < 0 (comparación ordenada: con NaN da false).
bool __cdecl AnimQuaternionDotNegativeCpp(const float* last, const float* cur);
// AnimCalcThetaCpp: theta = acos(producto recortado a 1) con la palabra de control del EXE y 1/sin(theta) sin redondear
// (0 si theta es exactamente 0), como CAnimBlendNode::CalcTheta (0x4D00E0).
void __cdecl AnimCalcThetaCpp(const float* prev, const float* curr, float* theta, float* invSinTheta);
