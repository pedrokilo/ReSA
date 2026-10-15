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

// Lote de dieciséis archivos (12/10/2026), fase A: fragmentos x87 pequeños.
// acos del CRT del EXE sobre un double (_CIacos, 0x822394): el resultado queda sin redondear (CBreakObject).
double __cdecl ExeAcosDoubleCpp(double x);
// asin del CRT del EXE (_CIasin, 0x821E70) por 57.29578f (0x859878), sin redondear (CBulletInfo::Update).
float __cdecl ExeAsinDegreesCpp(float x);
// (entero sin signo) + delta con fild, corrección de 2^32 y un solo redondeo (CClouds::Update, 0x71303C).
float __cdecl UnsignedPlusFloatCpp(int value, float delta);
// f + entero con signo (fld + fiadd) guardado como float (CWanted::RegisterCrime, 0x561E10).
float __cdecl FloatPlusIntCpp(float f, int i);
// n · a · b con fild y dos fmul, un solo redondeo a float (CWeaponInfo::GetTargetHeadRange, 0x743D50).
float __cdecl IntTimesTwoFloatsCpp(int n, float a, float b);
// CCurves::CalcCorrectedDist (0x43C880): distancia corregida y t; devuelve el valor sin redondear en ST0.
float __cdecl CurvesCorrectedDistCpp(float current, float total, float speedVariation, float* outT);
// Radio de la esfera del frustum de FxManager_c::CalcFrustumInfo (0x4A9165..0x4A91B5).
float __cdecl FxFrustumRadiusCpp(float dist, float farClip);

// Residuo de registros del EXE (lote de 12/10/2026, parte 2). Estas dos funciones son lo último que ejecuta la función que se reproduce:
// reciben los valores en los registros que hay que dejar (la convención __fastcall/__thiscall los pone en ECX/EDX, y su cuerpo no
// los toca), devuelven EAX y salen, así que la función llamadora termina con ECX, EDX y EAX exactamente como el EXE.
// ECX = ecxValue, EDX = edxValue, EAX = eaxValue.
unsigned int __fastcall ResidueRegsCpp(unsigned int ecxValue, unsigned int edxValue, unsigned int eaxValue);
// ECX = this, EAX = eaxValue y EDX intacto (no es parámetro): se llama como ((ResidueEcx*)ecxValue)->Get(eaxValue).
struct ResidueEcx {
    unsigned int __thiscall Get(unsigned int eaxValue);
};

// Palabra de estado x87 (como la deja fnstsw) tras comparar *a con *b; ver X87Math.cpp.
unsigned int __cdecl X87CompareStatusCpp(const float* a, const float* b);
// EAX con el que se entró a la función que lo llama, siempre que esa función lo invoque antes de tocar EAX (sin comprobaciones de ejecución ni
// cookie de /GS). Permite recibir argumentos en EAX o conservar sus bits altos sin ensamblador.
unsigned int __fastcall EntryEaxCpp();

// Lote de catorce archivos (12/10/2026).
// asin del CRT del EXE (_CIasin, 0x821E70) sobre un double: fpatan(x, sqrt((1 + x)(1 - x))) con la palabra de control del EXE.
double __cdecl ExeAsinDoubleCpp(double x);
// CIKChain: boneSpeed · acos(dot) · chainSpeed · 57.29578f (0x617AA3..0x617AC5), con el acos del CRT del EXE.
float __cdecl IKChainRotationAngleCpp(float boneSpeed, float dot, float chainSpeed);

// Lote de seis archivos de audio (12/10/2026).
float __cdecl UnsignedTimesFloatCpp(unsigned int value, float factor);
long __cdecl SecondsToMsCpp(double seconds);
long __cdecl BytesToMsCpp(unsigned int bytes, unsigned int bytesPerSecond);
int __cdecl WaveCursorBytesCpp(unsigned int position, unsigned int bytesPerSecond);
float __cdecl SmoothFadeVolumeCpp(float diff, float start, unsigned int elapsed, int fadeTime);
// 1 - num/den con enteros sin signo cargados como el EXE (CBulletTraces::Render).
float __cdecl OneMinusUnsignedRatioCpp(unsigned int numerator, unsigned int denominator);

