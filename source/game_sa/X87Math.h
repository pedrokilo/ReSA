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

// Lote de nueve archivos (11/10/2026).
// v *= pow(damp, step) con la precisión completa de ST0 en las tres multiplicaciones (CBouncingPanel::ProcessPanel, 0x6F49A0).
// pow(base, exponente) con los resultados del _CIpow del EXE (0x822130). Su rama SSE2 es el mismo algoritmo y las mismas tablas que
// __CIpow de la CRT de la DLL (comprobado byte a byte), y sólo difiere en las reglas de C99 para ±1 elevado a NaN/±infinito y NaN
// elevado a ±0, donde el EXE devuelve NaN y la DLL 1.
double __cdecl ExeCompatiblePowCpp(double base, double exponent);
void __cdecl PowScaleVector3Cpp(float* vector, float damp, float step);
// (tiempo sin signo) · 0.001 con fild + corrección de 2^32 y fmul, guardado como float (CCutsceneMgr::FinishCutscene, 0x5B04D0).
float __cdecl UnsignedMillisToSecondsCpp(unsigned int millis);
// double · 65535.0 redondeado por la x87 y guardado como float (CGamma::SetGamma, 0x747200).
float __cdecl GammaScaleCpp(double value);
// acos del CRT del EXE (_CIacos, 0x822380): palabra de control (anterior & 0x300) | 0x7F, fpatan(sqrt((1 + x)(1 - x)), x).
float __cdecl ExeAcosCpp(float x);

// Lote de diez archivos (12/10/2026).
// atan2(num, den) - sub con el fpatan sin redondear y un solo redondeo a float (CGameLogic: GetHeading 0x441DB0 menos pi/2; CRenderer).
float __cdecl Atan2MinusCpp(float num, float den, float sub);
// (coordenada +/- radio) · 0.02 + 60 guardado como double (CInterestingEvents::ScanForNearbyEntities, 0x605A30); useEnd suma el radio.
double __cdecl InterestingSectorCpp(float coordinate, float radius, bool useEnd);
// sin(i · k) con fild del entero y un solo redondeo a float (CMaths::InitMathsTables, 0x59AC90).
float __cdecl TrigIntSinMulCpp(int i, float k);
// n · f con fild del entero y un solo redondeo a float (CCustomCarEnvMapPipeline).
float __cdecl IntTimesFloatCpp(int n, float f);
// Salud restante = max(prevHP - factor, minimumHP) sin redondear antes del daño; daño = prevHP - restante (CPedDamageResponseCalculator::ComputeWillKillPed, 0x4B3290).
void __cdecl FallRemainingHealthCpp(float prevHP, float factor, float minimumHP, float* remaining, float* damage);
// Suma 1 / (2 · ancho) a las u y 1 / (2 · alto) a las v de los cuatro vértices RwIm2DVertex (28 bytes) (CSprite2d::OffsetTexCoordForBilinearFiltering, 0x728150).
void __cdecl SpriteBilinearOffsetCpp(float* vertices, float width, float height);
// Esquinas C y D de un área angulada (CRunningScript, 0x487F60 y similares): ángulo + pi/2 normalizado a [0, 2pi]; corners = {cx, cy, dx, dy}.
void __cdecl ScriptAngledCornersCpp(float angle, float startX, float startY, float endX, float endY, float width, float* corners);

// Lote de trece archivos de tareas (12/10/2026).
// atan2(y, x) con fpatan a precisión completa; el llamador guarda el float (varias tareas de ped).
float __cdecl TrigAtan2Cpp(float y, float x);
// Distancia al cuadrado entre dos vectores (x, y, z consecutivos) sumada como (dz² + dx²) + dy² sin redondear (CTaskAllocatorKillThreatsBasic, 0x69C850).
float __cdecl DistanceSquaredZXYCpp(const float* a, const float* b);
// speed = pow(0.96, paso) · speed; shift += speed · paso (CTaskSimpleCarJumpOut::ProcessPed), con el factor en ST0 sin redondear.
void __cdecl PowDecaySpeedCpp(float* speed, float* shift, float step);
// num / den con fild + fidiv y un solo redondeo a float (CTaskSimpleIKChain).
float __cdecl IntRatioCpp(int num, int den);
