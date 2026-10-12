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

// Audio: cada función devuelve un float guardado y recargado, como los helpers originales (fstp + fld).
// 20 · log10(x) con fldlg2/fyl2x a precisión interna completa y un solo redondeo (CAECollisionAudioEntity, CAEWeaponAudioEntity).
float __cdecl AudioLog10Times20Cpp(float x);
// 20 · log10(effects / music) - 9 (CAERadioTrackManager::UpdateRadioVolumes, 0x4EA010).
float __cdecl AudioRadioVolumeDbCpp(float effects, float music);
// (suma de 64 bits sin signo) · 0.0001, cargada en x87 como 63 bits bajos más el bit alto (CAEWMADecoder::GetStreamPlayTimeMs, 0x502AF0).
float __cdecl AudioSumToMsCpp(unsigned long long sum);
// producto entero · 0.01 con fild + fmul (CAESoundManager::Service, 0x4F0000).
float __cdecl AudioPercentOfProductCpp(int product);

// log10(p) con fldlg2/fyl2x y un solo redondeo a float (CAEAudioUtility::AudioLog10, 0x4D9E50; el umbral lo comprueba el llamador).
float __cdecl AudioLog10Cpp(float p);
// Cuadrado de la magnitud de un vector sumado como el EXE (CVector::SquaredMagnitude, 0x406DA0): (z·z + y·y) + x·x sin
// redondear, comparado con "mayor que" (con NaN da false). Los punteros apuntan a x, y, z consecutivos.
bool __cdecl VectorSquaredMagnitudeAboveCpp(const float* vector, float limit);