// Lote de cinco archivos (14/10/2026).
// {sin(angle/2)·axis, cos(angle/2)} en out[0..3] con el seno sin redondear y lecturas/escrituras intercaladas (la salida puede solaparse con el eje) (CQuaternion::Set, 0x59C600).
void __cdecl QuaternionFromAxisAngleCpp(float* out, const float* axis, float angle);
// float(from - atan2(num, den)) con el fpatan sin redondear y un solo redondeo a float (CBoat::ProcessControl, rumbo bloqueado).
float __cdecl Atan2FromCpp(float num, float den, float from);
// *value = float(pow(base, step) * *value) con el pow del EXE en ST0 sin redondear (CBoat::ProcessControlInputs).
void __cdecl PowScaleFloatCpp(float* value, float base, float step);
// CTrailer::ProcessAI (0x6CF5FC): giro del remolque de equipaje hacia el rumbo del tractor; forward = {x, y} de la matriz o nullptr (usa el rumbo compacto).
float __cdecl TrailerSteerCpp(const float* tractorForward, float tractorHeading, const float* selfForward, float selfHeading);
// 10 · log10(x) con fldlg2/fyl2x y un solo redondeo a float (CEvent::GetSoundLevel, 0x4B2850).
float __cdecl Log10Times10Cpp(float x);
// 10 · log10((power2 + power1) / power1) con un solo redondeo a float; power2 = pow sin redondear a float, power1 ya float (CEvent::CalcSoundLevelIncrement, 0x4AC050).
float __cdecl SoundLevelIncrementCpp(double power2, float power1);

// Lote de cinco archivos (14/10/2026, 2).
// CFireManager::StartFire sin objetivo (0x539F00): ((rnd · factorA) · factorB + offset) · burnTime + nowMs con enteros sin signo como fild (+ 2^32), un redondeo a 24 bits por operación; el llamador aplica _ftol2.
float __cdecl FireBurnTimeCpp(int rnd, float factorA, float factorB, float offset, unsigned int burnTime, unsigned int nowMs);
// value - whole con fisub (entero exacto, un solo redondeo) (CFire::ProcessFire, 0x53AD8F).
float __cdecl IntFractionCpp(float value, int whole);
// float(from - atanFn(x, y)) con el valor de ST0 de atanFn (CGeneral::GetATanOfXY) sin redondear y un solo redondeo a float (CPlayerInfo::EvaluateCarPosition, 0x56DAD0).
float __cdecl PlayerInfoAtanFromCpp(float (__cdecl* atanFn)(float, float), float from, float x, float y);
// CPointLights::AddLight (0x7000E0): filtro por x/y (offsets y límite = radio + 15 como float); false si la luz queda fuera (comparaciones estrictas, false con NaN).
bool __cdecl PointLightsInRangeXYCpp(const float* camera, float pointX, float pointY, float radius, float* outOffsetX, float* outOffsetY, float* outLimit);
// CPointLights::AddLight (0x7000E0): distancia (z·z + y·y) + x·x, false si queda fuera del límite; si cae en la franja de atenuación (>= 0.75 · límite) multiplica red/green/blue por el coeficiente del EXE.
bool __cdecl PointLightsDistanceFadeCpp(float pointZ, const float* camera, float offsetX, float offsetY, float limit, float* red, float* green, float* blue);
// Lote de Replay.cpp (14/10/2026).
// CReplay::ProcessLookAroundCam (0x45D760): offsets = {sin(dir)·cos(view)·dist, cos(dir)·cos(view)·dist, sin(view)·dist}, cada uno con un solo redondeo a float y fsin/fcos sin redondear.
void __cdecl ReplayLookAroundOffsetsCpp(float view, float dirAngle, float dist, float* offsets);

// Lote de cinco archivos (14/10/2026, 3).
// CRopes::FindCoorsAlongRope (0x555E40): dist (ya recortada) · 31 sin redondear, interpolación entre dos segmentos (stride 3 floats) escrita en out[0..2]; devuelve el índice de segmento.
int __cdecl RopesInterpolateCpp(float dist, const float* segments, float* out);
// CShotInfo::AddShot (0x739CE8..0x739DD3): float(_ftol(vida + tiempo sin signo)) con fild + 2^32 y un redondeo por operación; se coloca tras UnsignedAsX87 en X87Math.cpp.
float __cdecl ShotInfoDestroyTimeCpp(float lifespan, unsigned int nowMs);
// CSimpleTransform::Invert (0x54EF90): x = -(cos·bx) - sin·by, y = sin·bx - cos·by, con cos/sin sin redondear; base = {x, y, z, rumbo}.
void __cdecl SimpleTransformInvertCpp(const float* base, float* out);
// Coeficientes de las cuatro esquinas de un sprite girado {-cos - sin, sin - cos, cos + sin, cos - sin}, fsin/fcos sin redondear (CSprite 0x70D490/0x70E780/0x70EDE0).
void __cdecl SpriteQuadCoeffsCpp(float rotation, float* coeffs);
// {w·cos, sin·h, cos·h, sin·w} con fsin/fcos sin redondear y un redondeo a float por producto (CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension, 0x70EAB0).
void __cdecl SpriteDimensionTermsCpp(float rotation, float width, float height, float* terms);

// Lote de cuatro archivos (14/10/2026, 4).
// TransformPoint (0x54ECE0): out = rotación por transform[3] de in más {transform[0..2]}; transform = {x, y, z, rumbo}; la salida puede ser la entrada.
void __cdecl CommonTransformPointCpp(float* out, const float* transform, const float* in);
// CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges (0x628C80): sqrt((x·x + y·y) + z·z) con suma sin redondear y un solo redondeo a float; v apunta a x, y, z consecutivos.
float __cdecl DestroyCarDistanceCpp(const float* v);
// CTimeCycle::Initialise (0x5BC026): {cos(b)·cos(a), sin(b)·cos(a), sin(a)} con fcos/fsin sin redondear y un redondeo a float por componente.
void __cdecl TimeCycleDirnToSunCpp(double angleA, double angleB, float* out);
// CTimeCycle::CalcColoursForPoint (0x560A80): x = sunX + sin(a), z = sunZ - cos(a) con seno/coseno sin redondear y un redondeo a float por componente.
void __cdecl TimeCycleVectorToSunCpp(float angle, float sunX, float sunZ, float* outX, float* outZ);
// Common::TransformVectors (0x54EE30): bucle de rotación de numVectors vectores (3 floats) con x87, para propagar los NaN como el EXE.
void __cdecl CommonTransformVectorsCpp(float* out, int numVectors, float cosHeading, float sinHeading, const float* in);

// Lote de diecinueve archivos (14/10/2026, 5).
// CAEPedAudioEntity::UpdateJetPack (0x4E1068..0x4E10A2): *sinStored = float(sin(angle)) (fst); devuelve float(|sin| · k + add) con el seno de fsin sin redondear y el signo cambiado solo si sin < 0 (fchs; NaN y 0 intactos).
float __cdecl PedAudioJetPackSinCpp(float angle, float k, float add, float* sinStored);
// CAEMFDecoder::GetStreamPlayTimeMs (0x4E7940): fild(playTime) / divider con un solo redondeo a float (x87 a 24 bits), truncado como _ftol2 (0x821B40).
long __cdecl MfDecoderPlayTimeMsCpp(int playTime, float divider);
// CAEMFDecoder::SetCursor (0x4E78E0): fild(pos) (+2^32 redondeado a float si pos < 0 como entero) · multiplier con un solo redondeo a float, truncado como _ftol2.
long __cdecl MfDecoderCursorToPlayTimeCpp(unsigned int pos, float multiplier);
// CCarAI::EntitiesGoHeadOn (0x41CD00), entidad sin matriz: ((dz*0 + cos(h)*dy) + (-sin(h))*dx) con fsin/fcos sin redondear a float, resultado float.
float __cdecl CarAIHeadOnDotNoMatrixCpp(float heading, float dx, float dy, float dz);
// CCover::FindDirFromVector (0x698D40): int(fpatan(-x, y) * (128/pi)) con el truncamiento de _ftol2; devuelve los 32 bits bajos del entero de 64 bits.
int __cdecl CoverDirFromVectorCpp(float x, float y);
// CCover::FindAndReserveCoverPoint (0x69936D): ((dy * float(cos a)) + (-sin(a) * dx)) + dz * 0 con a = float(dirIndex * 2pi/256); el seno entra sin redondear al producto.
float __cdecl CoverDirDotCpp(float dx, float dy, float dz, int dirIndex);
// float(atan2(num, den) * k + base) con el fpatan sin redondear, un redondeo a 24 bits por operacion y fstp final (CEntity::CreateEffects, 0x533A0B..0x533A5E).
float __cdecl EntityHeadingDegreesPlusCpp(float num, float den, float k, float base);
// ProcSurfaceInfo_c::AddObject (0x5A2E60): fila de matriz {x, y, z} *= {xy, xy, z} con fld/fmul/fstp por componente (conserva denormales).
void __cdecl ProcSurfaceScaleRowCpp(float* row, float xy, float z);
// ProcSurfaceInfo_c::AddObject (0x5A3803): *z = *z - (minZ * zScale - minZ) con un redondeo a 24 bits por operación y sin guardados intermedios.
void __cdecl ProcSurfaceOffsetZCpp(float* z, float minZ, float zScale);
// RwFastRealToUInt32Inline (rwplcore.h): FLD dword + FISTP dword; redondeo al par mas cercano; fuera de rango/NaN/inf = 0x80000000.
unsigned int __cdecl RwPlCoreFastRealToUInt32Cpp(float x);
// CTheScripts::DrawScriptSpritesAndRectangles, rectangulo angulado (0x464AC3..0x464AE9): terms = {cos·vAMy, sin·vAMx, cos·vAMx, sin·vAMy}; fcos queda sin redondear en ST0 y fsin se guarda como float, un redondeo a float por producto.
void __cdecl TheScriptsAngledRectTermsCpp(float angle, float vAMx, float vAMy, float* terms);
// CAEAudioUtility (0x4D9E80, 0x4D9EF0, 0x4D9F40, 0x5B97F0); se coloca tras UnsignedAsX87 en X87Math.cpp (los tres primeros lo usan).
// GetCurrentTimeInMS: (contador / frecuencia) · 1000.0f menos el inicio (mitades baja/alta cargadas por separado), devuelve el int64 truncado (_ftol2) como uint64.
unsigned long long __cdecl AudioUtilTimeMsCpp(long long counter, long long frequency, unsigned long long start);
// ConvertFromBytesToMS: bytes / (producto · 0.002f), ambos enteros cargados como sin signo; devuelve el cociente (el llamador hace floor y _ftol2).
double __cdecl AudioUtilBytesToMsQuotientCpp(unsigned int bytes, unsigned int product);
// ConvertFromMSToBytes: (producto · 0.002f) · ms, ambos sin signo; devuelve el producto (el llamador hace floor y _ftol2).
double __cdecl AudioUtilMsToBytesProductCpp(unsigned int product, unsigned int ms);
// StaticInitialise: 10.0f / denominador a 24 bits, almacenado como float (el log10 se obtiene con AudioLog10Cpp).
float __cdecl AudioUtilLogInverseCpp(double denominator);
// CAEAudioHardware::RescaleChannelVolumes (0x4D8B58): gain = 10^(vol · 0.05) con el _CIpow del EXE, *gain = float(ST0); si sum no es nulo, *sum = float(ST0 + *sum) con ST0 sin redondear.
void __cdecl AudioHwPowGainCpp(float vol, float* gain, float* sum);
// CAEAudioHardware::RescaleChannelVolumes (0x4D8BC4): r = (diff · *gain) / divisor (fild, fmul, fidiv), r < 0 -> 0, r > *gain -> *gain (NaN se conserva), *sum = float(r + *sum).
void __cdecl AudioHwFadeNearEndCpp(int diff, int divisor, const float* gain, float* sum);
// Lote de IdleCam.cpp (14/10/2026).
// CIdleCam::ProcessFOVZoom (0x517E2E..0x517E63): from + (to - from) · (sin((270 - ((now - started) / duration) · 180) · 0.0174532924) + 1) · 0.5, FSIN sin redondear y un solo redondeo a float.
float __cdecl IdleCamFovLerpCpp(float now, float started, float duration, float from, float to);
// CIdleCam::ProcessSlerp (0x517A8E..0x517BD5): 4 llamadas a atanFn (CGeneral::GetATanOfXY, la 4a sin redondear), ajuste de +-2PI, t = min((now - timeLast) / duration, 1) y salidas outX/outZ (interpolación con FSIN); devuelve t. d1/d2 = {x, y, z}; mag1/mag2 = sqrt(x² + y²) ya float.
float __cdecl IdleCamSlerpCpp(float (__cdecl* atanFn)(float, float), const float* d1, const float* d2, float mag1, float mag2, float now, float timeLast, float duration, float* outX, float* outZ);
// CTaskSimpleFightingControl (0x624BF4, 0x62A28E): 32 bits bajos del _ftol2 de (paso · 0.02f) · 1000.f (milisegundos de un paso).
unsigned int __cdecl FightCtlElapsedCpp(float step);
// CTaskSimpleFightingControl::ProcessPed (0x62A239..0x62A253): (probabilidad · tasa) · 0.025f > aleatorio, comparación ordenada (con NaN da false).
bool __cdecl FightCtlShouldBlockCpp(float random, float chance, float rate);
// CTaskSimpleFightingControl::ProcessPed (0x62A2E5..0x62A32F): 32 bits bajos del _ftol2 de ((rand · 1/32767 + .25) / (((tasa · agresión) · .025 · .7) + .3)) · intervalo (intervalo con signo vía fild; si es negativo, + 2^32).
unsigned int __cdecl FightCtlAttackDelayCpp(int random, float shootingRate, float aggression, unsigned int interval);
// CTaskSimplePlayerOnFoot::ProcessPlayerWeapon (0x686DB2..0x686DBB): float(sin(pitch) + z) con el fsin sin redondear y un solo redondeo en el fadd.
float __cdecl OnFootSinPlusCpp(float pitch, float z);
// CTaskSimplePlayerOnFoot::PlayerControlFighter (0x687687..0x6877AB): proyección de (-sin, cos) sobre right/forward de la matriz; right y forward apuntan a x, y, z; out = {x, y}. roundTrig: seno y coseno redondeados a float (rama cooperativa).
void __cdecl OnFootFighterProjectCpp(const float* right, const float* forward, float angle, float magnitude, bool roundTrig, float* out);
// float(a · k + b) con k double (fmul qword) y cada paso redondeado a 24 bits por la x87 (CWeaponEffects::Render, 0x7437A7: rc · 0.1 + base.x).
float __cdecl WeaponFxMulDoubleAddCpp(float a, double k, float b);
// a = atan2(y, x) con el fpatan sin redondear: *outSin = float(sin a), *outCos = float(cos a), *outOneMinusCos = float(1 - cos a) con el coseno sin redondear (CWeaponEffects::Render, 0x743630..0x7436B9).
void __cdecl WeaponFxAngleSinCosCpp(float y, float x, float* outSin, float* outCos, float* outOneMinusCos);
