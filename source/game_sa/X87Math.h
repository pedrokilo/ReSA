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
// a · b con el fmul del x87 y un solo guardado a float: a diferencia de SSE conserva los denormales del resultado (CRealTimeShadow::SetupForThisEntity, 0x706520).
float __cdecl FloatTimesFloatCpp(float a, float b);
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
// asin del CRT del EXE (_CIasin, 0x821E70) sobre un double, todas las ramas: |x| < 1 fpatan(x, sqrt((1 + x)(1 - x))) con la palabra de control del EXE;
// +-1 da +-pi/2 (double 0x3FF921FB54442D18); |x| > 1 e infinitos dan -NaN indefinido (0xFFF8000000000000) y NaN da x silenciosa (ambos con errno = EDOM del EXE).
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

// Lote de veinte archivos de nivel 2 (14/10/2026).
// CPlayerPed::GetButtonSprintResults (0x60A820): float(((velocidad / umbral - 1) * k) + 1), un redondeo a 24 bits por operacion (x87).
float __cdecl PlayerPedSprintResultCpp(float speed, float threshold, float multiplier);
// CPlayerPed::EvaluateTarget (0x60D020): float(atanFn(x, y) - compensacion) con el ST0 de CGeneral::GetATanOfXY (0x53CC70) sin redondear antes de restar.
float __cdecl PlayerPedAtanMinusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float compensation);
// Ultima llamada de un puente __fastcall: ECX = ecxValue, EDX = edxValue (no se tocan), devuelve value en ST0 (CPlayerPed::GetButtonSprintResults deja ECX = datos del jugador y EDX de entrada).
float __fastcall PlayerPedResidueFloatCpp(unsigned int ecxValue, unsigned int edxValue, float value);
// float(atan2(-dx, dy) - rumbo) con los dos fpatan sin redondear y un solo redondeo a float (CCarEnterExit::GetNearestCarDoor, 0x6529A4..0x6529D8); rumbo = atan2(-forward[0], forward[1]) si forward != nullptr (x, y consecutivos), si no el float heading.
float __cdecl CarEnterExitHeadingDiffCpp(float dx, float dy, const float* forward, float heading);
// EDX con el que se entró a la función que lo llama, en la mitad alta del resultado de 64 bits (EDX:EAX), siempre que esa función lo invoque antes de tocar EDX (sin comprobaciones de ejecución ni cookie de /GS); uso: uint32(CarEnterExitEntryEdxCpp() >> 32) (envoltura de CCarEnterExit::ComputePassengerIndexFromCarDoor, 0x64F1E0).
unsigned long long __fastcall CarEnterExitEntryEdxCpp();
// Circular group placer: devuelve angleStep = 2pi/n (float) y escribe *radius = sqrt(0.5 / (1 - cos(angleStep))) con cada paso redondeado a float (EXE 0x5FCE80/0x5FD330).
float __cdecl PedGroupPlacerCircleCpp(int numPeds, float* radius);
// CWeather::Update: 0.5 - cos(angle) * 0.5 con un solo redondeo a float
float __cdecl WeatherHalfMinusHalfCosCpp(float angle);
// CWeather::UpdateInTunnelness: -sin(angle) con un solo redondeo a float
float __cdecl WeatherNegSinCpp(float angle);
// CQuadBike (ProcessControl 0x6CDCC0): delta = vel * float(pow(res, step)) - vel con el pow del EXE guardado como float (fstp) antes de multiplicar.
float __cdecl QuadBikeDampDeltaStoredCpp(float res, float timeStep, float vel);
// CQuadBike (ProcessControl 0x6CDCC0): delta = pow(res, step) * vel - vel con el pow en ST0 sin guardar como float.
float __cdecl QuadBikeDampDeltaFullCpp(float res, float timeStep, float vel);
// CQuadBike::ProcessAI (0x6CE460): v * leanAngle - ((fullAnimLean * steer) / (steeringLock * 0.017453292f)) * (1 - v), v = pow(desLean, step) sin redondear.
float __cdecl QuadBikeLeanAngleCpp(float desLean, float timeStep, float leanAngle, float fullAnimLean, float steerAngle, float steeringLock);
// Pickups: float((sin(double(angle)) + add) * mult), fsin a precision completa y sin redondeo intermedio antes de fadd (0x455EC5, 0x4557ED)
float __cdecl PickupsSinAddMulCpp(float angle, float add, float mult);
// Pickups: (one - distance * factor) * scale en float, en este orden (fmul, fsubr, fmul; 0x455AC4) sin reasociar para el _ftol2 posterior
float __cdecl PickupsAlphaValueCpp(float distance, float factor, float one, float scale);
// CQuaternion::Normalise del EXE (0x4D1610): q = {x, y, z, w}; suma ((x·x + y·y) + z·z) + w·w sin redondear; si es exactamente 0 solo pone w = 1; si no (también NaN) multiplica cada componente por 1/sqrt(suma), con el recíproco en ST0 y un redondeo a float por componente.
void __cdecl AnimQuaternionNormaliseCpp(float* q);
// Lote de PostEffects.cpp (14/10/2026), prefijo PostFx.
// CPostEffects::SetupBackBufferVertex (0x7043D9..0x704440): trunc(float(ln(value) / ln(base))) con ln(base) guardado como DOUBLE (fstp qword) y el cociente redondeado a 24 bits; base = double en 0x86F990 (2.0). Devuelve los 32 bits bajos del _ftol2.
int __cdecl PostFxLog2TruncViaDoubleCpp(int value, double base);
// CPostEffects::ImmediateModeFilterStuffInitialize (0x703EB5..0x703EEF): igual pero con ln(base) en ST(1) sin redondear (fdiv st, st(1)).
int __cdecl PostFxLog2TruncKeepExtendedCpp(int value, double base);
// CPostEffects::UnderWaterRipple (0x703A46..0x703A9C): float(sin((ms sin signo · speed) + freq · y) · xoffset) con fsin sin redondear y un redondeo a 24 bits por operación.
float __cdecl PostFxRippleWaveCpp(unsigned int timeMs, float speed, float freq, float y, float xoffset);
// CPostEffects::UnderWaterRipple, último par (0x703B87..0x703C4F): S = sin(ms · speed + freq · y) sin redondear; *outU0 = float((S + xoffset) / width), *outU1 = float(((width + S) - xoffset) / width).
void __cdecl PostFxRippleFinalUCpp(unsigned int timeMs, float speed, float freq, float y, float xoffset, int width, float* outU0, float* outU1);
// CPostEffects::Fog (0x7042C0 y 0x704361): float(((sin(angle) · (quarter + radius)) + half) - third), fsin sin redondear y un redondeo a 24 bits por operación.
float __cdecl PostFxFogSinCoordCpp(float angle, float quarter, float radius, float half, float third);
// CPostEffects::Fog (0x7042DA y 0x70437B): igual con fcos.
float __cdecl PostFxFogCosCoordCpp(float angle, float quarter, float radius, float half, float third);
// Lote de GangWars.cpp (14/10/2026): los cinco bloques x87 de CGangWars.
// CGangWars::CalculateTimeTillNextAttack (0x443DB0): _ftol2 de (((rnd · 3.0518509e-5f) · 0.9f) · 1080000.f) + 648000.f con fild del entero y un redondeo a 24 bits por operación; devuelve los 32 bits bajos.
unsigned int __cdecl GangWarsNextAttackDelayCpp(int random);
// CGangWars::CreateAttackWave (0x444810..0x444D83, 0x444FBE): float(((rnd · 3.0518509e-5f) · scale) + base) - subtract con fild y un redondeo a 24 bits por operación (posición aleatoria de cada peatón y del pickup).
float __cdecl GangWarsRandomOffsetCpp(int random, float base, float scale, float subtract);
// CGangWars::CreateDefendingGroup (0x44557C..0x4455A2): float((rnd · 3.0518509e-5f) · scale + base), el radio de cada defensor (scale 3.0f, base 2.0f).
float __cdecl GangWarsRandomScaledCpp(int random, float scale, float base);
// CGangWars::CreateAttackWave (0x444C72, 0x444EDB): _ftol2 de ((value · scale) + base) · multiple (fimul, multiple = 1 si el EXE no lo tiene); devuelve los 32 bits bajos.
int __cdecl GangWarsScaledTruncCpp(float value, float scale, float base, int multiple);
// CGangWars::Update (0x446BA2, 0x446CCA): _ftol2 de (paso · 0.02f) · (negative ? -1000.f : 1000.f); devuelve los 32 bits bajos.
unsigned int __cdecl GangWarsTimeDeltaCpp(float step, bool negative);
// CGangWars::CreateDefendingGroup (0x445586..0x445594): ángulo = float((índice · 6.2831855f) / total) con fild + fmul + fidiv, un redondeo a 24 bits por operación (el fst guarda ese float).
float __cdecl GangWarsDefenderAngleCpp(int index, int total);
// Lote de Hud.cpp (14/10/2026).
// CHud::DrawRadar (0x58A422..0x58A435): float(-atan2(negRightZ, upZ) - quarterPi) con el fpatan sin redondear (fchs + fsub, un solo redondeo a float).
float __cdecl HudRadarBaseAngleCpp(float negRightZ, float upZ, float quarterPi);
// CHud::DrawRadar (0x58A4C4..0x58A4F6), una esquina del anillo del avion con fsin/fcos sin redondear: *outX = ((zero*cos + sin)*radiusX + sx40) + centerX, *outY = (((cos - zero*sin)*radiusY) + centerY) + baseY.
void __cdecl HudRadarRingPointCpp(float angle, float zero, float radiusX, float radiusY, float sx40, float centerX, float centerY, float baseY, float* outX, float* outY);
// CHud::DrawSubtitles (0x58C4F6..0x58C52F): ancho entero exacto (fild) en la cadena de subtitulos con estadisticas vitales; un redondeo a 24 bits por operacion y fstp final.
float __cdecl HudSubtitleCentreVitalsCpp(int width, float k520, float k0, float k1, float k2, float k3);
// CHud::DrawSubtitles (0x58C47B..0x58C495): y = (h - k*(h*c524)) - 2*(h*c524) con h entero exacto (fild) y un redondeo a 24 bits por operacion.
float __cdecl HudSubtitleBottomYCpp(int height, float c524, float k866c1c);
// Timer (CTimer, 0x5617E0..0x561B10): num / (unsigned)den como fild (+2^32) ; fdivr num ; fstp float (0x561B38..0x561B86).
float __cdecl TimerDivByU32Cpp(float num, unsigned int den);
// Timer: uint64 -> float como en 0x561BB8..0x561BD8 ((v & 0x7FFF...) + 2^63 con un solo redondeo a 24 bits).
float __cdecl TimerU64ToFloatCpp(unsigned long long value);
// Timer: (unsigned)_ftol2(fild(pauseMode) + delta / fild(divider)) con el cociente sin redondear a float (0x561BF6..0x561C32).
unsigned int __cdecl TimerPauseModeCpp(unsigned int pauseMode, float delta, unsigned int divider);
// Timer: UpdateVariables 0x5618D0..0x56191A; q = elapsed / divider queda en el x87; nuevo no recortado = ftol(nonClipped + q), paso = float(0.05f * q), ms = ftol(q).
void __cdecl TimerUpdateStepCpp(float timeElapsed, unsigned int divider, unsigned int nonClipped, unsigned int* newNonClipped, float* stepNonClipped, unsigned int* elapsedMs);
// CTrain::FindPositionOnTrackFromCoors (0x6F6CC0..0x6F6DEB): ((nx-x)(px-x) + (ny-y)(py-y)) / (sqrt((y-ny)^2 + (x-nx)^2))^2 con la raiz redondeada a 24 bits antes de elevarla al cuadrado.
float __cdecl TrainProjectionCpp(float x, float y, float nx, float ny, float px, float py);
// CTrain::FindPositionOnTrackFromCoors (0x6F6E95..0x6F6EE3): iluminacion de un nodo = low·(1/30)·(1 - day) + high·(1/30)·day (low/high son nibbles 0..15, fild con signo).
float __cdecl TrainNodeLightingCpp(int low, int high, float day);
// CTrain::FindPositionOnTrackFromCoors (0x6F6EE9..0x6F6F38): (luz(low, high, day) - previous) · factor + previous guardado como float.
float __cdecl TrainLightingBlendCpp(int low, int high, float day, float previous, float factor);
// CTrain::FindPositionOnTrackFromCoors (0x6F6F6D..0x6F6FEC): (before·k + (after·k - before·k)·factor) - (maximum - minimum)·0.5, mas total si es < 0 (comparacion ordenada: NaN no suma); k = 0.33333334f.
float __cdecl TrainRailDistanceCpp(int before, int after, float factor, float minimum, float maximum, float total);
// Lote ProjectileInfo (14/10/2026). Misiles teledirigidos de CProjectileInfo::Update (0x738B20); punteros a x, y, z consecutivos.
// steering -= direction · dot si dot = (sz·dz + sy·dy) + sx·dx es < 0 (comparación ordenada; con NaN no cambia), todo con redondeo float por operación (0x7394CF).
void __cdecl ProjInfoRemoveBackwardSteeringCpp(float* steering, const float* direction);
// factor = (acceleration [· speedMultiplier si fast]) · timestep; velocity[i] = steering[i]·factor + velocity[i] (0x7395EC).
void __cdecl ProjInfoAddSteeringCpp(float* velocity, const float* steering, float acceleration, float timestep, bool fast, float speedMultiplier);
// Si m = sqrt((x² + y²) + z²) > limit (ordenado) la velocidad se multiplica por limit / m, cada componente con su redondeo float (0x73964E).
void __cdecl ProjInfoLimitSpeedCpp(float* velocity, float limit);
// Lote de Weapon.cpp (14/10/2026).
// float(atan2(-forwardX, forwardY) · 57.29578f): fpatan sin redondear por 0x859878 y un solo redondeo a float (CWeapon::FireAreaEffect 0x73E965, CWeapon::Fire 0x7423D4).
float __cdecl WeaponHeadingDegreesCpp(float forwardX, float forwardY);
// float(tan(x)) con fptan (CWeapon::FireAreaEffect 0x73E9C7: fld pitch; fptan; fstp st(0); el llamador cambia el signo).
float __cdecl WeaponTanCpp(float x);
// lhs < tan(radians) con la tangente sin redondear a float, comparación ordenada (false con NaN) (CWeapon::DoDriveByAutoAiming 0x73DA27..0x73DA48).
bool __cdecl WeaponLessThanTanCpp(float lhs, float radians);
// CWeapon::FireInstantHit (0x740103..0x7401B7 y 0x74059B..0x74065B): endpoint += sineBasis · amplitude · sine y luego += cosineBasis · amplitude · cosine, con las sumas z en registro sin guardar; los punteros apuntan a x, y, z consecutivos.
void __cdecl WeaponSwayAccumulateCpp(float* endpoint, const float* sineBasis, const float* cosineBasis, float amplitude, float sine, float cosine);
// Lote de EventKnockOffBike (14/10/2026). Todo en X87Math.cpp (se pueden colocar al final).
// CEventKnockOffBike::CalcForcesAndAnims (0x4B0020, 0x4B0036): num / den con fdiv y un solo redondeo a float (fstp), exponente extendido hasta el fstp.
float __cdecl KnockOffRatioCpp(float num, float den);
// CEventKnockOffBike::CalcForcesAndAnims (0x4B00AB..0x4B00BB, 0x4B015A..0x4B017B): ((a * b) * c) * d sin guardados intermedios y un solo redondeo a float.
float __cdecl KnockOffMul4Cpp(float a, float b, float c, float d);
// CEventKnockOffBike::CalcForcesAndAnims (0x4B0266, 0x4B03DA, 0x4B04F9): out[i] = float(v[i] * k) por componente (fld, fmul, fstp); v y out apuntan a x, y, z consecutivos y no deben solaparse.
void __cdecl KnockOffScaleCpp(const float* v, float k, float* out);
// CEventKnockOffBike::CalcForcesAndAnims (0x4B0311, 0x4B0422, 0x4B047D, 0x4B0541, 0x4B05E1): fuerza = up * upK + side * sideK con el orden de redondeo del EXE por componente
// (x: dos productos sin guardar; y: side guardado como float; z: ambos guardados como float); los vectores apuntan a x, y, z consecutivos y out no debe solaparse con ellos.
void __cdecl KnockOffCombineCpp(const float* up, float upK, const float* side, float sideK, float* out);
// CEventKnockOffBike::SetPedSafePosition (0x4B4B0E..0x4B4B47): float(fpatan(-fwdX, fwdY) [+ pi (0x858CB8) si addPi]) con el fpatan sin redondear y un solo redondeo a float.
float __cdecl KnockOffHeadingCpp(float fwdX, float fwdY, bool addPi);
// Lote de PedGeometryAnalyser.cpp (14/10/2026).
// CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached (0x5F16EC..0x5F171B): -((prev.z·plane.z + prev.y·plane.y) + prev.x·plane.x) con un solo guardado a float; los punteros apuntan a x, y, z consecutivos.
float __cdecl PedGeomPlaneOffsetCpp(const float* plane, const float* previous);
// CPedGeometryAnalyser::ComputePedShotSide (0x5F13F6..0x5F143D): fpatan(-(target.x - origin.x), target.y - origin.y) sin redondear, - rotation, + pi/4, + 2pi si < 0, · 0.6366198 y truncamiento de _ftol2 (32 bits bajos); origin/target apuntan a x, y, z.
int __cdecl PedGeomShotSideCpp(const float* origin, const float* target, float rotation);
// CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox (0x5F561B..0x5F56B1): longitud de la ruta (puntos de 3 floats) sumando sqrt((dz² + dy²) + dx²) por tramo con 24 bits por operación; el valor queda exacto en un double.
double __cdecl PedGeomRouteLengthCpp(const float* points, int count);
// Comparación ordenada de las dos longitudes (0x5F5706: fcomp; test ah,5): true solo si first < second (false con NaN).
bool __cdecl PedGeomRouteLengthLessCpp(double first, double second);
// Lote de Object.cpp (14/10/2026).
// CObject::ProcessControl (0x5A2763..0x5A27BD), puertas: rumbo atan2(-forward.x, forward.y) con fpatan sin redondear (forward apunta a x, y; nullptr usa compactHeading), ajuste de +-2PI (0x858CBC) frente a doorStart +- PI (0x858CB8) con comparaciones sobre el valor completo, y float(doorStart - rumbo).
float __cdecl ObjectDoorAngleDiffCpp(const float* forward, float compactHeading, float doorStart);
// CObject::ProcessSamSiteBehaviour (0x5A093C..0x5A0945): a = atanFn(x, y) (CGeneral::GetATanOfXY, ST0 sin redondear); *outAngle = float(a) (fst) y *outDiff = float(a - heading) (fsub).
void __cdecl ObjectSamSiteAtanCpp(float (__cdecl* atanFn)(float, float), float x, float y, float heading, float* outAngle, float* outDiff);
// CObject::ProcessTrainCrossingBehaviour (0x5A0CD3..0x5A0D11): float(acos(upZ) -/+ step) con el _CIacos del EXE (0x822380) sin redondear antes de la resta (subtract) o suma; |upZ| > 1 y NaN dan NaN.
float __cdecl ObjectTrainCrossingAngleCpp(float upZ, float step, bool subtract);
// CObject::SetMatrixForTrainCrossing (0x59F230..0x59F28B): forward = {0·sen + eje.x·cos, 0·sen + eje.y·cos, sen + eje.z·cos}; axis y forward apuntan a x, y, z; fcos/fsin sin redondear, cos·eje.x y cos·eje.y y 0·sen se guardan como float, sen se guarda como float y cos·eje.z queda en ST0.
void __cdecl ObjectTrainCrossingForwardCpp(const float* axis, float angle, float* forward);
// CAEWeatherAudioEntity (AddAudioEvent 0x5068BA, UpdateParameters 0x505A85): 20·log10(sin signo(duration)·0.0375 + 0.25) + fiadd(defaultVolume), log por fyl2x sin redondear, un solo fstp float.
float __cdecl WeatherAudioThunderBaseCpp(unsigned int duration, int defaultVolume);
// UpdateParameters 0x505AED: 20·log10(playPos / sin signo(reqPlayPos)) + reqVolume (fild, fild(+2^32), fdivp, fyl2x, fmul, fadd, fstp).
float __cdecl WeatherAudioThunderRiseCpp(int playPos, unsigned int reqPlayPos, float reqVolume);
// UpdateParameters 0x505B3F: p = pow(10, reqVolume·0.05) (0x822130); devuelve 20·log10((sin signo(diff)·(1 - p))·0.005 + p).
float __cdecl WeatherAudioThunderFallCpp(float reqVolume, unsigned int diff);
// UpdateParameters 0x505EE1 (ruido de ciudad): 20·log10((1 - dist·(1/6))·speed) + 30, un solo fstp float.
float __cdecl WeatherAudioCityNoiseOffsetCpp(float dist, float speed);
// Service 0x5055C4 (lluvia): 20·log10(wet·1.25) + fild(defaultVolume), un solo fstp float.
float __cdecl WeatherAudioRainTargetCpp(float wet, int defaultVolume);

// Lote de cinco archivos de nivel 2 (15/10/2026).
// float(add + atan2(x, y)) con el fpatan sin redondear y un solo redondeo a float; add ya es float (CBirds::Update, 0x71251B..0x712521, ángulo de aparición relativo a la cámara).
float __cdecl BirdsAddAtan2Cpp(float add, float x, float y);
// Giro de la velocidad objetivo (CBirds::Update, 0x7126C5..0x7126FB): x' = c*x + s*y, y' = c*y - s*x con fsin/fcos sin redondear, un redondeo a float por producto y por suma; x e y se leen antes de escribir.
void __cdecl BirdsRotateCpp(float angle, float* px, float* py);
// CMatrix::RotateZ (0x59B390) sobre m = 16 floats (right@0, forward@4, up@8, pos@12 como en CMatrix): gira x/y de cada fila; cos sin redondear, sin guardado como float, un redondeo por operacion.
void __cdecl HoldEntityRotateZCpp(float* m, float angle);
// CPlaceable::GetHeading/GetRoll: fpatan sin redondear en ST0 (double, extern "C"); Placeable.cpp los usa con otro nombre float via /alternatename, no por este encabezado.
extern "C" double __cdecl PlaceableHeadingDblCpp(const float* forward);
extern "C" double __cdecl PlaceableRollDblCpp(const float* right, float upZ);

// Lote de cinco archivos (General, BoneNode y otros; 15/10/2026).
// Lote de General.cpp (15/10/2026), prefijo General. Los cuatro bloques x87 de CGeneral (se pueden colocar al final de X87Math.h).
// CGeneral::GetRandomNumberInRange<float> (0x41BD90): ((random · 1/32767) · (max - min)) + min con fild del entero, un redondeo a 24 bits por operación y el resultado en ST0 sin guardar (acepta extremos invertidos).
extern "C" double __cdecl GeneralRandomInRangeDblCpp(int random, float min, float max); // General.cpp lo declara como float con otro nombre (/alternatename) para devolver ST0 sin redondear
// CGeneral::GetRadianAngleBetweenPoints (0x53CBE0): dx = x2 - x1, dy = y2 - y1 (0 pasa a 0.0001f), fpatan(dx/dy) sin redondear y pi/2 ± pi/2 en el orden del EXE según dx > 0 y dy > 0 (con NaN, false).
extern "C" double __cdecl GeneralRadianAngleDblCpp(float x1, float y1, float x2, float y2); // General.cpp lo declara como float con otro nombre (/alternatename)
// CGeneral::GetATanOfXY (0x53CC70): 0 si x e y son 0; si no, ocho ramas según |x| < |y| y los signos; la rama x > 0, y > 0, |x| >= |y| devuelve el fpatan sin redondear en ST0.
extern "C" double __cdecl GeneralATanOfXYDblCpp(float x, float y); // General.cpp lo declara como float con otro nombre (/alternatename) para devolver ST0 sin redondear
// CGeneral::SolveQuadratic (0x53CE30): d = b·b - (a·c)·4 en registro; false si d < 0 (NaN resuelve); si no escribe *x2 = ((sqrt(d) - b) · (1/a)) · 0.5 y luego *x1 = ((-b - sqrt(d)) · (1/a)) · 0.5 (orden de escritura del EXE).
bool __cdecl GeneralSolveQuadraticCpp(float a, float b, float c, float* x1, float* x2);
// Lote de BoneNode.cpp (15/10/2026), prefijo BoneNode. Todos __cdecl; los vectores/quaterniones son floats consecutivos (x, y, z[, w]).
// BoneNode_c::EulerToQuat (0x6171F0): grados Euler -> quaternion {x, y, z, w} con fsin/fcos y 1/(x²+y²+z²+w²) (sin raíz); lee los tres ángulos antes de escribir (out puede ser angles).
void __cdecl BoneNodeEulerToQuatCpp(const float* angles, float* out);
// BoneNode_c::QuatToEuler (0x617080): quaternion {x, y, z, w} -> grados Euler {x, y, z} con fpatan sin redondear; en el bloqueo cardán (sen = ±1) relee q tras escribir out[1], como el EXE.
void __cdecl BoneNodeQuatToEulerCpp(const float* q, float* out);
// BoneNode_c::Limit (0x617650), tramo de sujeción: euler = {x, y, z}, min/max = m_LimitMin/m_LimitMax (x, y, z); primero el mínimo y luego el máximo por eje; si narrowZ (BONE_HEAD) el rango Z se estrecha hacia poseZ con max(1 - |x|/45, 0) y el mínimo resultante se compara sin guardar como float.
void __cdecl BoneNodeLimitClampCpp(float* euler, const float* limitMin, const float* limitMax, bool narrowZ, float poseZ);
// BoneNode_c::CalcWldMat (0x616CD0): filas right/up/at (3 floats cada una) de la matriz local a partir del quaternion {x, y, z, w} con s = 2/(x²+y²+z²+w²).
void __cdecl BoneNodeLocalMatrixCpp(const float* q, float* right, float* up, float* at);

// Lote de tres archivos (cHandlingDataMgr y otros; 15/10/2026).
// Producto de dos float con la fmul del x87 (conserva subnormales; cHandlingDataMgr y CPed::EmitFootRainParticles).
float __cdecl X87TimesFloatCpp(float a, float b);
// cHandlingDataMgr::LoadHandlingData (0x5BE2AC): float(valor · 0.4) con 0.4 double (0x86A950) y un solo redondeo a 24 bits (valor = atof).
float __cdecl HandlingAtofTimes04Cpp(double value);
// cHandlingDataMgr::ConvertBikeDataToGameUnits (0x6F5294..0x6F529F): float(sin(angle · k)) con el producto redondeado a 24 bits y fsin a precisión completa.
float __cdecl HandlingSinScaledCpp(float angle, float k);
// cHandlingDataMgr::ConvertDataToGameUnits (0x6F5080), todo el cuerpo aritmetico salvo InitGearRatios: accel/velocidad/1-masa viajan sin redondear a float como en la pila x87 del EXE
// (los subnormales se conservan), bucle de reduccion (0,01 y 1/6 double) y ramas por tipo (RC Bandit = 38, motos 162..174, m_bUseMaxspLimit, resto). Los punteros son los campos de
// tHandlingData/cTransmission (+0x7c, +0x84, +0x94, +8, +0x24, +0xc8, +0x88, +0x8c); engineAccel, maxVelocity, brake y collision se leen antes de escribirse.
void __cdecl HandlingConvertGameUnitsCpp(float accelConst, float velocityConst, float mass, int percentSubmerged, float drag, int vehicleId,
                                         bool maxspLimit, bool driveType4, float* engineAccel, float* maxVelocity, float* brake,
                                         float* massRecpr, float* buoyancy, float* collision, float* maxFlat, float* maxReverse);

// Lote Ped (15/10/2026).
// Registros con que sale _ftol2 (0x821B40) para el ST0 v entrado con ECX = entryEcx: EAX/EDX = int64 truncado, ECX = bits de la fraccion v - R (+/-0x7FFFFFFF), con R = fistp
// con el redondeo vigente (al par, -inf, +inf o 0) y la resta y el fstp dword con la precision y el redondeo vigentes (la propia x87). Compartido por
// CTheZones::SetCurrentZoneAsUnlocked (0x572800) y CPed::GetLocalDirection (0x5DEF60).
// Ftol2RegsCpp recibe v como float (ST0 ya redondeado a float por el llamador: con PC24 es lo que hay en el EXE); Ftol2RegsDblCpp recibe un ST0 de hasta 53 bits
// (cadena x87 con PC24 o PC53 sin pasar por float): usar esta cuando el EXE llama a _ftol2 directamente tras fmul/fadd sin fstp.
struct Ftol2Regs { unsigned int eax, ecx, edx; };
Ftol2Regs __cdecl Ftol2RegsCpp(float v, unsigned int entryEcx);
Ftol2Regs __cdecl Ftol2RegsDblCpp(double v, unsigned int entryEcx);
// CPed::GetLocalDirection (0x5DEF60): fpatan(-x, y) - rotacion + pi/4, + 2pi mientras < 0, * 2/pi y _ftol2; point apunta a x, y; entryEcx = this (ECX de entrada, que _ftol2 conserva en
// su salida corta). Devuelve EAX/ECX/EDX con que sale _ftol2 (eax = 32 bits bajos del entero); el llamador aplica "si > 3 reducir modulo 4" y el residuo de esa rama.
Ftol2Regs __cdecl PedLocalDirectionRegsCpp(const float* point, float rotation, unsigned int entryEcx);
// CPed::SetPedPositionInCar / PositionAttachedPed: atan2(-x, y) [+ add] con el fpatan sin redondear en ST0 (double, extern "C"); Ped.cpp los usa como float con otro nombre via /alternatename (no por este encabezado).
extern "C" double __cdecl PedAtanNegXYDblCpp(float x, float y);
extern "C" double __cdecl PedAtanNegXYAddDblCpp(float x, float y, float add);

// Ajuste de exactitud de Ped y RpAnimBlend (15/10/2026).
// --- CPed::CalculateNewVelocity (0x5E4C50), prefijo Cnv. Añadir al final de X87Math.h ---
// pow(base, step) del EXE (_CIpow, 0x822130) guardado como float (fstp dword [esp+0x10] en 0x5E5021 / 0x5E5193).
float __cdecl CnvDecayPowCpp(float base, float step);
// asin del EXE (_CIasin, 0x821E70) guardado como float (fstp dword [esi+0x520], 0x5E50E4). x ya recortado a [-1, 1].
float __cdecl CnvAsinFloatCpp(float x);
// asin(x) * (1 - blend) + blend * previous (0x5E512C..0x5E5153): el asin queda en ST0 sin pasar por float; (1 - blend) y blend * previous se redondean como float.
float __cdecl CnvBlendedAsinCpp(float x, float blend, float previous);
// (0.0f + first) + second (0x5E51F9..0x5E5268): el EXE pone 0x550/0x554 a +0.0 y les suma cada término, con lo que un -0.0 de first pasa a +0.0.
float __cdecl CnvZeroPlusSumCpp(float first, float second);
// Lote KillPedWithCar (15/10/2026).
// CPed::KillPedWithCar (0x5F094E..0x5F097A): lift · (random · 0.002 + 1.5) con fild del entero, fmul y fadd de los double 0.002 y 1.5 y fmul del float lift,
// cada paso redondeado a 24 bits por la x87 y un solo fstp a float (random = GetRandomNumber() & 255).
float __cdecl KpcBounceLiftScaleCpp(float lift, int random);
// Lote RpNan (RpAnimBlend, 15/10/2026). Aritmética float del x87 de las rutinas de animación 0x4D1680..0x4D32D0 y 0x59C080. Todo en X87Math.cpp (se puede colocar al final).
// fld a; fadd b (resultado en ST0; el que llama lo guarda como float, igual que el fstp del EXE). Conserva subnormales y con dos NaN elige el de mayor mantisa.
float __cdecl RpNanAddCpp(float a, float b);
// fld a; fsub b.
float __cdecl RpNanSubCpp(float a, float b);
// fld a; fmul b; fadd c con el producto sin redondear a float (exponente extendido), como `fld st(0); fmul [BonePos]; fadd [t]; fstp` de 0x4D2B90/0x4D30A0/0x4D2E40/0x4D32D0.
float __cdecl RpNanMulAddCpp(float a, float b, float c);
// 0x4D1A05..0x4D1A31 (skinned 2D, z entre -0.8 y -0.4): ((z · 2.5 + 2) · bonePosZ) + z en una sola cadena x87 sin redondear a float entre operaciones.
float __cdecl RpNanSkinZCorrectCpp(float z, float bonePosZ);
// CQuaternion::Get(RwMatrix*) del EXE (0x59C080): q = {x, y, z, w}; escribe right (m[0..2]), up (m[4..6]) y at (m[8..10]) de la RwMatrix. 2·z se guarda como float, 2·x y 2·y quedan en registros,
// y 2·y·w y 2·z·w no se redondean a float antes de sumarse (exponente extendido).
void __cdecl RpNanQuatToMatrixCpp(const float* q, float* m);
// CQuaternion::Get(RwV3d*, float*) del EXE (0x59C230): q = {x, y, z, w}; *angle = float(acos(2w)) (_CIacos 0x822380) y axis = (1 / sin(acos(2w))) · {x, y, z} con cada producto redondeado a float;
// 2w, el acos y el seno no se redondean a float, y las lecturas del cuaternion y escrituras de angle/axis siguen el orden del EXE (pueden solaparse). Fuera de |2w| <= 1 el acos del CRT da el indefinido y errno = 33.
void __cdecl QuaternionGetAxisAngleCpp(const float* q, float* axis, float* angle);
// Lote RpUpdate (RpAnimBlendClumpUpdateAnimations, 0x4D34F0; w20). Cadenas x87 sin guardar entre operaciones (cada operación redondea a 24 bits; un solo fstp a float).
// 0x4D3578..0x4D3585: fld total; fdiv speed; fmul blend; fadd acc.
float __cdecl RpUpdateSyncAccumCpp(float total, float speed, float blend, float acc);
// 0x4D35F3..0x4D3605: fld total; fdiv totalTime; fmul totalBlend; fmul timeStep.
float __cdecl RpUpdateSyncStepCpp(float total, float totalTime, float totalBlend, float timeStep);

// Lote de doce archivos (15/10/2026).
// --- Lote Matrix.cpp (w20, prefijo Matrix): m = CMatrix como 16 floats (right@0, flags@3, forward@4, up@8, pos@12). Todos cdecl. ---
// CMatrix::SetRotateXOnly/YOnly/ZOnly (0x59AFA0 / 0x59AFE0 / 0x59B020): fcos/fsin guardados como float; no tocan flags, relleno ni posicion.
void __cdecl MatrixSetRotateXOnlyCpp(float* m, float angle);
void __cdecl MatrixSetRotateYOnlyCpp(float* m, float angle);
void __cdecl MatrixSetRotateZOnlyCpp(float* m, float angle);
// CMatrix::SetRotate(x, y, z) (0x59B120): fsin/fcos sin redondear, cos(y), sin(z) y cos(z)*sin(x) guardados como float; posicion a cero.
void __cdecl MatrixSetRotateCpp(float* m, float x, float y, float z);
// CMatrix::RotateX / RotateY (0x59B1E0 / 0x59B2C0): rotan right, forward, up y pos; fcos sin redondear, fsin como float (en RotateY, right.x usa el seno sin redondear). RotateZ (0x59B390) usa HoldEntityRotateZCpp.
void __cdecl MatrixRotateXCpp(float* m, float angle);
void __cdecl MatrixRotateYCpp(float* m, float angle);
// CMatrix::Reorthogonalise (0x59B6A0): up = normalize(right x forward), right = normalize(forward x up), forward = up x right, sin comprobar cero, como el x87 del EXE.
void __cdecl MatrixReorthogonaliseCpp(float* m);
// CMatrix::ConvertToEulerAngles (0x59A840): flags = banderas del llamador; i, j, k = ejes derivados de las banderas (tablas Idx1/Idx2 de Matrix.cpp); pX/pY/pZ pueden apuntar dentro de m.
void __cdecl MatrixConvertToEulerCpp(const float* m, float* pX, float* pY, float* pZ, unsigned int flags, int i, int j, int k);
// CMatrix::ConvertFromEulerAngles (0x59AA40): escribe los 9 elementos de right/forward/up; mismos flags e i, j, k que MatrixConvertToEulerCpp.
void __cdecl MatrixConvertFromEulerCpp(float* m, float x, float y, float z, unsigned int flags, int i, int j, int k);
// CPedIK::PitchForSlope (0x5FE721..0x5FE72C): num / fcos(angle) con fdivr y un solo redondeo a float (el llamador resta 0.3f).
float __cdecl PedIKDivCosCpp(float num, float angle);
// CPedIK::RotateTorsoForArm (0x5FDFB4..0x5FE000): rumbo atan2(-(direction.x - pos.x), direction.y - pos.y) con fpatan sin redondear, ajustado a +-2pi respecto a rotation (0x858CB8/0x858CBC) y restado; direction y pedPos apuntan a x, y.
float __cdecl PedIKArmHeadingDiffCpp(const float* direction, const float* pedPos, float rotation);
// --- CAEVehicleAudioEntity (lote w20, prefijo VehAudio) ---
// GetFreqForPlayerEngineSound 0x4F8145..0x4F8196: B = ((|fsin(angle)| * lean) [+ boostOffset si boost]) * gf, todo en ST0 sin redondear y un solo fstp a float (fchs solo si sin < 0).
float __cdecl VehAudioBikeLeanCpp(float angle, float lean, bool boost, float boostOffset, float gf);
// ProcessPlayerVehicleEngine 0x4FBBCD..0x4FBC1C: ((absSpeed - gearDown) / (gearMax - gearDown)) limitado a [0, 1] sobre ST0 sin redondear (NaN se conserva), un solo fstp a float.
float __cdecl VehAudioRevsRatioCpp(float absSpeed, float gearMax, float gearDown);
// ProcessDummyTrainEngine 0x500868..0x5008BE: z = (y + x) * 0 + z saturado a +-0.3 (doble; NaN -> -0.3) por la pendiente, un solo fstp a float.
float __cdecl VehAudioTrainTiltCpp(float forwardX, float forwardY, float forwardZ, float slope);
// ProcessDummyBicycle/ProcessPlayerBicycle 0x4FFE75..0x4FFED9 y 0x5000EE..0x50014F: min(|speed / maxVelocity|, 1) con NaN -> 1.0 y resultado <= 0 -> +0.0, guardado como float.
float __cdecl VehAudioBicycleRatioCpp(float speed, float maxVelocity);
// Bicicletas 0x4FFF39..0x4FFF66 y 0x5001AF..0x5001DC: ((|fsin(angle)| * multiplier) + 1.0f) * frequencyBase sobre ST0 sin redondear y un solo fstp a float.
float __cdecl VehAudioBicycleFrequencyCpp(float angle, float multiplier, float frequencyBase);
// --- CTaskSimpleJetPack (lote w20, prefijo JetPack). Añadir al final de X87Math.h ---
// JetPackUnsignedDivCpp: (float)(entero sin signo) / divisor con un solo redondeo (RenderJetPack 0x67F7DC).
float __cdecl JetPackUnsignedDivCpp(unsigned int value, float divisor);
// JetPackLookBackHeadingCpp: atan2(-(x * -1), y * -1) como float (ProcessControlInput 0x67E914, cámara mirando a otro lado).
float __cdecl JetPackLookBackHeadingCpp(float x, float y);
// JetPackLerpThrustAngleCpp: ((1 - p) * ud) * 75 grados + p * ángulo con p = pow(0.9, paso) sin redondear (ProcessControlInput 0x67E93E).
float __cdecl JetPackLerpThrustAngleCpp(float scaledUpDown, float angle, float step);
// JetPackThrustDirCpp: out[0..2] = -sin(a) * delante + cos(a) * arriba con los redondeos del EXE (ProcessThrust 0x67F1C2); up/forward/out apuntan a x, y, z consecutivos.
void __cdecl JetPackThrustDirCpp(float thrustAngle, const float* up, const float* forward, float* out);
// JetPackSwingSumsCpp: sums[0..2] = delta + aceleración de la velocidad del peatón (ProcessThrust 0x67F43D).
void __cdecl JetPackSwingSumsCpp(const float* pedVelocity, const float* prevPedVelocity, const float* taskVelocity, float* sums);
// JetPackSwingAxisCpp: balanceo de una pierna (velocidad *speed y ángulo *angle de la tarea) con el eje delante o lateral de la matriz (ProcessThrust 0x67F4A0 / 0x67F544).
void __cdecl JetPackSwingAxisCpp(const float* sums, const float* axis, bool grounded, float step, float* speed, float* angle);
// JetPackSpeedDampBaseCpp: 1 - |velocidad| * 0.02 como float (ProcessThrust 0x67F5EA); después PowScaleVector3Cpp.
float __cdecl JetPackSpeedDampBaseCpp(const float* velocity);
// --- Lote PathFind (w20). Anadir al final de X87Math.h ---
// Como PlayerPedResidueFloatCpp pero ademas fija EAX: al volver ECX = ecxValue, EDX = edxValue, EAX = eaxValue y ST0 = value. Ultima llamada de CPathFind::CalcRoadDensity
// (0x44EFC0) y FindNodeOrientationForCarPlacement (0x450320); no toca ECX ni EDX (llama a ResidueRegsCpp con los mismos registros y recarga value de la pila).
float __fastcall PathFindResidueFloatCpp(unsigned int ecxValue, unsigned int edxValue, float value, unsigned int eaxValue);
// CVector::Normalise del EXE (0x59C910) sobre v[0..2]: (x*x + y*y) + z*z; si es <= 0 solo pone v[0] = 1.0f, si no multiplica los tres por 1.0f / sqrt(suma) con un redondeo a float por producto.
// Devuelve la palabra de estado que deja su fnstsw (0x59C934) SIN el campo TOP: C3/C2/C0 de fcom contra 0.0 y las banderas IE/DE/ZE/OE/UE/PE acumuladas leidas con _statusfp2.
unsigned int __cdecl PathFindNormaliseStatusCpp(float* vector);
// --- CTaskSimpleSwim (lote w20, prefijo Swim) ---
// ProcessControlInput 0x688BE5..0x688BF3: fpatan(y, x); fst dword [out]; fsub dword [sub]. *out = float(atan2) y devuelve float(atan2 - sub) con un solo redondeo (el llamador niega el resultado, fchs).
float __cdecl SwimAtan2StoreMinusCpp(float y, float x, float sub, float* out);
// ProcessControlInput 0x688C72..0x688C7B: float(_CIasin(x) - sub) con el asin del EXE (0x821E70, todas las ramas) en ST0 sin redondear antes de la resta.
float __cdecl SwimAsinMinusCpp(float x, float sub);
// ProcessControlInput 0x688FA2..0x688FA7: float(angleFn(0, 0, x2, y2) - sub); angleFn es CGeneral::GetRadianAngleBetweenPoints (0x53CBE0) y su ST0 se consume directamente en la resta.
float __cdecl SwimRadianAngleMinusCpp(float (__cdecl* angleFn)(float, float, float, float), float x2, float y2, float sub);
// ProcessSwimmingResistance 0x68A43A..0x68A4D6 (SWIM_UNDERWATER_SPRINTING): out = {P0 + A, P1 + B, ((sin·sy + (P2 + C)) + 0,01)} con A,B,C = sx·right, P = (cos·forward)·sy, fcos/fsin completos y un redondeo a 24 bits por operación.
void __cdecl SwimSprintVelocityCpp(float rotationX, const float* right, const float* forward, float sx, float sy, float* out);
// ProcessSwimmingResistance 0x68A4D8..0x68A54D: ms = pow(0,9, paso)·ms + (1 - pow)·v con el pow del EXE (0x822130) en ST0 sin redondear y los tres ms leídos antes de escribir.
void __cdecl SwimResistanceCpp(float step, float* moveSpeed, const float* wanted);
// --- CBmx (prefijo Bmx; Bmx.cpp) ---
// Los demas bloques de Bmx.cpp usan ayudantes ya existentes: TrigSimpleSinCpp/TrigSimpleCosCpp (steer), TrigSimpleSinMulCpp (ruedas, k = 0.02f/0.04f), TrigAtan2Cpp (GetFrameOffset) y PowScaleFloatCpp (cigueñal, base 0.97f).
// float(sin(angle * k)) y float(cos(angle * k)) con el producto de 24 bits en ST0 sin pasar por float (CBmx::PreRender 0x6C0E0A..0x6C0E34, k = 0x8595EC).
float __cdecl BmxSinScaledCpp(float angle, float k);
float __cdecl BmxCosScaledCpp(float angle, float k);
// float((sin(angle) * k1) * k2) con el seno sin redondear (CBmx::PreRender 0x6C0A27..0x6C0A31, k1 = minZ de la colision, k2 = 0.8f de 0x858C98).
float __cdecl BmxSinTimes2Cpp(float angle, float k1, float k2);
// float((((1 - cos(lean)) * boundMinZ) * 0.9f) + zOff) con el coseno sin redondear (CBmx::PreRender 0x6C1251..0x6C128C; 0.9f de 0x858C20).
float __cdecl BmxChassisZCpp(float lean, float boundMinZ, float zOff);
// float(sin(cur / total * 2PI + start) * blend * mult) (CBmx::ProcessControl 0x6BFA9C..0x6BFAB8, rama de sprint) y la variante con "+ add" sin redondear antes (rama de pedaleo 0x6BFB0B..0x6BFB2F, add = LeanAngle).
float __cdecl BmxLeanTermCpp(float curTime, float totalTime, float start, float blend, float mult);
float __cdecl BmxLeanTermAddCpp(float curTime, float totalTime, float start, float blend, float mult, float add);
// float(asin(x) * -1.0f) con el _CIasin del EXE (0x821E70, todas sus ramas) y el fmul por -1.0f de 0x858C1C (no cambia el signo de un NaN); CBmx::PreRender 0x6C0FCA..0x6C0FDA.
float __cdecl BmxNegAsinCpp(float x);
// Lote WaterLevel (WaterLevel.cpp, w20). Aritmetica x87 de CWaterLevel; todo en X87Math.cpp (se puede colocar al final). Reutiliza TrigCompoundSinMulAdd, TrigCompoundCosMulAdd, TrigSimpleCosCpp, IntTimesFloatCpp, Ftol2RegsDblCpp.
// Imagen de un CWaterVertex del EXE (20 bytes: x, y, CRenPar { z, bigWaves, smallWaves, flowX, flowY }); WaterLevel.cpp la lee desde m_aVertices.
struct WaterLevelVertexCpp { short x; short y; float z; float bigWaves; float smallWaves; signed char flowX; signed char flowY; };
static_assert(sizeof(WaterLevelVertexCpp) == 20, "WaterLevelVertexCpp debe medir lo mismo que CWaterVertex");
// CWaterLevel::AddWaveToResult (0x6E8269..0x6E8287): fractionY + fractionX a 24 bits < 1.0 (ordenado: con NaN da false).
bool __cdecl WaterLevelWaveLowerCpp(float fractionX, float fractionY);
// 0x6E82D3..0x6E8453 y 0x6E8498..0x6E8546: *level = (((alongX-base)*wx + (alongY-base)*wy) + *level) + base (wx = fractionX, o 1-fractionX en la mitad superior) y, si normal != null,
// el producto vectorial de 0x59C730 (x = dy*0 - dx*K, y = dx*0 - dy*K, z = 4; K = 2 abajo, -2 arriba) sin normalizar; la llamada a Normalise la hace el llamador.
void __cdecl WaterLevelWaveInterpolateCpp(float base, float alongX, float alongY, float fractionX, float fractionY, bool lower, float* level, float* normal);
// CWaterLevel::FindNearestWaterAndItsFlow (0x6E9E3F..0x6E9EF4): distancia a un cuadrilatero (cada resta a 24 bits, raiz con fsqrt) devuelta como float; minX/maxX/minY/maxY son x e y de los vertices 0, 1, 0 y 2.
float __cdecl WaterLevelNearestDistanceCpp(float x, float y, float minX, float maxX, float minY, float maxY);
// 0x6E9FE6..0x6EA1FE: indice (0..3) del vertice mas cercano por distancia al cuadrado ((y-vy)^2 + (x-vx)^2 guardada como float) con las comparaciones ordenadas del EXE.
int __cdecl WaterLevelNearestFlowIndexCpp(float x, float y, const float* vertexX, const float* vertexY);
// CWaterLevel::CalculateWavesOnlyForCoordinate (0x6E6F4F..0x6E713F): las tres ondas con la tabla de senos (indice = _ftol2 & 0xFF), escribe *out y la normal (x, y, z) SIN normalizar.
// absX/absY = valores absolutos tras el neg del EXE; rem1/rem2/rem3 = (tiempo - desplazamiento) % 5000, 3500 y 3000 (div sin signo); multX*multY = factores de 0x8D38C8/0x8D38E8.
void __cdecl WaterLevelWavesFullCpp(int absX, int absY, float multX, float multY, float wavyness, unsigned int rem1, unsigned int rem2, unsigned int rem3,
                                    float bigAmplitude, float smallAmplitude, const float* sinTable, float* out, float* normal);
// 0x6E7146..0x6E7209: multiplicador de color y brillo a partir de la normal ya normalizada (colorScale = 0x8D3924, colorBias = 0x8D3920, sunGlare = 0xC812E0).
void __cdecl WaterLevelWaveColorGlareCpp(const float* normal, float colorScale, float colorBias, float sunGlare, float* colorMult, float* glare);
// CWaterLevel::CalculateWavesOnlyForCoordinate2Native (0x6E7210..0x6E739F): mismas tres ondas sobre *height, con y guardado como float (fstp dword [esp+8]) y x exacto en ST.
// Devuelve el EAX (indice de la tercera onda), ECX y EDX con que sale el EXE (residuo de la tercera llamada a _ftol2 con ECX de entrada 0xBB8).
Ftol2Regs __cdecl WaterLevelWavesNativeCpp(int absX, int absY, float multX, float multY, float wavyness, unsigned int rem1, unsigned int rem2, unsigned int rem3,
                                           float bigAmplitude, float smallAmplitude, const float* sinTable, float* height);
// CWaterLevel::GetWaterLevelNoWavesForQuad (0x6E5BB0) y ...ForTriangle (0x6E5E90): vertex apunta a los vertices 0..3 (cuadrilatero) o 0..2 (triangulo) en el orden de la lista de indices del poligono.
// outBigWaves/outSmallWaves pueden ser nulos (el cuadrilatero sale con true sin probar la profundidad; el triangulo si la prueba); con outBigWaves != null se escribe tambien outSmallWaves.
bool __cdecl WaterLevelQuadCpp(const WaterLevelVertexCpp* const* vertex, bool limitedDepth, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves);
bool __cdecl WaterLevelTriangleCpp(const WaterLevelVertexCpp* const* vertex, bool limitedDepth, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves);
// CWaterLevel::RenderWater (0x6EF8E2..0x6EF9EB): avance de los desplazamientos de textura; shifts apunta a 0x8D3824 (cuatro floats contiguos); flowX/flowY = 0xC22890/0xC22894, scale = 0x8D392C.
void __cdecl WaterLevelShiftsAdvanceCpp(float timeStep, float flowX, float flowY, float scale, float* shifts);
// 0x6EFA0A..0x6EFA31 y 0x6EFA37..0x6EFA4B: float(sin(angle) * first * second + add) y float(cos(angle) * first * second + add) con el seno/coseno sin redondear y cada producto a 24 bits.
float __cdecl WaterLevelSinMulMulAddCpp(float angle, float first, float second, float add);
float __cdecl WaterLevelCosMulMulAddCpp(float angle, float first, float second, float add);
// 0x6EFAA2..0x6EFAB8: (random * scale) * multiplier con un solo guardado a float.
float __cdecl WaterLevelNoiseFirstCpp(int random, float scale, float multiplier);
// 0x6EFAC7..0x6EFAF9: (random * scale) * multiplier + cosineSpill * factor con un solo guardado a float.
float __cdecl WaterLevelNoiseSecondCpp(int random, float scale, float multiplier, float cosineSpill, float factor);
// --- CPhysical (Physical.cpp; w20, prefijo Physical) ---
// floor(coord * 0.02f + 60) con fld/fmul/fadd/fstp qword, floor y _ftol2 (CPhysical::Add 0x544A30, RemoveAndAdd 0x542560, ProcessShift 0x54DB10, CheckCollision 0x54D920); devuelve los 32 bits bajos de la conversion de 64 bits (0 si |valor| >= 2^63, NaN o infinito).
int __cdecl PhysicalSectorCpp(float coordinate);
// CMatrix::Rotate(x, y, z) del EXE (0x59B460) sobre m = 16 floats (right@0, forward@4, up@8, pos@12): fsin/fcos sin redondear, un redondeo por operacion, lecturas antes de escribir.
void __cdecl PhysicalRotateXYZCpp(float* m, float x, float y, float z);
// CPhysical::PositionAttachedEntity (0x547990..0x547A76): turnZ' = float(((clamp(ajuste2pi(hA - hB), +-0.5) * 1e-5f) * mass) + turnZ), hA/hB = fpatan(-fwd.x, fwd.y) sin redondear (forward nullptr usa el rumbo compacto).
float __cdecl PhysicalAttachTurnZCpp(const float* attachedForward, float attachedHeading, const float* selfForward, float selfHeading, float turnZ, float mass);
// CPhysical::ApplySpeed (0x548270..0x5482B1), puerta: rumbo (fpatan sin redondear) ajustado en +-2pi contra doorStart +- pi (0x858CB8/0x858CBC) y float(rumbo - doorStart).
float __cdecl PhysicalDoorHeadingCpp(const float* forward, float compactHeading, float doorStart);
// Lote de Radar.cpp (15/10/2026).
// CRadar::CalculateCachedSinCos (0x5837B5..0x5837D6, 0x5837E9..0x583813): a = atan2(-x, y) con fpatan sin redondear; orientation = float(a) (fst dword), sin/cos de a sin redondear (un solo redondeo a float cada uno).
void __cdecl RadarSinCosFromAtan2Cpp(float x, float y, float* orientation, float* sinOut, float* cosOut);
// CRadar::CalculateCachedSinCos sin matriz de camara (0x5837F5): a = rumbo (fld dword); orientation = float(a), sin/cos de a con un solo redondeo a float cada uno.
void __cdecl RadarSinCosFromAngleCpp(float angle, float* orientation, float* sinOut, float* cosOut);
// CRadar::DrawRotatingRadarSprite (0x584896..0x584916): cuatro vertices {x, y} en verts[8]; theta = i · halfPi + (angle - subtract) (un redondeo a 24 bits por operacion), fsin/fcos sin redondear,
// vx = ((zero·cos + sin) · W + x), vy = ((cos - zero·sin) · H + y) con W/H = fild del entero con signo (+2^32 si es negativo) sin redondear a float.
void __cdecl RadarRotatedSpriteVertsCpp(float angle, float x, float y, unsigned int width, unsigned int height, float subtract, float halfPi, float zero, float* verts);
// CRadar::DrawYouAreHereSprite (0x584A25..0x584A4A): *outY = float(y - sin(a)·radius), *outX = float(cos(a)·radius + x) con fsin/fcos sin redondear.
void __cdecl RadarYouAreHereOffsetCpp(float circleAngle, float radius, float x, float y, float* outX, float* outY);
// CRadar::DrawEntityBlip (0x58752A..0x58754E), destello de la pista: a = heading · degToRad (un redondeo a 24 bits), *x = float(cos(a) · steps + *x), *y = float(*y - sin(a) · steps) con fcos/fsin sin redondear
// y steps = fild del int16 con signo.
void __cdecl RadarAirstripRotateCpp(float heading, float degToRad, int steps, float* x, float* y);
// CRadar::DrawRadarMask (0x585850..0x58586C): arc = j · step; *outX = float(cos(arc)·cornerX), *outY = float(sin(arc)·cornerY) con fcos/fsin sin redondear antes de multiplicar.
void __cdecl RadarArcPointCpp(int j, float step, float cornerX, float cornerY, float* outX, float* outY);
// CRadar::DrawRadarGangOverlay (0x5867A2..0x5867F7): ((fsin(angle) + one) · half) · alpha (fimul) con un redondeo a 24 bits por operacion; el llamador trunca con _ftol2.
float __cdecl RadarGangPulseCpp(float angle, int alpha, float one, float half);
// CRadar::DrawRadarMap (0x586A76..0x586A99): float(playerY - 2 · (atan2(-forwardZ, upZ) · degrees)) con el fpatan sin redondear.
float __cdecl RadarPlaneBottomYCpp(float forwardZ, float upZ, float degrees, float playerY);
// CRadar::ClipRadarPoly (0x585333..0x5853B7): poly = 4 vertices {x, y} consecutivos; true si el cuadrado cae dentro del cuadrilatero: (s1·x3 - y3)·(s1·x0 - y0) < 0 con s1 = (y0 - y1)/(x0 - x1) sin redondear a float
// y (s3·x1 - y1)·(s3·x0 - y0) < 0 con s3 = (y0 - y3)/(x0 - x3) guardado como float (fstp dword); comparaciones ordenadas (false con NaN).
bool __cdecl RadarClipHullInsideCpp(const float* poly);
// CPad (lote w20, prefijo Pad). Anadir al final de X87Math.h.
// ProcessPad: angulo del POV (fild con signo, + 2^32 si es negativo, * 0.01f * 0.017453292f) como double sin redondear a float.
double __cdecl PadPovAngleCpp(unsigned int pov);
// ProcessPad: *sine = float(sin(angle)); *negCosine = float(cos(angle) * -1.0) (fsin / fcos / fmul -1.0f del EXE).
void __cdecl PadPovSinCosCpp(double angle, float* sine, float* negCosine);
// StartShake_Distance: true si sqrt((dz*dz + dy*dy) + dx*dx) < limit (ordenada); camera y point apuntan a x, y, z consecutivos.
bool __cdecl PadShakeWithinDistanceCpp(const float* camera, const float* point, float limit);
// StartShake_Train: *distanceSquared = float(dy*dy + dx*dx) y devuelve true si la suma SIN redondear es < 4900.0 (ordenada); camera y point apuntan a x, y consecutivos.
bool __cdecl PadShakeTrainWithinCpp(const float* camera, const float* point, float* distanceSquared);
// StartShake_Train: byte bajo de _ftol2((70.0f - sqrt(distanceSquared)) + 30.0f) (0x53FB18..0x53FB36).
unsigned char __cdecl PadShakeTrainFrequencyCpp(float distanceSquared);
// memmgr: sin ayudantes nuevos.

// --- CPedIK::PitchForSlope (lote pitfix) ---
// CMatrix::operator* (0x59BE30) con la aritmetica x87 del EXE (exponente extendido en los productos y sumas intermedios). a, b y r apuntan a CMatrix como 16 floats (right@0, forward@4, up@8, pos@12); r recibe 12 floats (los rellenos no se tocan) y puede coincidir con a o b.
void __cdecl PedIKFixMatMulCpp(const float* a, const float* b, float* r);

// --- CTaskSimpleSwim (lote swfix, prefijo SwFix); insertar tras SwimResistanceCpp ---
// float(acc + (a * b) * c) con los productos sin redondear (ST0 con exponente extendido) y un solo fstp final (ProcessControlInput: `x += ts * k * y`).
float __cdecl SwFixAccum3Cpp(float acc, float a, float b, float c);
// float(acc + a * b) (0x688EE0).
float __cdecl SwFixAccum2Cpp(float acc, float a, float b);
// float(acc - a * b) (0x689327).
float __cdecl SwFixSubScaledCpp(float acc, float a, float b);
// float(-(a - b)) con fsub + fchs: a == b devuelve -0 (0x6891CC).
float __cdecl SwFixNegDiffCpp(float a, float b);

// --- CTaskSimpleJetPack::ProcessThrust (lote jpfix, prefijo JpFix). Anadir al FINAL de X87Math.h (despues del bloque JetPack) ---
// Todos reproducen la secuencia x87 del EXE (0x67EF20..0x67F660): 24 bits de precision con exponente extendido, un fstp float solo donde el EXE guarda,
// y los operandos de memoria (masa, paso, matriz, velocidades) con la semantica de `fop m32`: un NaN senalizante en memoria pierde frente a un NaN
// silencioso del registro (y se silencia si el registro es un numero). Los punteros apuntan a la memoria ORIGINAL (no pasar copias por registro x87).
// JpFixUpForceCpp: fuerza de ((arriba * 0.008) * masa) * paso (rama de parada, 0x67EF51); out[0..2].
void __cdecl JpFixUpForceCpp(const float* up, const float* mass, const float* step, float* out);
// JpFixNominalForceCpp: fuerza nominal 0.8 (0.4 si halve) hacia arriba (0x67F103); out[0..2].
void __cdecl JpFixNominalForceCpp(const float* up, bool halve, const float* mass, const float* step, float* out);
// JpFixThrustDirCpp: out = -sin(a) * delante + cos(a) * arriba, con z limitada a 0.75 (0x67F1C2..0x67F23C).
void __cdecl JpFixThrustDirCpp(const float* angle, const float* up, const float* forward, float* out);
// JpFixHeightBelowCpp: (suelo + 100) < z (0x67F25A..0x67F268).
bool __cdecl JpFixHeightBelowCpp(float groundHeight, const float* z);
// JpFixThrustForceCpp: fuerza hacia delante (0x67F2B5..0x67F368); scaled = JpFixHeightBelowCpp, groundHeight = segunda llamada a CalculateGroundHeight.
void __cdecl JpFixThrustForceCpp(const float* t, bool scaled, float groundHeight, const float* z, const float* mass, const float* step, float* out);
// JpFixStrafeForceCpp: fuerza lateral (0x67F386..0x67F433) con *strafe != 0.
void __cdecl JpFixStrafeForceCpp(const float* strafe, const float* right, const float* mass, const float* step, float* out);
// JpFixStopForceCpp: frenado de la rama de parada (0x67EFDA..0x67F0FB). true = fuerza en out; false = velocidad a cero.
bool __cdecl JpFixStopForceCpp(const float* speed, const float* step, const float* mass, float* out);
// JpFixSwingSumsCpp / JpFixSwingAxisCpp / JpFixTwistDampCpp / JpFixSpeedDampCpp: cola 0x67F43D..0x67F660 (sustituyen a JetPackSwingSumsCpp, JetPackSwingAxisCpp,
// PowScaleFloatCpp(&m_LegTwist...) y JetPackSpeedDampBaseCpp + PowScaleVector3Cpp).
void __cdecl JpFixSwingSumsCpp(const float* pedVelocity, const float* prevPedVelocity, const float* taskVelocity, float* sums);
void __cdecl JpFixSwingAxisCpp(const float* sums, const float* axis, bool grounded, const float* step, float* speed, float* angle);
void __cdecl JpFixTwistDampCpp(float* twist, const float* step);
void __cdecl JpFixSpeedDampCpp(float* velocity, const float* step);

// --- CFormation (Formation.cpp, lote w20, prefijo Formation) ---
// CMatrix::TransformPoint del EXE (0x59C890 inlinada en 0x699FF0): out = matrix · {v0, v1, v2}; matrix = float[16] como CMatrix; out no puede solaparse con matrix ni vector.
void __cdecl FormationTransformPointCpp(const float* matrix, const float* vector, float* out);
// float(sqrt(dy² + dx²)) con dx = firstX - secondX, dy = firstY - secondY, sumas y raíz en x87 (CFormation::FindCoverPointsBehindBox, 0x69A130..0x69A15E).
float __cdecl FormationDistance2DCpp(float firstX, float firstY, float secondX, float secondY);
// Punto del círculo de CFormation::GenerateGatherDestinations (0x69A6A0..0x69A705): out = {float(float(sin(a)·radius) + position.x), float(cos(a)·radius + position.y), fld/fstp(position.z)}, a en ST0 sin redondear.
void __cdecl FormationGatherPointCpp(int index, int count, float heading, float radius, const float* position, float* out);
// Punto de CFormation::GenerateGatherDestinations_AroundCar (0x69A823..0x69AB7E): position -/+ right·offset (positive = segunda mitad) más forward·length·float(0.5 - index/count), con los redondeos float del EXE.
void __cdecl FormationGatherCarPointCpp(const float* position, const float* right, float offset, const float* forward, float length, int index, int count, bool positive, float* out);
// Centroide de CFormation::DistributeDestinations (0x69B2B7.. destinos si destination, 0x69B321.. peatones): Σ en x87, mismas multiplicaciones por 1.0f/count que el EXE; out = {x, y, z}.
void __cdecl FormationCentroidCpp(const float* points, int count, bool destination, float* out);
// Radio medio Σ sqrt((dz² + dy²) + dx²) / count que el EXE deja en ST0 (el valor de 24 bits de mantisa cabe exacto en un double); center = {x, y, z}.
double __cdecl FormationMeanRadiusCpp(const float* points, int count, const float* center);
// Escala de DistributeDestinations (0x69B44C..0x69B484): float(destinationRadius) y pedRadius limitados por abajo a 1.0 (NaN no cambia); devuelve destino / peatones sin redondear.
double __cdecl FormationRadiusScaleCpp(double destinationRadius, double pedRadius);
// Reescala un punto de peatón (0x69B490..0x69B4F0); out no debe solaparse con point.
void __cdecl FormationScalePositionCpp(const float* point, const float* pedCenter, const float* destinationCenter, double scale, float* out);
// CFormation::FindNearestFreeDestination (0x69B1B0): índice del primer destino libre de menor distancia (-1 si no hay) y *total = float(mínimo + *total); claimed = bytes de CFormation::m_Destinations.m_PointHasBeenClaimed.
int __cdecl FormationNearestFreeCpp(const float* points, const unsigned char* claimed, int count, const float* origin, float* total);
// CFormation::FindCoverPoints (0x69B8D3..0x69B8FF): sqrt((x² + y²) + z²) < 0.005f sobre CVector speed (comparación ordenada: con NaN da false).
bool __cdecl FormationVehicleStationaryCpp(const float* speed);
// CFormation::DistributeDestinations_CoverPoints (0x69B646..0x69B6C8; sólo lo usa formation-extra.patch): false si el peatón se descarta; si no, *score = 1 - ((dist(destino) + targetDistance) - pedDistance) / pedDistance (double sin redondear).
bool __cdecl FormationCoverScoreCpp(const float* ped, const float* destination, const float* target, float targetDistance, double* score);

// fight: sin declaraciones nuevas (reutiliza ResidueRegsCpp y EntryEaxCpp ya declarados en X87Math.h).

// --- CAutomobile (Automobile.cpp; lote w20, prefijo Automobile). Colocar al FINAL de X87Math.h (tras los de Pad/Physical). ---
// 0x6A884D / 0x6A887C: fild tiempo (con signo, + 2^32 si es negativo) + fadd float (24 bits por operacion) y _ftol2; devuelve el dword bajo del entero truncado (EAX).
unsigned int __cdecl AutomobileTimeMsPlusFloatCpp(unsigned int timeMs, float addend);
// ProcessControl 0x6B2DCD..0x6B2DEE: (1 - p) * rate + p * audio con p = pow(base, timeStep) del CRT del EXE sin redondear a float.
float __cdecl AutomobileWheelSpinAudioCpp(float base, float timeStep, float audio, float rate);
// ProcessControl 0x6B2989..0x6B2A34: f = asin(x) / (steeringLock * degToRad) sin redondear; f = 1.0 si forceOne o 1.0 < f; devuelve float(f * steerAngle) (nuevo m_fSteerAngle).
float __cdecl AutomobileSteerFactorCpp(float x, float steeringLock, float degToRad, float steerAngle, bool forceOne);
// ProcessControl 0x6B30E0..0x6B317E: fuerza de los hidraulicos force = (speed * (up * -1.0f)) * turnMass con speed = pow(pow(base, ts) / (tsf^2 * factor + 1), ts) * tsf - tsf sin redondear; up y force apuntan a x, y, z.
void __cdecl AutomobileHydraulicForceCpp(float turnSpeedForward, float timeStep, float base, float factor, const float* up, float turnMass, float* force);
// ProcessCarWheelPair 0x6A5CE7..0x6A5D0F: float(pow(base, timeStep) * temperatureAboveOne + 1.0) con el pow del CRT del EXE sin redondear.
float __cdecl AutomobileTireDecayCpp(float base, float timeStep, float temperatureAboveOne);
// HydraulicControl 0x6A0E9D..0x6A0EBF: angulo = fpatan(stickLR, stickUD) + quarterPiNeg (24 bits); *cosOut = float(fcos(angulo) * jump); *sinOut = float(fsin(angulo) * jump).
void __cdecl AutomobileHydraulicStickCpp(float stickLR, float stickUD, float jump, float quarterPiNeg, float* cosOut, float* sinOut);
// ProcessSwingingDoor 0x6A9F12..0x6A9F51: nuevo m_angVel = (((s*fy)*vy + (s*fz)*vz) + (s*fx)*vx) + angVel con s = (fsin(angle) + bias) * mul, todo sin pasar por float salvo el fstp final; forward y moveSpeed apuntan a x, y, z.
float __cdecl AutomobileBonnetAngVelCpp(float angle, float bias, float mul, const float* forward, const float* moveSpeed, float angVel);
// GetCarRoll 0x6A6010: PlaceableRollDblCpp(right, upZ) * 57.2957763671875 (0x859878, fmul a 24 bits); extern "C" y double: Automobile.cpp lo declara como float con otro nombre (/alternatename) para devolver ST0 sin redondear.
// GetCarPitch (0x6A6050) es PlaceableRollDblCpp sobre el vector forward: Automobile.cpp lo enlaza directamente por /alternatename.
extern "C" double __cdecl AutomobileCarRollDblCpp(const float* right, float upZ);

// decision: sin ayudantes nuevos en X87Math.h (se reutilizan ResidueRegsCpp y CarEnterExitEntryEdxCpp).

// CVehicle::SetTransmissionRotation (0x6DBBB0..0x6DBC67), prefijo Vehicle. wheelPos apunta a x, y, z consecutivos. outAngleX = float(-atan2((L + R) * 0.5 - z, matY - y)) (+ PI 0x40490FDB si isFront, sumado en x87); outAngleY = float(atan2(L - R, |x| + |x|)); fpatan sin redondear y un solo fstp float cada uno.
void __cdecl VehicleTransmissionAnglesCpp(float angleL, float angleR, float matY, const float* wheelPos, bool isFront, float* outAngleX, float* outAngleY);

// --- Lote Garage (Garage.cpp, w20). Colocar al final de X87Math.h. Reutiliza RpNanAddCpp (Garage.cpp lo llama para fRadius + radio de la esfera). ---
// CGarage::BuildRotatedDoorMatrix (0x4479F0..0x447A67): angulo = doorPos * k (un fmul a 24 bits, sin redondear a float), sin y cos con fsin/fcos de precision completa guardados como float;
// up = {-(sin * fy), sin * fx, cos} con productos del x87 (fy y fx = forward.y y forward.x). El producto vectorial right = forward x up lo hace el que llama (CrossProduct, 0x59C730).
void __cdecl GarageDoorUpCpp(float doorPos, float k, float fx, float fy, float* up);
// CrossProduct (0x59C730) llamado con a = forward, b = up: right = {b.z*a.y - a.z*b.y, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y}; cada par de productos se queda en el x87 (24 bits de mantisa, exponente extendido), la resta se guarda como float.
void __cdecl GarageDoorRightCpp(const float* a, const float* b, float* out);
// CGarage::IsPointInsideGarage(point) (0x448740): g = el CGarage como floats (pos 0..2, dirA 3..4, dirB 5..6, topZ 7, ancho 8, alto 9), p = {x, y, z}. Comparaciones ordenadas (con NaN no rechaza);
// dx queda sin redondear en el primer producto y como float en el segundo. *ecxOut recibe los bits de dy (float) cuando la ruta llega a guardarlo (0x448764), si no se deja como estaba.
bool __cdecl GarageInsidePointCpp(const float* g, const float* p, unsigned int* ecxOut);
// CGarage::IsPointInsideGarage(point, radius) (0x4487D0): mismos g y p; gz - r, r + topZ, r + ancho y r + alto sin redondear a float; -r guardado como float.
bool __cdecl GarageInsideRadiusCpp(const float* g, const float* p, float radius);
// CGarage::IsEntityTouching3D (0x448F15..0x448FBF): true si la posicion pos = {x, y, z} esta dentro de la caja del garaje ampliada en radius (limites g[10..13] = izquierda, derecha, frente, fondo y z, topZ).
bool __cdecl GarageEntityNearCpp(const float* g, float radius, const float* pos);

// --- Lote Shadows.cpp (prefijo Shadows). Anadir al final de X87Math.h. Todos __cdecl; los vectores son floats consecutivos (x, y[, z]). Reutiliza ShadowSqrtCpp/ShadowDistanceFadeCpp ya declaradas. ---
// CShadows::UpdatePermanentShadows (0x70C9B9..0x70CA46): int((1.0f - transcurrido / cuarto) * valor), enteros sin signo como fild y _ftol2 (palabra baja; 0/0 -> 0).
int __cdecl ShadowsPermanentFadeCpp(unsigned int elapsed, unsigned int quarter, int value);
// CShadows::AddPermanentShadow (0x707007..0x70702E): x*x + y*y < limit con cada producto y la suma redondeados a 24 bits (false con NaN).
bool __cdecl ShadowsSumSquaresBelowCpp(float x, float y, float limit);
// CShadows::UpdatePermanentShadows (0x70CB24, 0x70CBEE): fsqrt(((sz-hz)^2 + (sy-hy)^2) + (sx-hx)^2) < *shortest (estricto) -> guarda float(mag) en *shortest y devuelve true.
bool __cdecl ShadowsOilNearerCpp(const float* source, const float* shadow, float* shortest);
// CShadows::UpdatePermanentShadows (0x70CBBB..0x70CC24): ShadowsOilNearerCpp y ademas (shadow - source) . (first - source) < 0 (todo a 24 bits sin guardar).
bool __cdecl ShadowsOilSecondCpp(const float* source, const float* shadow, const float* first, float* shortest);
// CShadows::StoreRealTimeShadow (0x707E09..0x707E4C): azimut = fpatan(-vx, -vy) * 57.29578f; elevacion = -(fpatan(-vy, -vz) * 57.29578f), fpatan sin redondear y float guardado.
void __cdecl ShadowsSunAnglesCpp(float vx, float vy, float vz, float* azimuth, float* elevation);
// CShadows::CastShadowEntityXY (0x70874E..0x7088E1): (a*b) + (c*d) en la x87 guardado como float (argumentos en el orden de carga del EXE).
float __cdecl ShadowsDot2Cpp(float a, float b, float c, float d);
// CShadows::CastShadowEntityXY (0x709840..0x709858): ((a*b) + (c*d)) + e, la suma con e sin pasar por float.
float __cdecl ShadowsDot2AddCpp(float a, float b, float c, float d, float e);
// CShadows::CastShadowEntityXY (0x708E80..0x708E9A): (x - ox) * ey - (y - oy) * ex con ex = destino.x - ox, ey = destino.y - oy; se devuelve sin redondear (el llamador guarda float(cruce)).
double __cdecl ShadowsEdgeCrossCpp(float x, float y, float originX, float originY, float destinationX, float destinationY);
// CShadows::CastShadowEntityXY (0x708EC9..0x708FFB): interseccion de recorte; ratio = |cruce(anterior)| / (|value| + |cruce(anterior)|); outPos/outUv [0..1] = (1 - ratio) * anterior + ratio * actual. Todos los punteros apuntan a x, y.
void __cdecl ShadowsClipIntersectCpp(const float* prevPos, const float* curPos, const float* prevUv, const float* curUv, const float* origin, const float* destination, float value, float* outPos, float* outUv);
// CShadows::CastShadowEntityXY (0x7096F9..0x709713): (nz*z + ny*y) + nx*x como float; normal y vertex apuntan a x, y, z.
float __cdecl ShadowsPlaneDistanceCpp(const float* normal, const float* vertex);
// CShadows::CastShadowEntityXY (0x70971D..0x709750): (-1.0f / nz) * ((ny*y + nx*x) - distance) como float; normal apunta a x, y, z.
float __cdecl ShadowsPlaneHeightCpp(const float* normal, float x, float y, float distance);
// CShadows::CastShadowEntityXYZ (0x70A142..0x70A176): numerador / (valor * multiplicador) como float.
float __cdecl ShadowsDivByProductCpp(float numerator, float value, float multiplier);
// CShadows::CastShadowEntityXYZ (0x70A208..0x70A247): valor * multiplicador + base como float.
float __cdecl ShadowsMulAddCpp(float value, float multiplier, float base);
// a * b en la x87 a 24 bits (exponente extendido) como double (StoreStaticShadow 0x70BA47, CastShadowEntityXYZ 0x70A3B3, StoreShadowForVehicle).
double __cdecl ShadowsMulDblCpp(float a, float b);
// CShadows::StoreStaticShadow (0x70BC53..0x70BCB7): |a - b| en la x87 como double.
double __cdecl ShadowsAbsDiffCpp(float a, float b);
// CShadows::CastShadowSectorList / CastRealTimeShadowSectorList (0x70A728..0x70A771, 0x70A8B1..0x70A8FA): (posZ - zDistance) < (maxZ + height) && (minZ + height) < posZ, sumas a 24 bits sin guardar (false con NaN).
bool __cdecl ShadowsHeightBandCpp(float posZ, float zDistance, float maxZ, float minZ, float height);
// CShadows::RenderStoredShadows (0x70AA76) y GeneratePolysForStaticShadow (0x70B746): position -/+ (|side| + |front|) como float (maximum suma, si no resta).
float __cdecl ShadowsRectEdgeCpp(float position, float front, float side, bool maximum);
// CShadows::StoreStaticShadow (0x70BA27..0x70BA43) y StoreShadowForVehicle: (dy*dy) + (dx*dx) como float; position y camera apuntan a x, y.
float __cdecl ShadowsCameraDistanceSquaredCpp(const float* position, const float* camera);
// CShadows::StoreStaticShadow (0x70BAB9..0x70BB10) y StoreShadowForVehicle (0x70BEFE): int(fade * valor) con fild y _ftol2 (palabra baja); fade es un valor de 24 bits en un double.
int __cdecl ShadowsFadeScaleCpp(double fade, int value);
// CShadows::CastShadowEntityXY (0x709959..0x709979): int(a * b) con _ftol2 (palabra baja).
int __cdecl ShadowsTruncMulCpp(float a, float b);
// CShadows::StoreShadowForVehicle (0x70C035..0x70C064): desplazamiento = ((largo * 0.5f) - maxY) * shift; position[0..1] -= desplazamiento * forward.
void __cdecl ShadowsVehiclePositionCpp(float* position, float length, float maximumY, float shift, float forwardX, float forwardY);
// CShadows::ShadowRenderTriangleCB (0x709F13..0x709F45): 0 si 1 - depth*depth < 0, si no el byte bajo de int(intensity * (1 - depth*depth)).
unsigned int __cdecl ShadowsTriangleAlphaCpp(float depth, unsigned int intensity);
// CShadows::ShadowRenderTriangleCB (0x709D07..0x709D43): ((ny*ay) + (nz*az)) + (nx*ax) > 0 (false con NaN); normal y at apuntan a x, y, z.
bool __cdecl ShadowsTriangleFacesAwayCpp(const float* normal, const float* at);
// CShadows::StoreShadowForVehicle (0x70BE2B..0x70BE5E): squared / ((mult*mult) * 4.0f) como float.
float __cdecl ShadowsCutsceneScaleCpp(float squared, float lodMultiplier);
// CShadows::StoreShadowForVehicle (0x70BEC2..0x70BEF8): 1.0f - (fsqrt(squared) - 0.75f*distance) / (0.25f*distance) sin redondear a float (distinto de ShadowDistanceFadeCpp, que usa * (4/d) como StoreStaticShadow).
double __cdecl ShadowsVehicleFadeCpp(float squared, float distance);
// CShadows::StoreShadowForVehicle (0x70C075..0x70C0C3, moto): float(factor * ancho) con factor = |lean| * 5.092958f + 1.0f (+ |rightZ| * 4.0f si abandoned y |rightZ| > 0.6f) sin pasar por float.
float __cdecl ShadowsBikeBreadthCpp(float lean, float breadth, bool abandoned, float rightZ);
// CShadows::StoreShadowForVehicle (0x70C17F..0x70C1D9): mag = fsqrt(side.y^2 + side.x^2); si mag < 0.5f escribe *outX = (side.x * (1/mag)) * 0.5f, *outY = ((1/mag) * side.y) * 0.5f y devuelve true; si no, no escribe.
bool __cdecl ShadowsSideNormalizeCpp(const float* side, float* outX, float* outY);
// CShadows::StoreShadowForVehicle (0x70C28B..0x70C2C1): fsqrt((x*x + y*y) + z*z) * timeStep > 0.1f a 24 bits sin guardar (false con NaN); speed apunta a x, y, z.
bool __cdecl ShadowsMovementAboveCpp(const float* speed, float timeStep);

// CCamera (Camera.cpp; lote w20, prefijo Camera). ANADIR AL FINAL de source/game_sa/X87Math.h.
// Segundos transcurridos: fild (+2^32 si el bit alto esta puesto, a 24 bits) * 0.001f, un solo fstp float (CamShake 0x50AA91, CamShakeNoPos 0x50A97E).
float __cdecl CameraElapsedSecondsCpp(unsigned int elapsed);
// CamShake (0x50AA72): float(1.0 - distance * 0.01) con las constantes double 0x85A308 y 0x85A310.
float __cdecl CameraShakePercentCpp(float distance);
// SetPercentAlongCutScene (0x50C088): (percent * 0.01f) sin guardar * tiempo final sin signo, un solo fstp float.
float __cdecl CameraCutSceneElapsedCpp(float percent, unsigned int finishTime);
// AddShakeSimple (0x50D254) y LerpFOV: *start = float(reloj sin signo), *end = float(reloj + plus) con el entero sin pasar por float.
void __cdecl CameraTimePlusCpp(unsigned int nowMs, float plus, float* start, float* end);
// Find3rdPersonCamTargetVector (0x514970): ((crosshairX - 0.5f) * 2) * tan(fov * 0.5f * pi/180) y la parte vertical ((0.5f - crosshairY) * 2 * (1.0f / aspect)) * tan.
float __cdecl CameraAimHorizontalCpp(float fov, float crosshairX);
float __cdecl CameraAimVerticalCpp(float fov, float crosshairY, float aspect);
// Find3rdPersonQuickAimPitch (0x50AD40): -(atan2((tan * ((0.5f - crosshairY) * 2)) * (1.0f / aspect), 1) + vertical) con fptan/fpatan sin redondear.
float __cdecl CameraQuickAimPitchCpp(float fov, float vertical, float crosshairY, float aspect);
// SetNearClipBasedOnPedCollision (0x50CB90): ((sqrt(arg) / denominator) * factor) * (maximum - base) + base, con suelo en base (ordenado) como el EXE.
float __cdecl CameraNearClipPedCpp(float arg, float base, float denominator, float factor, float maximum);
// IsSphereVisible (0x420C40): true si el punto en espacio de camara (x, y, z) esta dentro de los 4 planos (normals = 4 x {x, y, z}) y entre near y far; NaN no rechaza.
bool __cdecl CameraSphereInFrustumCpp(const float* point, float radius, const float* normals, float nearLimit, float farLimit);
// CameraLinearComponent (ProcessVectorMoveLinear/TrackLinear/FOVLerp, 0x50D510..): mezcla lineal o con suavizado seno, fsin sin redondear y el resto a 24 bits.
float __cdecl CameraLinearBlendCpp(float from, float to, float ratio, bool ease);
// ProcessFOVLerp (0x516500): false si el reloj sin signo supera el fin (o desordenado); si no *ratio = (ahora - inicio) / (fin - inicio) sin guardar intermedios.
bool __cdecl CameraFovLerpRatioCpp(unsigned int nowMs, float endTime, float startTime, float* ratio);
// Process (transicion, 0x52BA9D): fase = elapsed / duration con fild + 2^32 y un solo redondeo a float.
float __cdecl CameraPhaseCpp(unsigned int elapsed, unsigned int duration);
// Process: ease = 0.5 - cos(x * pi) * 0.5 con las sustituciones de fase del EXE (moving / catchup == 0), fcos sin redondear.
float __cdecl CameraEaseCpp(float phase, float moving, float catchup, bool initial);
// Process: out[i] = ease * speed[i] + base[i]; mode 0..4 elige que productos se guardan a float (0: y, z; 1: x; 2 y 3: z; 4: x, y, z).
void __cdecl CameraAdvanceCpp(float* out, const float* base, const float* speed, float ease, int mode);
// Process (modos 1 y 4): igual que CameraAdvanceCpp con la velocidad = destination - base calculada como el EXE (modo 4 guarda las restas a float; modo 1 solo la de z).
void __cdecl CameraAdvanceTowardCpp(float* out, const float* base, const float* destination, float ease, int mode);
// Process: fov = ((operand [- base]) * coefficient) + base, difference == true resta base antes de multiplicar.
float __cdecl CameraFovAdvanceCpp(float base, float operand, float coefficient, bool difference);
// Process: fuerza del temblor = force - elapsed(sin signo) * 0.00028f.
float __cdecl CameraShakeForceCpp(float force, unsigned int elapsed);
// Process: sum + sqrt((dz*dz + dy*dy) + dx*dx) con dx = current - previous; devuelve el double sin pasar a float (current y previous apuntan a x, y, z).
double __cdecl CameraAccumulateDistanceCpp(const float* current, const float* previous, float sum);
// Process: float(sum / frames) con el double de CameraAccumulateDistanceCpp.
float __cdecl CameraAverageDistanceCpp(double sum, int frames);
// CalculateFrustumPlanes (0x514D60): (ny*py + nz*pz) + nx*px, como float.
float __cdecl CameraDotYZXCpp(const float* normal, const float* position);
// CalculateFrustumPlanes: terms = {cosine, sine, verticalCosine, verticalSine} (el seno vertical ya con el signo cambiado).
void __cdecl CameraFrustumTermsCpp(float fov, int height, int width, float* terms);
// CalculateDerivedValues (0x5151A6): normaliza (x, y) con 1 / sqrt(y*y + x*x); si la longitud es 0 da (1, y).
void __cdecl CameraFrontNormCpp(float x, float y, float* nx, float* ny);
// StartTransition (0x515236 / 0x515440 / 0x5154CF): float(atan + add) y float(atan - sub); atanFn = &CGeneral::GetATanOfXY (devuelve ST0 sin redondear).
float __cdecl CameraAtanPlusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float add);
float __cdecl CameraAtanMinusCpp(float (__cdecl* atanFn)(float, float), float x, float y, float sub);
// TryToStartNewCamMode, modos cinematicos (0x51F5DD..): angulo = atanFn(inputX, inputY) + rotation; outX = float(cos + inputX), outY = float(sin) + inputY.
void __cdecl CameraCineyRotatedCpp(float (__cdecl* atanFn)(float, float), float inputX, float inputY, float rotation, float* outX, float* outY);
// CameraControlChooseJump (0x52A87A): float(heading - sub) con heading = headingFn(self) (bit_cast de &CPlaceable::GetHeading) sin redondear a float antes de la resta.
float __cdecl CameraHeadingMinusCpp(float (__fastcall* headingFn)(const void*, void*), const void* self, float sub);
// Process (0x52C8C2, 0x52CA1B): EAX de _ftol2(value * multiplier) (32 bits bajos del truncado de 64 bits).
int __cdecl CameraTruncateProductCpp(float value, float multiplier);

// --- Lote CStats (Stats.cpp, w20), prefijo Stats. Anadir al FINAL de X87Math.h (Ftol2Regs ya esta declarado en el lote Ped) ---

// Palabra de estado (AX de fnstsw) tras comparar st0 con other: condiciones C0/C2/C3 + banderas x87 acumuladas + TOP = 8 - depthAfter (bits 11..13); ES, SF y C1 valen 0.
unsigned int __cdecl StatsCompareDblDblAxCpp(double st0, double other, unsigned int depthAfter); // fcompp / fcomp con los dos operandos en la pila x87
unsigned int __cdecl StatsCompareDblFltAxCpp(double st0, float other, unsigned int depthAfter);  // st0 en la pila, el otro operando es un float de memoria
unsigned int __cdecl StatsCompareFltFltAxCpp(float st0, float other, unsigned int depthAfter);   // fld dword ptr [a]; fcomp dword ptr [b]
unsigned int __cdecl StatsCounterVsFloatAxCpp(unsigned int counter, float threshold);            // fild [contador] (+2^32 si el bit alto esta puesto); fcomp [umbral] (TOP = 0)
unsigned int __cdecl StatsAbsVsZeroAxCpp(float v);                                               // fld [v]; fabs; fcomp 0.0 (TOP = 0)
// Operaciones x87 sueltas cuyos resultados se pasan sin redondear (double) a las comparaciones anteriores.
double __cdecl StatsCounterX87Cpp(unsigned int counter);   // fild + fadd 2^32 si el bit alto esta puesto
double __cdecl StatsScaledLimitCpp(float limit);           // limite * 1000.0f
double __cdecl StatsSquareCpp(float c);                    // c * c
double __cdecl StatsSpeedSumSqCpp(const float* v);         // (x*x + y*y) + z*z con v = {x, y, z}
double __cdecl StatsSpeedCpp(const float* v);              // fsqrt de la suma anterior
// Conversiones con las banderas del x87 del EXE.
float __cdecl StatsIntToFloatCpp(int value);               // fild + fstp dword
int __cdecl StatsFtolCpp(float v);                         // _ftol2 de un float cargado con fld: palabra baja del entero truncado
Ftol2Regs __cdecl StatsStepMsRegsCpp(float step, unsigned int ecxAtCall);                              // _ftol2((paso * 0.02f) * 1000.0f) con los registros de salida del EXE
Ftol2Regs __cdecl StatsAccumulateRegsCpp(int ticks, float k, float base, unsigned int ecxAtCall);       // _ftol2(fild(ticks) [+2^32] * k + base)
Ftol2Regs __cdecl StatsCeilRegsCpp(int ticks, float multiplier, unsigned int ecxAtCall);               // _ftol2(ceil(multiplicador * fild(ticks) [+2^32]))
int __cdecl StatsFlightMinutesCpp(int n, float a, float b);                                              // ftol(fild(n) * a * b) con un redondeo por operacion
// CStats::FindCriminalRatingNumber (0x559080): cadena x87 completa; devuelve EAX del EXE.
int __cdecl StatsCriminalRatingCpp(int kills, int busted, int hospital, int fireLevel, int paramedicLevel, int money5000, int planes, int firesOut,
                                   int vigilante, int ambulance, bool hasCheated, int cheated, int bullets, int hits, float made, float total);
// CStats::GetFatAndMuscleModifier (0x559AF0): los 16 casos; k apunta a la configuracion viva 0x8CDE58..0x8CDEC0.
float __cdecl StatsFatMuscleModifierCpp(unsigned int mode, float fat, float stamina, float muscle, float maxHealth, int cycling, int lung, int bike, int driving, const float* k);

// --- CCam / Cam.cpp (prefijo Cam; lote w20). Ayudantes generados de los bloques __asm de Cam.cpp ---
// CCam::Get_TwoPlayer_AimVector (0x513E40), desde CCam::Get_TwoPlayer_AimVector; fptan, fdiv; sin ramas
void __cdecl CamTwoPlayerAimTanCpp(float fov, float fovScale, float aspect, float* p_tanOverAspect, float* p_tanRounded);
// CCam::LookBehind (0x520690), desde CCam::LookBehind; fsin, fcos; sin ramas
void __cdecl CamLookBehindPedSideCpp(float heading, float* p_x, float* p_y);
// CCam::LookRight (0x520E40), desde LookSideOrbit; fsin, fcos, llamada; sin ramas
void __cdecl CamLookSideOrbitCpp(float directionY, float directionX, unsigned atanFunction, float sign, float halfPi, float distance, float targetX, float* p_outX, float targetY, float* p_outY);
// CCam::RotCamIfInFrontCar (0x50A4F0), desde CCam::RotCamIfInFrontCar; fsin, fcos; sin ramas
void __cdecl CamRotInFrontCarOrbitCpp(float angle, float distance, float baseX, float* p_resultX, float baseY, float* p_resultY);
// CCam::Process (0x526FC0), desde CamVelocityInverseStep; fdiv; sin ramas
void __cdecl CamVelocityInverseStepCpp(float one, float denominator, float* p_reciprocal);
// CCam::Process (0x526FC0), desde CamVehicleTargetSpeed; fsqrt, fdiv; con ramas
void __cdecl CamVehicleTargetSpeedCpp(float* p_forwardY, float* p_forwardX, float zero, float one, float velocityX, float velocityY, float positiveScale, float negativeScale, float half, float* p_result);
// CCam::Process (0x526FC0), desde CamVehicleSmoothedSpeed; solo aritmetica 24 bits; sin ramas
void __cdecl CamVehicleSmoothedSpeedCpp(float target, float targetWeight, float previous, float previousWeight, float* p_result);
// CCam::Process (0x526FC0), desde CamPowerComplement; _CIpow; sin ramas
void __cdecl CamPowerComplementCpp(float base, float step, float one, float* p_result);
// CCam::ProcessPedsDeadBaby (0x519250), desde DeadCamWave; fsin, fdiv; sin ramas
void __cdecl CamDeadWaveCpp(float elapsed, float period, float capped, float factor, float sx, float amplitude, float sy, float* p_scaledY, float* p_y, float tx, float* p_x);
// CCam::ProcessPedsDeadBaby (0x519250), desde CCam::ProcessPedsDeadBaby; solo aritmetica 24 bits; con ramas
void __cdecl CamDeadUnsignedElapsedCpp(int elapsedInt, float unsignedCorrection, float* p_elapsed);
// CCam::ProcessPedsDeadBaby (0x519250), desde CCam::ProcessPedsDeadBaby; fcos, fdiv; sin ramas
void __cdecl CamDeadCosineCpp(float elapsed, float period, float* p_cosine);
// CCam::Process_1rstPersonPedOnPC (0x50EB70), desde CCam::Process_1rstPersonPedOnPC; fsin, fcos; sin ramas
void __cdecl CamFirstPersonPedFrontCpp(float pitch, float yaw, float* p_sineY, float* p_sineZ, float* p_cosineX);
// CCam::Process_AimWeapon (0x521500), desde CamAimOffsets355; fptan, fpatan, fdiv; sin ramas
void __cdecl CamAimOffsetsCpp(float fov, float* p_vResult, float* p_hResult);
// CCam::Process_AimWeapon (0x521500), desde CamAimTargetAngles355; fpatan, fsqrt; sin ramas
void __cdecl CamAimTargetAnglesCpp(float tx, float fx, float ty, float fy, float tz, float fz, float* p_dz, float* p_hResult, float* p_fResult, float* p_pResult);
// CCam::Process_AimWeapon (0x521500), desde CamAimProjection355; fsin, fcos; con ramas
void __cdecl CamAimProjectionCpp(unsigned cam, unsigned scratch, unsigned rowOffset);
// CCam::Process_AimWeapon (0x521500), desde CamAimMeleeBlend355; _CIpow; sin ramas
void __cdecl CamAimMeleeBlendCpp(unsigned scratch);
// CCam::Process_AimWeapon (0x521500), desde CamAimMouseAngles355; solo aritmetica 24 bits; sin ramas
void __cdecl CamAimMouseAnglesCpp(unsigned cam);
// CCam::Process_AimWeapon (0x521500), desde CamAimVehicleResetPitch355; _CIasin; sin ramas
void __cdecl CamAimVehicleResetPitchCpp(float clampedZ, float initialAlpha, float* p_result);
// CCam::Process_AttachedCam (0x512B10), desde CCam::Process_AttachedCam; fsin, fcos; sin ramas
void __cdecl CamAttachedUpCpp(float angle, float sideX, float* p_sinX, float sideY, float* p_sinY, float sideZ, float* p_sinZ, float upX, float* p_cosX, float upY, float* p_cosY, float upZ, float* p_cosZ);
// CCam::Process_Cam_TwoPlayer (0x525E50), desde TwoPlayerCameraGeometry; fsin, fcos; sin ramas
void __cdecl CamDirectionFromAnglesCpp(float alpha, float beta, float* p_directionX, float* p_directionY, float* p_directionZ);
// CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), desde CamShootingSourceZ355; fsin; sin ramas
void __cdecl CamShootingSourceZCpp(float alpha, float radius, float targetZ, float* p_result);
// CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), desde CamShootingOrbit355; fsin, fcos; sin ramas
void __cdecl CamShootingOrbitCpp(unsigned cam, unsigned source, unsigned scratch);
// CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), desde CamShootingTilt355; fpatan, fsqrt, _CIpow; con ramas
void __cdecl CamShootingTiltCpp(unsigned cam, unsigned vehicle, unsigned scratch);
// CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), desde CamShootingFov355; _CIpow; con ramas
void __cdecl CamShootingFovCpp(unsigned cam, unsigned vehicle);
// CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), desde CCam::Process_Cam_TwoPlayer_InCarAndShooting; solo aritmetica 24 bits; sin ramas
void __cdecl CamStoredHeadingMinusHalfPiCpp(float storedHeading, float* p_vehicleHeading);
// CCam::Process_Cam_TwoPlayer_Separate_Cars (0x513510), desde CamTwoCarsBlend355; fsin, fcos; sin ramas
void __cdecl CamTwoCarsBlendWeightCpp(float blend, float* p_sine, float* p_weight);
// CCam::Process_Cam_TwoPlayer_Separate_Cars (0x513510), desde CamTwoCarsBlend355; fsqrt; sin ramas
void __cdecl CamTwoCarsBlendCpp(unsigned scratch, unsigned cam, unsigned firstPos, unsigned secondPos);
// CCam::Process_Cam_TwoPlayer_Separate_Cars (0x513510), desde CamTwoCarsSources355; solo aritmetica 24 bits; sin ramas
void __cdecl CamTwoCarsSourcesCpp(float firstZ, float firstHeight, float* p_liftedFirst, float secondZ, float secondHeight, float* p_liftedSecond, float betweenX, float* p_scaledX, float firstX, float* p_fx, float secondX, float* p_sx, float betweenY, float firstY, float* p_fy, float secondY, float* p_sy, float betweenZ, float* p_scaledZ, float* p_fz, float* p_sz);
// CCam::Process_DW_BirdyCam (0x51B850), desde BirdyCameraEase; fsin; sin ramas
void __cdecl CamEaseCpp(float ratio, float degrees180, float degrees270, float radians, float one, float half, float to, float from, float* p_value);
// CCam::Process_DW_BirdyCam (0x51B850), desde BirdyLampHeight355; solo aritmetica 24 bits; sin ramas
void __cdecl CamBirdyLampHeightCpp(int randomValue, float randomScale, float height, float one, float pointZ, float* p_result);
// CCam::Process_DW_CamManCam (0x51B120), desde CamManFov; fsin, fsqrt, fdiv; con ramas
void __cdecl CamManFovCpp(unsigned target, unsigned source, float distanceScale, float zero, float one, float degrees180, float degrees270, float radians, float half, float final, float initial, float* p_fov, float progress, float timeScale, float early);
// CCam::Process_DW_HeliChaseCam (0x51A740), desde HeliChaseInitialFov; fsin, fdiv; sin ramas
void __cdecl CamHeliInitialFovCpp(float one, float duration, float progress, float degrees180, float degrees270, float radians, float half, float to, float from, float* p_result);
// CCam::Process_DW_HeliChaseCam (0x51A740), desde HeliChaseReturnFov355; fsin, fdiv; con ramas
void __cdecl CamHeliReturnFovCpp(int start, int now, int end, float zero, float one, float degrees, float phase, float radians, float half, float to, float from, float* p_result);
// CCam::Process_DW_HeliChaseCam (0x51A740), desde HeliChaseDistanceFov355; fsin, fsqrt, fdiv; con ramas
void __cdecl CamHeliDistanceFovCpp(float to, int initial, float one, float duration, float progress, float degrees, float phase, float radians, float half, float from, float zero, unsigned camera, unsigned dest, float* p_distance, float nearDistance, float farDistance, float reduction, float* p_result);
// CCam::Process_DW_PlaneCam1 (0x51C760), desde PlaneOrbitWave; fsin, fdiv; sin ramas
void __cdecl CamPlaneOrbitWaveCpp(int start, int now, int end, int cycles, float degrees, float radians, float componentX, float* p_resultX, float componentY, float* p_resultY, float componentZ, float* p_resultZ);
// CCam::Process_DW_PlaneSpotterCam (0x51C250), desde PlaneSpotterRandomCoordinate; solo aritmetica 24 bits; sin ramas
void __cdecl CamPlaneSpotterRandomCpp(int randomValue, float randomScale, float range, float halfRange, float origin, float* p_result);
// CCam::Process_FlyBy (0x5B25F0), desde CCam::Process_FlyBy; fsin, fcos; sin ramas
void __cdecl CamFlyByUpCpp(float roll, float radians, float halfPi, float* p_upX, float* p_upZ);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarBetaBlend355; solo aritmetica 24 bits; con ramas
void __cdecl CamFollowCarBetaBlendCpp(float betaInput, float betaCorrection, float maximumSpeed, float* p_negativeMaximum, float smoothing, float previous, float* p_result);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarHistory355; solo aritmetica 24 bits; sin ramas
void __cdecl CamFollowCarHistoryCpp(float radius, float frontX, float frontY, float frontZ, float* p_scaledZ, float focalX, float* p_outputX, float focalY, float* p_outputY, float focalZ, float* p_outputZ);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarFinalHistory355; fsin, fcos; sin ramas
void __cdecl CamFollowCarFinalHistoryCpp(unsigned cam, unsigned scratch);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarBetaCorrection355; fpatan, fsqrt, fdiv; con ramas
void __cdecl CamFollowCarBetaCorrectionCpp(unsigned cam, unsigned vehicle, unsigned scratch, unsigned front);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarActualDistance355; fsqrt; sin ramas
void __cdecl CamFollowCarActualDistanceCpp(unsigned origin, unsigned previous, float* p_result);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarAutoAlpha355; _CIpow; con ramas
void __cdecl CamFollowCarAutoAlphaCpp(float base, float step, float wanted, float previous, float* p_result, float maximum);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarAlphaBlend355; solo aritmetica 24 bits; sin ramas
void __cdecl CamFollowCarAlphaComplementCpp(float smoothing, float* p_complement);
// CCam::Process_FollowCar_SA (0x5245B0), desde CamFollowCarAlphaBlend355; solo aritmetica 24 bits; con ramas
void __cdecl CamFollowCarAlphaBlendCpp(unsigned cam, unsigned scratch);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseFinalDistance355; fsqrt, _CIpow, fdiv, llamada; con ramas
void __cdecl CamMouseFinalDistanceCpp(unsigned cam, unsigned source, unsigned scratch);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseChanges355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMouseChangesStickCpp(float inputY, float fov, float scaleFactor, float stickBeta, float step, float stickScale, float inputX, float* p_betaResult, float stickAlpha, float* p_alphaResult);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseChanges355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMouseChangesMouseCpp(float inputX, float mouseBeta, float inputY, float mouseAlpha, float fov, float scaleFactor, float betaSensitivity, float* p_betaResult, float alphaSensitivity, float* p_alphaResult);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseRepeatedRadius355; fptan; sin ramas
void __cdecl CamMouseRepeatedRadiusCpp(float fov, float radians, float half, float aspect, float nearPlane, float factor, float* p_result);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMousePerpendicularDistance355; fsqrt, fdiv; sin ramas
void __cdecl CamMousePerpendicularDistanceCpp(float one, float radiusFactor, float* p_inverse, unsigned collision, unsigned source, unsigned front, float* p_zDifference, float* p_yProjection, float* p_zProjection, float* p_xPerpendicular, float* p_result);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseSphereCenter355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMouseSphereCenterCpp(unsigned front, unsigned source, unsigned result, float nearPlane, float* p_zProduct);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseInitialSource355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMouseInitialSourceCpp(unsigned target, unsigned front, unsigned result, float distance, float* p_zProduct);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseFirstHitDistance355; fsqrt; sin ramas
void __cdecl CamMouseFirstHitDistanceCpp(unsigned target, unsigned hit, float* p_result);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMousePullSource355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMousePullSourceCpp(unsigned target, unsigned source, float* p_zDifference, float factor, float* p_xProduct);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CamMouseRecenterDelta355; solo aritmetica 24 bits; sin ramas
void __cdecl CamMouseRecenterDeltaCpp(float orientation, float beta, float* p_result);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CCam::Process_FollowPedWithMouse; fcos; con ramas
void __cdecl CamFollowMouseDistanceCpp(float alpha, float alphaMultiplier, float* p_distance);
// CCam::Process_FollowPedWithMouse (0x50F970), desde CCam::Process_FollowPedWithMouse; fptan; sin ramas
void __cdecl CamFollowMouseRadiusCpp(float fov, float radians, float half, float factor, float* p_radiusFactor);
// CCam::Process_FollowPed_SA (0x522D40), desde CameraRound4; _CIpow; con ramas
void __cdecl CamRound4ScaleCpp(int five, float value, float* p_scaled);
// CCam::Process_FollowPed_SA (0x522D40), desde CameraRound4; _CIpow, fdiv; sin ramas
void __cdecl CamRound4FinishCpp(float integral, int four, float* p_result);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedAutoAlpha355; _CIpow; con ramas
void __cdecl CamFollowPedAutoAlphaCpp(float base, float step, float wanted, float previous, float* p_result, float maximum);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedPitchCorrection355; solo aritmetica 24 bits; con ramas
void __cdecl CamFollowPedPitchCorrectionCpp(float wantedPitch, float followWeight, float followMaximum, float* p_maximum, float taskWeight, float historyPitch, float* p_negativeMaximum, float verticalOffset, float* p_result);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedDistance355; fsqrt; con ramas
void __cdecl CamFollowPedDistanceCpp(unsigned origin, unsigned previous, float* p_actual, float* p_distance, float rowMinimum, float minimumDistance);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedLimitHistoryPitch355; _CIpow, fdiv; con ramas
void __cdecl CamFollowPedHistoryPitchCpp(float separation, float* p_maximum, float step, float* p_historyPitch);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedHistoryBase355; solo aritmetica 24 bits; sin ramas
void __cdecl CamFollowPedHistoryBaseCpp(float step, float speed, float wanted, float* p_result);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedResetHistory355; solo aritmetica 24 bits; sin ramas
void __cdecl CamFollowPedResetHistoryCpp(unsigned origin, unsigned direction, unsigned firstOut, float firstDistance, float* p_zProduct, unsigned secondOut, float secondDistance);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedHistory355; fsin, fcos; sin ramas
void __cdecl CamFollowPedHistoryCpp(float historyDelta, float verticalOffset, float* p_angle, float* p_cosAlpha, float beta, float* p_xDirection, float* p_zDirection, float historyDistance, float* p_xProduct, float* p_yProduct, unsigned origin, unsigned firstOut, float distance, unsigned secondOut);
// CCam::Process_FollowPed_SA (0x522D40), desde CamFollowPedHistoryHeading355; fpatan; con ramas
void __cdecl CamFollowPedHistoryHeadingCpp(float frontX, float frontY, float* p_result);
// CCam::Process_M16_1stPerson (0x5105C0), desde CamM16Blend355; solo aritmetica 24 bits; sin ramas
void __cdecl CamM16BlendCpp(float smoothing, float* p_complement, float betaInput, unsigned betaSpeed, unsigned alphaSpeed, float alphaInput, float* p_alphaRounded, unsigned betaAngle, unsigned alphaAngle);
// CCam::Process_M16_1stPerson (0x5105C0), desde CamM16Bump355; fcos, _CIpow, fdiv; con ramas
void __cdecl CamM16BumpCpp(int signedElapsed, unsigned horz, unsigned beta, unsigned vert, unsigned alpha, float damping, float step);
// CCam::Process_M16_1stPerson (0x5105C0), desde CamM16StickInputs355; solo aritmetica 24 bits; con ramas
void __cdecl CamM16StickInputsCpp(float vertical, int attached, float horizontal, float fov, float step, float* p_betaResult, float* p_alphaResult, float* p_horizontalSign);
// CCam::Process_M16_1stPerson (0x5105C0), desde CamM16MouseAngles355; solo aritmetica 24 bits; sin ramas
void __cdecl CamM16MouseAnglesCpp(float horizontal, float fov, float* p_scale, float vertical, unsigned beta, unsigned alpha);
// CCam::Process_M16_1stPerson (0x5105C0), desde CamM16SideRay355; fsin, fcos; con ramas
void __cdecl CamM16SideRayCpp(float beta, int positive, float alpha, float* p_xDirection, float* p_yDirection, float* p_yProduct, float* p_zProduct, unsigned origin, unsigned outputVector);
// CCam::Process_Rocket (0x511B50), desde CCam::Process_Rocket; fsin, fcos; sin ramas
void __cdecl CamRocketFrontCpp(float verticalAngle, float horizontalAngle, float* p_frontX, float* p_frontY, float* p_frontZ);
// CCam::Process_Rocket (0x511B50), desde CCam::Process_Rocket; llamada; sin ramas
void __cdecl CamRocketHeadingCpp(float atanY, float atanX, unsigned atanFn, float halfPi, float* p_heading);
// CCam::Process_WheelCam (0x512110), desde WheelCameraUp; fsin, fcos; sin ramas
void __cdecl CamWheelUpCpp(int phase, float angleScale, float amount, float sx, float* p_lateralX, float sy, float* p_lateralY, float sz, float ux, float uy, float* p_verticalY, float uz, float* p_verticalZ, float* p_outputX, float* p_outputY, float* p_outputZ);
// CameraPathInterpolateVector (0x5B2090): spline de la ruta de camara (traduccion del naked, pila emulada en un marco local; _ftol2 como (long long)); los argumentos cdecl llegan como palabras de 32 bits (t = bits del float)
void __cdecl CamPathInterpolateVectorCpp(unsigned out, unsigned knots, unsigned tBits, unsigned index);
// CameraPathInterpolateScalar (0x5B2330): spline de la ruta de camara (traduccion del naked, pila emulada en un marco local; _ftol2 como (long long)); los argumentos cdecl llegan como palabras de 32 bits (t = bits del float)
void __cdecl CamPathInterpolateScalarCpp(unsigned out, unsigned knots, unsigned tBits, unsigned index);

// --- CBike (Bike.cpp; lote w20, prefijo Bike). Reutiliza PowScaleVector3Cpp, PowScaleFloatCpp, CnvDecayPowCpp, QuadBikeDampDeltaFullCpp, TrigSimpleSinCpp/CosCpp/SinMulCpp, BmxSinScaledCpp/CosScaledCpp y UnsignedPlusFloatCpp. ---
// CBike::CBike (0x6BF59E..0x6BF5BD): float(fptan(degrees * 0.017453292f)); el producto de 24 bits va a fptan sin pasar por float.
float __cdecl BikeTanScaledCpp(float degrees, float k);
// float(1 - cos(angle)) con el coseno sin redondear (CBike::CalculateLeanMatrix 0x6B7211) y float(((1 - cos(lean)) * minZ) * 0.9f) (CBike::PreRender 0x6BDD56).
float __cdecl BikeOneMinusCosCpp(float angle);
float __cdecl BikeChassisHeightCpp(float lean, float minZ);
// CBike::PreRender (0x6BD2B3): out[i] = float(float(sin(lean) * minZ * 0.8f) * right[i] + point[i]); out, point y right apuntan a x, y, z consecutivos.
void __cdecl BikeContactOffsetCpp(float* out, const float* point, const float* right, float lean, float minZ);
// CBike::PreRender (0x6BDB11): float(asin((height - original) / forkLength) * -1.0f) con el _CIasin del EXE.
float __cdecl BikeForkAngleCpp(float height, float original, float forkLength);
// CBike::PlaceOnRoadProperly (0x6BF0F1): rellena right (m[0..2]) y forward (m[4..6]) de una CMatrix; el resto de la matriz no se toca.
void __cdecl BikeRoadOrientationCpp(float* m, float rear, float front, float backX, float backY, float backZ, float frontX, float frontY, float frontZ);
// CBike::PlaceOnRoadProperly (0x6BF167): punto medio entre el punto delantero y el trasero; out apunta a x, y, z.
void __cdecl BikeRoadMidpointCpp(float frontX, float backX, float frontY, float backY, float frontZ, float backZ, float* out);
// CBike::FixHandsToBars (0x6B8274): bonePosition (x, y, z consecutivos) += delta * factor con el orden de redondeo del EXE.
void __cdecl BikeMoveHandBoneCpp(float* bonePosition, const float* delta, float factor);
// CBike::VehicleDamage (0x6B91C2): *health = *health - damage * factor.
void __cdecl BikeHealthMinusCpp(float* health, float damage, float factor);
// CBike::SetupSuspensionLines (0x6B8C10 / 0x6B8CFB / 0x6B8D6D): z del extremo de la linea, altura sobre la carretera y ajuste de una esfera de colision (sphere = x, y, z, radio).
float __cdecl BikeSetupEndCpp(float upper, float lower, float wheel, float startZ);
float __cdecl BikeSetupHeightCpp(float force, float divisor, float suspensionLength, float wheel, float startZ);
void __cdecl BikeSetupSphereCpp(float* sphere, float limit);
// CBike::ProcessRiderAnims (0x6B7932 / 0x6B7CF1 / 0x6B7EBB / 0x6B7C16): suavizado con pow, pesos del conductor, ruido aleatorio y limite hacia delante.
float __cdecl BikeRiderSmoothCpp(float base, float step, float previous, float target);
void __cdecl BikeRiderWeightsCpp(float lateral, float forward, float available, float* lateralOut, float* forwardOut);
float __cdecl BikeRiderJitterCpp(float speed, int randomValue);
float __cdecl BikeRiderForwardLimitCpp(float lean, float target);
// CBike::ProcessControlInputs (0x6BE497 / 0x6BE504 / 0x6BE8B4 / 0x6BE31B / 0x6BE98C): mezcla de entradas, producto con suma, direccion, velocidad hacia delante y limite de velocidad.
float __cdecl BikeControlBlendCpp(float oldValue, int input, float step, float factor);
float __cdecl BikeControlProductAddCpp(float oldValue, float input, float factor);
float __cdecl BikeInputSteerCpp(float degrees, float steer);
float __cdecl BikeForwardSpeedCpp(const float* speed, const float* forward);
void __cdecl BikeSpeedCapCpp(float* speed, float limit);
// CBike::ProcessControl (0x6B9250): fusiones con asin del EXE para la inclinacion y el manillar (caballete parado 0x6B9C04, caballete con conductor 0x6BB991, libre 0x6BBB7C, manillar 0x6BAB8E).
float __cdecl BikeStoppedLeanCpp(float keep, float desired, float sine);
float __cdecl BikeSideStandLeanCpp(float keep, float desired, float rightZ, float offset);
float __cdecl BikeFreeLeanCpp(float keep, float desired, float sine, float offset);
float __cdecl BikeSteerBarCpp(float sine, float steeringLock, float steer, bool doubleIt);

// --- CPlane (Plane.cpp; lote w20, prefijo Plane). Anadir al FINAL de X87Math.h ---
// Cadenas x87 de CPlane (VehicleDamage 0x6CC4B0, ProcessControlInputs 0x6CADD0, ProcessFlyingCarStuff 0x6CB7C0, PreRender 0x6C94A0): 24 bits por operacion, ST0 sin redondear entre ellas.
// VehicleDamage: v = mass / scale (si minimumOne y 1.0 > v, v = 1.0); float(v * threshold).
float __cdecl PlaneMassThresholdCpp(float mass, float scale, float threshold, bool minimumOne);
// float(health - damage * factor).
float __cdecl PlaneHealthAfterDamageCpp(float health, float damage, float factor);
// Delta float(nodePosition - relative) por eje, suma (x² + y²) + z²; si suma < closest² (ordenado) closest = float(sqrt(suma)) y devuelve true. Punteros a x, y, z consecutivos.
bool __cdecl PlaneSelectCloserCpp(const float* nodePosition, const float* relative, float* closest);
// ProcessControlInputs: float(((fild(input) * scale - old) * step) * factor + old).
float __cdecl PlaneControlBlendCpp(float oldValue, int input, float scale, float step, float factor);
// (z·z' + y·y') + x·x' sobre dos vectores x, y, z consecutivos (velocidad · direccion), guardado como float.
float __cdecl PlaneForwardSpeedCpp(const float* speed, const float* forward);
// Cierre a [-1, 1] del deslizamiento: float(((target - old) * step) * factor + old), !(v > -1) -> -1 (NaN incluido), !(v < 1) -> 1.
float __cdecl PlaneInputSkidCpp(float oldValue, float target, float step, float factor);
// float(-(max(1 - speed, minimum) * ((degrees * radians) * skid))).
float __cdecl PlaneGroundSteerCpp(float speed, float degrees, float skid, float minimum, float radians);
// Entero bajo de _ftol2(((fild(axis) * step) * rate) * scale); el llamador toma el int16 bajo.
int __cdecl PlaneNozzleChangeCpp(int axis, float step, float rate, float scale);
// *gear = float(step * factor + *gear); devuelve true si el valor sin redondear >= limit (ordenado).
bool __cdecl PlaneGearAdvanceCpp(float* gear, float step, float factor, float limit);
// sqrt((x² + y²) + z²) < limit (ordenado) sobre un vector x, y, z consecutivo.
bool __cdecl PlaneSpeedBelowCpp(const float* speed, float limit);
// Si l = sqrt((x² + y²) + z²) > limit, cada componente = float((limit / l) * componente).
void __cdecl PlaneSpeedCapCpp(float* speed, float limit);
// float((drag / divisor) * factor).
float __cdecl PlaneGearResistanceCpp(float drag, float divisor, float factor);
// ProcessFlyingCarStuff: incremento (entero sin signo) del contador de onda: ver Plane-x87.cpp.txt (0x6CB7E0..0x6CB85B).
unsigned int __cdecl PlaneWaveAdvanceCpp(float spread, float step, int random, float randomScale, float timeScale, float countScale);
// min(sqrt((x² + y²) + z²) * 3.0, 1.0) como float (el 1.0 sustituye solo si el producto es mayor, ordenado).
float __cdecl PlaneDamageSpeedCpp(const float* speed);
// Ruido de daño de un eje/panel: float(((((((amplitude - (-amplitude)) * (fild(random) * randomScale)) + (-amplitude)) * squaredDamage) * step) * speed) + old).
float __cdecl PlaneDamageNoiseCpp(float oldValue, float amplitude, int random, float squaredDamage, float step, float speed, float randomScale);
// Onda de las helices (fsin sin redondear): float(((((sin(fase) - 1) * ((upper - lower) * fraccion + lower)) * daño) * daño) * step) * amount + old).
float __cdecl PlanePropellerWaveCpp(float oldValue, float upper, float lower, int random, int damage, unsigned int wave, unsigned int period, float step, float randomScale, float circle, float amount);
// float((1 - fild(damage) * scale) * old); storeFactor guarda el factor como float antes (fstp + fld).
float __cdecl PlaneFlightDecayCpp(float oldValue, int damage, float scale, bool storeFactor);
// subtract: float(value - a * b); si no float(a * b + value).
float __cdecl PlaneFlightProductAddCpp(float value, float a, float b, bool subtract);
// float(((target - value) * step) * factor + value).
float __cdecl PlaneFloatBlendCpp(float value, float target, float step, float factor);
// ((dz² + dy²) + dx²) < threshold con d = first - second sin redondear (ordenado).
bool __cdecl PlaneDistanceBelowCpp(const float* first, const float* second, float threshold);
// |rollFn(plane)| contra threshold, con el ST0 sin redondear que deja CPlaceable::GetRoll (rollFn se llama como __fastcall, ECX = plane): greater -> mayor que, si no menor que.
bool __cdecl PlaneRollCompareCpp(float(__fastcall* rollFn)(const void*, void*), const void* plane, float threshold, bool greater);
// float(max(height * scale, 0) * target) (0.0 sustituye solo si 0.0 > h, ordenado).
float __cdecl PlaneCeilingTargetCpp(float height, float target, float scale);
// float(max(acceleration - height * adjust, -1)) (-1.0 sustituye solo si -1.0 > v, ordenado).
float __cdecl PlaneCeilingAccelerationCpp(float height, float acceleration, float adjust);
// subtract: float(1.0 - spread); si no float(spread + 1.0).
float __cdecl PlaneFlightBoundCpp(float spread, bool subtract);
// out = { float((v.x * quarter) * multiplier), float((v.y * quarter) * multiplier), float(float(v.z * quarter) * multiplier) }.
void __cdecl PlaneSmokeParticleVelocityCpp(float* out, const float* speed, float quarter, float multiplier);
// float(((fild(random) * randomScale) * spread + base) * value).
float __cdecl PlaneVelocityNoiseCpp(float value, int random, float randomScale, float spread, float base);
// negativo = float(-strength); float((((strength - negativo) * (fild(random) * randomScale)) + negativo) + value).
float __cdecl PlanePositionNoiseCpp(float value, float strength, int random, float randomScale);
// standard; acceleration > 0 -> float((maximum - standard) * a + standard); acceleration < 0 -> float((standard - minimum) * a + standard).
float __cdecl PlanePropellerTargetCpp(float acceleration, float standard, float maximum, float minimum);
// ((x² + y²) + z²) > threshold (double, ordenado).
bool __cdecl PlaneSpeedSquaredAboveCpp(const float* speed, double threshold);
// PreRender: L = (compression - (1 - spring/line)) / (1 - (1 - spring/line)), guardado como float.
float __cdecl PlaneWheelCompressionCpp(float springLength, float lineLength, float compression);
// PreRender: altura de rueda (posZ + lowerLimit, - L*spring si L > 0, suavizado 0.75 salvo snap), guardada como float.
float __cdecl PlaneWheelHeightCpp(float posZ, float lowerLimit, float compression, float springLength, float oldHeight, bool snap);
// PreRender (Andromada): rotacion de rueda; compression >= 1 -> rotation * 0.95; si no float(-asin(clamp(dot, -1, 1))) con el _CIasin del EXE.
float __cdecl PlaneAndromadaWheelRotationCpp(float rotation, float compression, const float* normal, const float* forward);
// PreRender: angulo de las helices: v = step * propSpeed + angle; si v > fullCircle se resta fullCircle (al menos una vez) mientras v > fullCircle.
float __cdecl PlanePropAngleCpp(float timeStep, float propSpeed, float angle, float fullCircle);
// PreRender: *scale = float((1 - |gear|) * half + 1), *offset = float((1 - |gear|) * third).
void __cdecl PlaneGearFactorsCpp(float gear, float half, float third, float* scale, float* offset);
// PreRender: float(((amplitude * radians) * control) [operation 1: + addend, 2: - addend, 3: cambio de signo]).
float __cdecl PlaneSurfaceAngleCpp(float amplitude, float radians, float control, float addend, int operation);
// PreRender: vector[i] = float(vector[i] - position[i]) para x, y, z.
void __cdecl PlaneSubtractPositionCpp(float* vector, const float* position);
// PreRender: float((((1 - finScale) * boxMaxY) * factor) + y).
float __cdecl PlaneFinOffsetCpp(float finScale, float boxMaxY, float y, float factor);
// PreRender: float(direction * propAngle) duplicado (v + v) o con el signo cambiado (fchs).
float __cdecl PlanePropRotationCpp(float direction, float propAngle, bool doubled);
// PreRender: float(|gear| * value).
float __cdecl PlaneGearRotationCpp(float gear, float value);
// PreRender (Hydra): float((fild(misc) * halfPi) / limit).
float __cdecl PlaneNozzleAngleCpp(int misc, float halfPi, int limit);
// PreRender (Hydra): float((acceleration + one) * half).
float __cdecl PlaneDustTimeCpp(float acceleration, float one, float half);
// PreRender: out[i] = float(a[i] + b[i]), i = 0..2.
void __cdecl PlaneSumVector3Cpp(const float* a, const float* b, float* out);
// PreRender: byte bajo = alfa de la estela: low32(_ftol2(max(|(((vz·az + vy·ay) + ax·vx) * attackLift) * scale| - offset, 0))).
unsigned int __cdecl PlaneStreakAlphaCpp(const float* speed, const float* up, float attackLift, float scale, float offset);
// PreRender (Cropduster/Stunt): out = { x*scale, y*scale, z*scale [- zOffset si subtractOffset] } como float.
void __cdecl PlaneSmokeVelocityCpp(const float* speed, float scale, float zOffset, bool subtractOffset, float* out);
// PreRender: fld; fchs; fstp (-value, con la quietud del fld).
float __cdecl PlaneNegateCpp(float value);
// PreRender (Vortex): inclinacion del nodo 23 sobre la matriz temporal; ver plane-x87.cpp.txt. lines = CColLine[4] (0x20 bytes: start.x en [0], start.y en [1]).
void __cdecl PlaneHoverTiltCpp(float* rightZ, float* forwardZ, float* upX, float* upY, float* upZ, const float* matrixRight, const float* matrixForward, const float* velocity,
                               const float* wheelPositions, const float* lines, float wheelZ, float wheelSize, float timeStep);

// --- Lote CStats (statsfix3): captura de ECX/EDX tras una llamada ---
// Se llama a traves de un puntero __fastcall SIN parametros justo despues de un colaborador: el compilador no carga nada en ECX/EDX y la funcion devuelve
// ECX en la mitad baja y EDX en la alta (compilada /O2 es `mov eax,ecx; ret`; comprobada al cargar la DLL).
unsigned long long __fastcall StatsCaptureEcxEdxCpp(unsigned int ecx, unsigned int edx);
// EAX con que sale el colaborador anterior (mismo uso: llamada inmediata tras el colaborador; sin `return`, MSVC no escribe EAX). Distinto de EntryEaxCpp: aquel solo
// vale como primera lectura del EAX de entrada de la funcion (lo vigila el auditor); este se vigila con su propia regla (violations_before_capture).
unsigned int __fastcall StatsCaptureEaxCpp();

// --- CStats::UpdateSexAppealStat (0x55BF20, Stats.cpp, w20), prefijo SexAppeal. Anadir al FINAL de X87Math.h ---

// 0x55BF23: fld dword [apariencia]; fmul dword [0x858B8C] (0.5f); fstp dword: escribe el float resultante (bits) en *out; no devuelve nada en ST0.
void __cdecl SexAppealHalfCpp(float appearance, unsigned int* out);
// 0x55BF90..0x55BFB9: dx = target[0] - player[0], dy = target[1] - player[1]; fsqrt(dy*dy + dx*dx); fcomp 35.0f; devuelve 1 si distancia < 35 (NaN y >= 35: 0).
// Deja en la pila x87 vacia el mismo residuo que el EXE (registros 2, 3 y 4: dy, distancia y dx*dx).
unsigned int __cdecl SexAppealNearCpp(const float* target, const float* player);
// 0x55BFC5..0x55C04C: salud del vehiculo -> float(2*salud - 1000) guardado, limitado a [0, 1000] (NaN se conserva) y multiplicado por el factor de la clase
// (factorIndex 0 = x0.1f, 1 = sin factor, 2 = x0.3f, 3 = x0.5f); ST0 sin redondear a float.
double __cdecl SexAppealVehiclePartCpp(float health, unsigned int factorIndex);
// 0x55C04C..0x55C093: valor = parte * 0.5f + float(halfBits) (los bits que dejo SexAppealHalfCpp); limite inferior 0 y superior 1000 (NaN se conserva); cheat != 0 -> 1000; fstp dword [0xB793E4] (STAT_SEX_APPEAL).
void __cdecl SexAppealFinishCpp(double vehiclePart, unsigned int halfBits, unsigned int cheat);

// --- CPlayerPed::ControlButtonSprint (0x60A610; prefijo PlayerPedSprint) ---
// 0x60A69E: float(f0 + velocidad) o f10 si f10 < f0 + velocidad (el valor que el EXE guarda en datos+0x1C con fstp).
float __cdecl PlayerPedSprintAddClampCpp(float f0, float speed, float f10);
// 0x60A6E3 / 0x60A72C: float(velocidad - paso * k) o limite si limite > velocidad - paso * k (limite 1.0f o 0.0f).
float __cdecl PlayerPedSprintDecayCpp(float timeStep, float k, float speed, float limit);
// 0x60A776: float(velocidad / umbral).
float __cdecl PlayerPedSprintRatioCpp(float speed, float threshold);
// 0x60A7A9..0x60A7D3: ((max(ratio - 1, 0) * k) + 1) con ST0 sin guardar (double, extern "C"); PlayerPed.cpp lo usa como float con otro nombre via /alternatename, no por este encabezado.
extern "C" double __cdecl PlayerPedSprintTailDblCpp(float ratio, float k);

// --- CTaskSimpleFight: FightHitCar (0x61D0B0) y FightStrike (0x6240B0) (prefijo Fight; lote gc) ---
// 0x61D1CB..0x61D1DD / 0x61D210..0x61D221: float((masa * danio) * escala): fld masa; fmul danio; fmul escala; fstp dword (dos productos a 24 bits, el intermedio sin guardar).
float __cdecl FightHitCarIntensityCpp(float mass, float damage, float scale);
// 0x6242A0..0x6242BF: alcance = float(radioEnvolvente + radioGolpe); si el golpe es fuerte (bigHit): float(radioGolpe * 0.5f + alcance) (fstp dword [esp+14h] y relectura).
float __cdecl FightStrikeReachCpp(float boundRadius, float hitRadius, bool bigHit);
// 0x62432A..0x624385 (ped): distancia en el plano (dx*dx + dy*dy) a 24 bits frente a alcance*alcance; true solo si es menor (fcompp + test ah,5 + jp: orden estricto, NaN -> false).
bool __cdecl FightStrikeWithin2DCpp(const float* centre, const float* position, float reach);
// 0x62434E..0x624385 (vehiculo/objeto): ((dx*dx + dy*dy) + dz*dz) frente a alcance*alcance.
bool __cdecl FightStrikeWithin3DCpp(const float* centre, const float* position, float reach);
// 0x6243D2..0x624435: direccion = centro - posicion (float, se guarda en `direction`); true si ((dx*dx + dy*dy) + dz*dz) < (radioEsfera + radioGolpe)^2 (el radio sumado sin guardar).
bool __cdecl FightStrikeSphereHitCpp(const float* sphere, const float* position, float hitRadius, float* direction);
// 0x624456..0x6244BA: posicion += adelante * 1.5f * radio (x: float(adelante.x * 1.5f) guardado y relectura; y: sin guardar; z: guardado), cada componente se guarda en la posicion.
void __cdecl FightStrikeAdvanceCpp(const float* forward, float radius, float* position);

// --- CTaskSimpleGangDriveBy (lote w21 D; prefijo GangDriveBy). Anadir al FINAL de X87Math.h ---
// PlayerTarget (0x621960, 0x6219E1..0x621A5F): cuadrante 0..3 (byte con signo del _ftol2) de ((atan2(-aimX, aimY) - rumbo del vehiculo) ajustado a +-pi + pi/4, + 2pi si < 0) * 2/pi, sin guardar como float.
// forward = &matriz del vehiculo->forward (x, y consecutivos) o nullptr (heading = placement.m_fHeading).
int __cdecl GangDriveByPlayerQuadrantCpp(float aimX, float aimY, const float* forward, const float* heading);
// AITarget (0x627600, 0x627858..0x62787B): igual para el angulo ya guardado como float.
int __cdecl GangDriveByAiQuadrantCpp(float angle);
// AITarget (0x627600, 0x627657..0x627741): out = {dx, dy, dz, distancia, angulo} (ver X87Math.cpp); target y pos apuntan a x, y, z.
void __cdecl GangDriveByAiGeometryCpp(const float* target, const float* pos, const float* forward, const float* heading, float* out);
// AITarget (0x6277E7..0x62780B): out = {dx + pos.x, dy + pos.y, dz + pos.z} (delta = {dx, dy, dz}); deja las sumas z, y, x en los registros muertos T-1, T-2 y T-3 como el EXE.
void __cdecl GangDriveByAimPointCpp(const float* delta, const float* pos, float* out);
// AITarget: comparaciones ordenadas fld [a]; fcomp [b] con el resultado de los saltos del EXE (Gt: a > b, Lt: a < b, Ge: a >= b, Le: a <= b; con NaN, false).
bool __cdecl GangDriveByGtCpp(const volatile float* a, const volatile float* b);
bool __cdecl GangDriveByLtCpp(const volatile float* a, const volatile float* b);
bool __cdecl GangDriveByGeCpp(const volatile float* a, const volatile float* b);
bool __cdecl GangDriveByLeCpp(const volatile float* a, const volatile float* b);
// AimGun (0x628350, 0x6283CB..0x628403): float(sqrt((dz^2 + dy^2) + dx^2) + 20.0f) con d = pos - source.
float __cdecl GangDriveByAimRangeCpp(const float* pos, const float* source);
// AimGun (jugador, 0x628422..0x628460): out = {pos.x, pos.y pasado por la x87, pos.z + 0.7f}.
void __cdecl GangDriveByMuzzleCpp(const float* pos, float* out);
// AimGun (0x628350, 0x6285F4..0x6286DB): elevacion de la rama de moto. radianFn = CGeneral::GetRadianAngleBetweenPoints reinterpretado con bits en x1, x2 e y2 (se llama una vez); forward != nullptr (lee forward[2]).
float __cdecl GangDriveByBikeElevationCpp(float (__cdecl* radianFn)(unsigned int, float, unsigned int, unsigned int), const float* target, const float* gun, float baseAngle, const float* forward, const float* heading, float lean);

// --- Grupo E (ge): 0x610DB0, 0x72D6D0 y 0x63A380 ---
// 0x610DB0: fild random; fmul 2^-15; fimul range; _ftol2; devuelve los 32 bits bajos (el llamador suma el limite inferior). random ya viene enmascarado a 16 bits.
int __cdecl PopulationRandomRangeCpp(int random, int range);
// 0x72D6D0 (CZoneDef::GetCenter): zona = 8 int16 {esquinaX, esquinaY, vec1X, vec1Y, vec2X, vec2Y, minZ, maxZ}; out = {x, y, z} (centro, fild/fiadd/fmul 0.5/fiadd/fstp).
// Los enteros llegan ya extendidos como parametros (zSum = minZ + maxZ, y5 = vec2Y, y3 = vec1Y, y1 = esquinaY, x4 = vec2X, x2 = vec1X, x0 = esquinaX) para que el compilador emita fimul/fiadd.
void __cdecl CullZoneCenterCpp(int zSum, int y5, int y3, int y1, int x4, int x2, int x0, float* out);
// 0x63A460..0x63A49A: true si |valor| > 20 (movsx; fild; fabs; fcomp 20.0f; test ah, 41h; je), con el estado x87 de la comparacion como el EXE.
bool __cdecl EnterCarCamStickBeyondCpp(short value);
// 0x63A4CE..0x63A53F (CTaskComplexEnterCar): out = {rumbo - pi/2, escalar coche->peaton . eje derecho, atan2(escalar, distancia) * 0.7f}, los tres como float.
// matrix: matriz del coche o nullptr (rumbo = placementHeading; como el EXE, el escalar desreferencia matrix + 8 y falla). carPos/pedPos: x, y, z consecutivos.
void __cdecl EnterCarCamLateralCpp(const float* matrix, float placementHeading, const float* carPos, const float* pedPos, float distance, float* out);
// 0x63A548..0x63A67A (CTaskComplexEnterCar): camMinY = CColModel::m_boundBox.m_vecMin.y del coche; choice = byte [tarea+0x29] (0..2); t1 = float [0x86E684 + 4 * choice];
// t2 = float [0x86E678 + 4 * choice]; limit = float [0x8D2ECC]. Actualiza *betaStick y, si *vertical > limit, *alphaStick. vertical/horizontal solo se leen.
void __cdecl EnterCarCamStickCpp(float camMinY, float distance, float angle, float heading, float dot, unsigned int choice, float t1, float t2, float limit,
                                 const volatile float* vertical, const volatile float* horizontal, float* alphaStick, float* betaStick);

// --- Grupo G (BRIEF_21): CQuaternion::Slerp ---
// CQuaternion::Slerp del EXE (0x59C300): out/from/to = x, y, z, w consecutivos (out puede coincidir con from o to). theta == 0 copia `to`; si no, mezcla from y to con los pesos
// A = sin((1 - t)·theta')·invSin y B = ±sin(theta'·t)·invSin (theta' = float(pi - theta) si theta > pi/2). Aritmetica x87 de 24 bits, senos sin redondear, estado x87 final como el del EXE.
void __cdecl QuaternionSlerpCpp(float* out, const float* from, const float* to, float theta, float invSin, float t);

// --- CVehicle::DoHeadLightEffect (0x6E0A50) y CVehicle::DoTailLightEffect (0x6E1780), lote g22b, prefijo VehHeadTail. Anexar al FINAL de X87Math.h. ---
// Geometria de DoTailLightEffect (0x6E1820..0x6E18CC): offset = dummy (x = x - (x + x) si mirror), mismo world/delta/magnitude y dot = ((-fz*dz) + (-fy*dy)) + (-fx*dx).
void __cdecl VehHeadTailTailGeometryCpp(const float* dummy, bool mirror, const float* matrix, const float* camera, float* offset, float* magnitude, float* dot);
// Tamano, rango y rojo de la corona de DoTailLightEffect (0x6E1916..0x6E19E6); mode 1 = freno (a * 128), 2 = a * 96, 0 = rojo 0.
void __cdecl VehHeadTailTailCoronaCpp(float magnitude, float dot, float lodMultiplier, bool train, int mode, float* size, float* range, unsigned int* red);

// --- Grupo g22c (BRIEF_21): CCam::WrapAngle y FxEmitter_c::CreateParticles. Anadir al FINAL de X87Math.h ---
// CCam::WrapAngle (0x509BE0): while (a >= pi) a -= 2pi; while (a < -pi) a += 2pi sobre *angle (cdecl, ST0 sin redondear entre pasos). Deja la pila x87, las banderas y C0-C3 como el EXE.
void __cdecl CamWrapAngleCpp(float* angle);
// FxEmitter_c::CreateParticles (0x4A41E0), tramo 0x4A421C..0x4A4331: visibilidad por distancia, *intensity = count * vis * (rateMult * 0.001f) + *intensity (se guarda siempre) y los filtros
// de viento, lluvia e intensidad >= 1. Devuelve false si el EXE sale (dist, minWind..maxRain, wind y rain se leen de memoria como floats; lodStart/lodEnd son los uint16 en bruto, /64 dentro).
bool __cdecl FxEmitterAccumulateCpp(float* intensity, const float* dist, unsigned int rateMult, unsigned int lodStartRaw, unsigned int lodEndRaw, float count,
                                    const float* minWind, const float* maxWind, const float* minRain, const float* maxRain, const float* wind, const float* rain);
// _ftol2 sobre *intensity (fld; call _ftol2): palabra baja del entero truncado; deja en la pila x87 el residuo del _ftol2 del EXE (v - R y R en los registros muertos 1 y 2).
int __cdecl FxEmitterTruncateCpp(const float* intensity);
// fild paso; fdiv *intensity; fmul delta: devuelve ST0 sin redondear (el llamador lo guarda como float).
float __cdecl FxEmitterTimeSinceCpp(int step, const float* intensity, float deltaTime);
// *intensity = *intensity - (float)_ftol2(*intensity) (la parte fraccionaria queda; fild eax; fsubr; fstp).
void __cdecl FxEmitterTakeWholeCpp(float* intensity);

// --- Lote g22a (PointLights.cpp, 17/10/2026): GenerateLightsAffectingObject, GetLightMultiplier, ProcessVerticalLineUsingCache y RenderFogEffect ---
// Todos reciben punteros a la memoria original del EXE (la luz es un CPointLight de 0x30 bytes: pos {0x00}, direccion {0x0C}, radio 0x18, color RGB {0x1C}, entidad 0x28, tipo 0x2C,
// tipo de niebla 0x2D) y conservan el orden, los operandos y los puntos de almacenamiento a float del desensamblado; las comparaciones dejan en C0/C2/C3 lo mismo que el EXE.
// CPointLights::GetLightMultiplier (0x6FFE70): F (producto de las luces ANTILIGHT) + S (suma de las demas) en ST0 sin guardar como float; GetLightMultiplier lo declara como float con otro
// nombre (/alternatename). lights = CPointLights::aLights, count = NumLights.
extern "C" double __cdecl PointLightsLightMultiplierDblCpp(const float* point, const void* lights, int count);
// CPointLights::ProcessVerticalLineUsingCache (0x6FFFF0): indice de la primera entrada de aCachedMapReads (32 x {x, y, z}) igual al punto (comparaciones ordenadas) o -1.
int __cdecl PointLightsCacheFindCpp(const float* cache, const float* point);
// FLD/FSTP de un float (un NaN senalizante sale silenciado): el z que se guarda en la cache y el de *outZ (0x700070..0x7000B8).
void __cdecl PointLightsFloatCopyCpp(const float* source, float* destination);
// z - 20.0 como float (segundo argumento de CWorld::ProcessVerticalLine en 0x6FFFF0).
float __cdecl PointLightsVerticalEndZCpp(float z);
// CPointLights::GenerateLightsAffectingObject (0x6FFBB0), cuerpo del bucle para una luz: true si el EXE llamaria a AddAnExtraDirectionalLight con args = {x, y, z, rojo, verde, azul};
// *result es el acumulador (1.0f al empezar), totalLighting puede ser nulo.
bool __cdecl PointLightsGenerateOneCpp(const void* light, const float* point, float* totalLighting, const void* entity, float* result, float* args);
// CPointLights::RenderFogEffect (0x7002D0): tramos aritmeticos entre las llamadas a colaboradores.
struct PointLightsFogSprite { float x, y, z, w, h; int red, green, blue, intensity; float recipNearZ, angle; };
// fogType == 1: *intensity = float(*foggyness); false si es exactamente 0.
bool __cdecl PointLightsFogIntensityCpp(const float* foggyness, float* intensity);
// Luz direccional: out = {gx0, gx1, gy0, gy1} de la rejilla de 4 unidades; false si no hay celdas.
bool __cdecl PointLightsFogDirBoundsCpp(const void* light, int* out);
// Luz direccional, celda (gx, gy) antes de CWorld::ProcessVerticalLine: origin = {gx, gy, z + 10}, *endZ = origin.z - 20, *tab = indice de la tabla 0x8D5068.
bool __cdecl PointLightsFogDirCellPreCpp(const void* light, int gx, int gy, float* origin, float* endZ, int* tab);
// Luz direccional tras ProcessVerticalLine: posn = argumento de CSprite::CalcScreenCoors, *alpha = factor alfa.
bool __cdecl PointLightsFogDirCellPostCpp(const void* light, const float* origin, const float* colPointZ, const float* intensity, float* posn, float* alpha);
// Luces de punto y de solo niebla: out = {gx0, gx1, gy0, gy1} de la rejilla de 2 unidades; size = 9 o 3.
bool __cdecl PointLightsFogPointBoundsCpp(const void* light, const float* size, int* out);
// Luz de punto, celda (gx, gy) antes de CSprite::CalcScreenCoors; camera = x, y, z de la camara.
bool __cdecl PointLightsFogPointCellPreCpp(const void* light, int gx, int gy, const float* size, const float* camera, const float* intensity, const float* groundZ, float* posn, float* alpha, int* tab);
// Argumentos de CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect tras CalcScreenCoors con exito; table = floats de 0x8D5068, screen = {x, y, z} de CalcScreenCoors.
void __cdecl PointLightsFogSpriteCpp(const void* light, const float* alpha, const float* screen, float w, float h, const float* table, int tab, int timeValue, bool pointLight, PointLightsFogSprite* result);

// --- Grupo g23a (BRIEF_21): CVehicle::DoVehicleLights (0x6E1A60), prefijo VehLights, y bloques x87 de DoHeadLightEffect (0x6E0A50), prefijo VehHeadTail. Anadir al FINAL de X87Math.h ---
// Cada funcion es un tramo recto del EXE con la pila x87 vacia al empezar y al acabar: levanta las mismas banderas, deja C0/C2/C3 del ultimo fcomp del tramo y escribe en los registros muertos
// los mismos valores que el EXE (ver X87Math.cpp). Los operandos que el EXE lee de memoria llegan por puntero.
// 0x6E1ABE: |dx| + |dy| entre la camara y el vehiculo > 100 (camera / position apuntan a x, y).
bool __cdecl VehLightsAbandonedFarCpp(const float* camera, const float* position);
// 0x6E1C8E: ZR-350 con luces: *prop = min(ts * 0.01 + *prop, 0.698); true si queda por debajo del objetivo (la funcion termina).
bool __cdecl VehLightsPropRiseCpp(const float* timeStep, float* prop);
// 0x6E1C53: ZR-350 sin luces: si *prop > 0, *prop = max(*prop - ts * 0.01, 0).
void __cdecl VehLightsPropFallCpp(const float* timeStep, float* prop);
// 0x6E2753: vx*vx + vy*vy < 0.2025 (speed apunta a x, y).
bool __cdecl VehLightsSlowCpp(const float* speed);
// 0x6E27EE: point = position - forward * 4.
void __cdecl VehLightsTrailPointCpp(const float* position, const float* forward, float* point);
// 0x6E2850: brake > 0.
bool __cdecl VehLightsBrakeCpp(const float* brake);
// 0x6E287A: out = -forward.
void __cdecl VehLightsNegateCpp(const float* forward, float* out);
// DoHeadLightEffect (0x6E0A50 y las copias en linea de DoVehicleLights): dummy == (0, 0, 0) con comparaciones ordenadas.
bool __cdecl VehHeadTailDummyZeroCpp(const float* dummy);
// Geometria (0x6E0AC5..0x6E0BD6): offset = dummy + frente * 0.05 (o (-sin(heading), cos(heading), 0) * 0.05 sin matriz; forward nulo), x = x - (x + x) si mirror;
// world = matrix * offset (0x59C890), delta = camara - world, magnitude = 0x59C970(delta) y dot = ((dz*fz) + (dy*fy)) + (dx*fx) con el frente de matrix (float[16] como CMatrix).
// dummy, forward, camera, offset apuntan a x, y, z; magnitude y dot son salidas float. Todo con el x87 a 24 bits y la semantica de NaN del hardware.
void __cdecl VehHeadTailHeadGeometryCpp(const float* dummy, const float* forward, float heading, bool mirror, const float* matrix, const float* camera,
                                        float* offset, float* magnitude, float* dot);
// dot > 0 (ordenado).
bool __cdecl VehHeadTailPositiveCpp(const float* dot);
// *sqrtDot = float(sqrt(*dot)); devuelve *sqrtDot > limite (0.9; 0.85 si train).
bool __cdecl VehHeadTailLimitCpp(const float* dot, bool train, float* sqrtDot);
// magnitude < 40 (ordenado).
bool __cdecl VehHeadTailNearCpp(const float* magnitude);
// *range = float(lod * 150).
void __cdecl VehHeadTailRangeStoreCpp(const float* lodMultiplier, float* range);
// Tamano, rango y colores de la corona principal (0x6E0D09..0x6E0DAB; inlinedCopy = las copias en linea de DoVehicleLights, que multiplican por sqrtDot antes que por 0.4). red / blue = EAX completo de _ftol2 (el llamador pasa solo el byte bajo).
void __cdecl VehHeadTailHeadCoronaCpp(const float* magnitude, const float* sqrtDot, const float* lodMultiplier, bool train, bool halogen, bool inlinedCopy,
                                      float* size, float* range, unsigned int* red, unsigned int* blue);

// =====================================================================================================================================================
// g23b: CVehicle::FlyingControl (0x6D85F0). Maquina x87 de software y funciones auxiliares del EXE como secuencias x87. Anadir AL FINAL de X87Math.h.
// =====================================================================================================================================================
// ============================================================================================================================================================
// g23b: maquina x87 de software (CVehicle::FlyingControl, 0x6D85F0).
// Reproduce, solo con aritmetica entera, las instrucciones x87 que ejecuta el EXE a precision de 24 bits (palabra de control 0x7F del juego): valores de 64 bits de mantisa,
// reglas de NaN del x87, banderas acumuladas (IE DE ZE OE UE PE), C0..C3 y C1, y los registros de la pila con el residuo que el EXE deja en los que quedan por debajo de la cima.
// No ejecuta ninguna instruccion de coma flotante (ni x87 ni SSE) al calcular: el hardware solo interviene en FlcX87::Commit (deja el estado final) y en fsin/fcos/fpatan/pow/asin/acos.
// ============================================================================================================================================================

struct FlcExt {
    unsigned long long sig;   // mantisa con bit entero (bit 63 = 1 en normales y NaN; en NaN el bit 62 es el de "silenciado"); 0 para cero
    int                exp;   // exponente sin sesgo: valor = sig * 2^(exp - 63)
    unsigned char      kind;  // 0 cero, 1 normal, 2 infinito, 3 NaN
    unsigned char      neg;
};
enum { FLC_ZERO = 0, FLC_NORMAL = 1, FLC_INF = 2, FLC_NAN = 3 };

// Banderas acumuladas (posiciones de la palabra de estado) y codigos de condicion.
enum { FLC_IE = 0x01, FLC_DE = 0x02, FLC_ZE = 0x04, FLC_OE = 0x08, FLC_UE = 0x10, FLC_PE = 0x20 };
enum { FLC_C0 = 0x0100, FLC_C1 = 0x0200, FLC_C2 = 0x0400, FLC_C3 = 0x4000 };

// Conversiones (los operandos de memoria float/double/int32 que carga el EXE). `flags` recibe DE/IE de la carga.
FlcExt FlcFromFloat(unsigned int bits, unsigned int* flags);
FlcExt FlcFromDouble(unsigned long long bits, unsigned int* flags);
FlcExt FlcFromInt(int value);
// fstp dword: devuelve los bits del float; flags recibe OE/UE/PE; *roundedUp = C1.
unsigned int FlcToFloat(const FlcExt& v, unsigned int* flags, bool* roundedUp);
unsigned long long FlcToDouble(const FlcExt& v);
unsigned long long FlcToDoubleFlags(const FlcExt& v, unsigned int* flags, bool* roundedUp);
FlcExt FlcFromDoubleExact(double d);   // sin banderas (valores que ya estaban en un registro)
double FlcToDoubleValue(const FlcExt& v);

// Aritmetica a 24 bits (resultado redondeado al par). `state` = banderas acumuladas; *c1 = el resultado se redondeo hacia arriba (magnitud mayor).
FlcExt FlcAdd(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcMul(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcDiv(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcSqrt(const FlcExt& a, unsigned int* state, bool* c1);
// Comparacion ordenada: devuelve C0|C2|C3 (0x0100, 0x0400, 0x4000) como fcom; marca IE si hay un NaN.
unsigned int FlcCompare(const FlcExt& a, const FlcExt& b, unsigned int* state, bool unorderedSignals);
bool FlcSameBits(const FlcExt& a, const FlcExt& b);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// La maquina: pila de 8 niveles. reg[d] = registro del nivel d (d = 0 es el primero que se apila tras la entrada, es decir T-1; la cima es reg[depth - 1]).
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
struct FlcX87 {
    FlcExt        reg[8];
    bool          written[8];
    int           depth;
    int           maxDepth;
    unsigned int  state;       // banderas acumuladas de esta maquina (FLC_IE ... FLC_PE)
    unsigned int  cc;          // C0 / C2 / C3 de la ultima operacion que los escribe
    bool          ccValid;     // alguna operacion ha escrito C0/C2/C3
    bool          c1;
    int           lastClass;   // 0 ninguna, 1 fcomp m32, 2 fstp m32, 3 otra (clase de la ultima instruccion x87 del EXE; la reproduce Commit)
    unsigned int  pendingDE;   // DE del operando de memoria, que solo cuenta si ningun operando es NaN

    FlcX87();

    // carga / almacenamiento
    void Fld(const float& m);              // fld dword [m]
    void FldD(const double& m);            // fld qword [m]
    void FldI(int value);                  // fild dword
    void FldSt(int i);                     // fld st(i)
    void FldZero();                        // fldz (no se usa en esta funcion, sirve a las pruebas)
    void FldExt(const FlcExt& v);          // empuja un valor ya calculado (retorno en ST0 de un llamado)
    void FldRet(const float& m);           // valor float que un llamado deja en ST0: la carga ya levanto sus banderas en el hardware, aqui solo se apila (senalizante -> silenciado)
    void Fstp(float& m);                   // fstp dword [m]
    void Fst(float& m);                    // fst dword [m]
    void FstpSt(int i);                    // fstp st(i)
    void FstSt(int i);
    void Fxch(int i);
    void Fchs();
    void Fabs();
    void Pop();                            // fstp st(0)
    // aritmetica ST0 op= m32 / m64 / int
    void Fadd(const float& m);   void Fsub(const float& m);   void Fsubr(const float& m);   void Fmul(const float& m);   void Fdiv(const float& m);   void Fdivr(const float& m);
    void FaddD(const double& m); void FsubD(const double& m); void FsubrD(const double& m); void FmulD(const double& m); void FdivD(const double& m); void FdivrD(const double& m);
    void FimulI(int v);  void FidivI(int v);  void FiaddI(int v);
    // aritmetica con registros: dest = (ST(a) op ST(b)); pop = true para las formas con "p"
    void Arith(char op, int dest, int srcA, int srcB, bool pop);   // op: '+', '-', '*', '/'; calcula ST(srcA) op ST(srcB) y lo escribe en ST(dest)
    // comparaciones: fcom/fcomp con m32 o ST(i)
    void FcomMem(const float& m, bool pop);
    void FcomSt(int i, bool pop);
    void Fcompp();
    void Fsqrt();
    void Fsin(); void Fcos(); void Fpatan();
    void FldOne();                         // fld1
    void FstpD(double& m);                 // fstp qword [m]
    void FstD(double& m);                  // fst qword [m]
    void FldI64(long long v);              // fild qword
    void Fistp64(long long& m);            // fistp qword [m] (redondeo al par)
    // lecturas de la ultima comparacion: AH de fnstsw (bits C0=1, C2=4, C3=0x40)
    unsigned int Ah() const;
    // valor de ST(i)
    const FlcExt& St(int i) const { return reg[depth - 1 - i]; }
    float  StFloat(int i);                 // el valor de ST(i) como float (sin tocar banderas)

    // Deja el estado final en el hardware (registros muertos T-1..T-maxDepth, banderas acumuladas y codigos de condicion). Implementado en X87Math.cpp.
    void Commit();

private:
    FlcExt& Top() { return reg[depth - 1]; }
    FlcExt& S(int i) { return reg[depth - 1 - i]; }
    void Push(const FlcExt& v);
    void Write(int i, const FlcExt& v);    // escribe ST(i) y marca el nivel
    FlcExt MemOperand(const float& m);
    FlcExt MemOperandD(const double& m);
    void ArithReg(char op, FlcExt a, FlcExt b, int dest, bool pop, bool memNaNOperand);
};

// Funciones x87 reales (X87Math.cpp, /arch:IA32): fsin, fcos y fpatan sobre doubles; el resultado de 64 bits sale como double.
double __cdecl FlcHwSin(double x);
double __cdecl FlcHwCos(double x);
double __cdecl FlcHwAtan2(double y, double x);
FlcExt FlcFromInt64(long long value);

// g23b: funciones auxiliares pequenas del EXE (CVector, CMatrix, CRT) como secuencias x87 de la maquina FlcX87.
// Cada una repite, instruccion por instruccion, la secuencia x87 del EXE (direccion entre parentesis); los argumentos de memoria son los mismos punteros/flotantes que recibe la funcion del EXE.

// ---- CVector (en el EXE son instancias fuera de linea de los operadores de CVector)
void FlcVecAdd(FlcX87& fp, float* out, const float* a, const float* b);            // 0x40FE30  out = a + b           (z, y, x)
void FlcVecSub(FlcX87& fp, float* out, const float* a, const float* b);            // 0x40FE60  out = a - b           (z, y, x)
void FlcVecMulLeft(FlcX87& fp, float* out, const float& f, const float* v);        // 0x40FE90  out = f * v           (z, y, x)
void FlcVecMulRight(FlcX87& fp, float* out, const float* v, const float& f);       // 0x40FEC0  out = v * f           (z, y, x)
void FlcVecScale(FlcX87& fp, float* v, const float& f);                            // 0x40FEF0  v *= f                (x, y, z)
void FlcVecAddTo(FlcX87& fp, float* v, const float* a);                            // 0x411A00  v += a                (x, y, z)
void FlcVecMagnitude(FlcX87& fp, const float* v);                                  // 0x4082C0  deja |v| en ST0
void FlcVecMagnitudeSquared(FlcX87& fp, const float* v);                           // 0x406DA0  deja |v|^2 en ST0
void FlcVecDot(FlcX87& fp, const float* a, const float* b);                        // 0x40FDB0  deja a.b en ST0       (z, y, x)
void FlcVecNormalise(FlcX87& fp, float* v);                                        // 0x59C910  (0x59C943: longitud 0 -> x = 1)
void FlcVecCross(FlcX87& fp, float* out, const float* a, const float* b);          // 0x59C730  out = a x b (via pila local del EXE)
void FlcMatMul3x3(FlcX87& fp, float* out, const float* m, const float* v);         // 0x59C790  out = M * v (columnas)
void FlcMatInvMul3x3(FlcX87& fp, float* out, const float* m, const float* v);      // 0x59C810  out = M^T * v
void FlcFloatMax(FlcX87& fp, const float& a, const float& b);                      // 0x420800  deja max en ST0 (b si a <= b o NaN)
void FlcFloatMin(FlcX87& fp, const float& a, const float& b);                      // 0x404330  deja min en ST0

// ---- CRT del EXE (argumentos en la pila x87)
void FlcCrtAsin(FlcX87& fp);                // 0x821E70 _CIasin: ST0 = asin(ST0)
void FlcCrtAcos(FlcX87& fp);                // 0x822380 _CIacos: ST0 = acos(ST0)
void FlcCrtPow(FlcX87& fp);                 // 0x822130 _CIpow:  ST1 ^ ST0 -> ST0 (saca uno)
int  FlcCrtFtol2(FlcX87& fp);               // 0x821B40 __ftol2: saca ST0 y devuelve EAX

// Estado del mundo que lee el CRT del EXE (en el juego son globales del EXE; los bancos las fijan).
struct FlcWorld {
    int sse2Pow;        // [0xC9C400]: el CRT usa pow SSE2
    int matherrFlag;    // [0xC9ABFC]: 0 -> el CRT comprueba y notifica las excepciones x87
};
FlcWorld FlcReadWorld();
unsigned int FlcHwSticky();   // banderas acumuladas del x87 real (IE.. PE) en el formato FLC_*

// --- Grupo g24a (BRIEF_21): Win::__MainWndProc (0x747EB0), prefijo WndProc. Anadir al FINAL de X87Math.h (Ftol2Regs ya esta declarado en el lote Ped) ---
// WM_ACTIVATE con la ventana inactiva (0x747FAA..0x747FC2): fild dword [RsGlobal.frameLimit]; fdivr dword [0x858C4C] (1000.0f); call _ftol2. Devuelve EAX de _ftol2 (palabra baja del entero truncado
// de 1000.0f / frameLimit, con el cociente redondeado a 24 bits por la propia x87) dejando las banderas acumuladas (PE del fdiv y del fistp, ZE si frameLimit = 0, IE del fistp con +inf) y los
// registros x87 muertos del EXE (T-2 = R, T-1 = fraccion), como Ftol2RegsCpp. Se usa como segundo argumento de SetTimer.
unsigned int __cdecl WndProcTimerPeriodCpp(int frameLimit);

// --- Grupo g26a (BRIEF_21): CAEAudioHardware::PlaySound (0x4D86B0). Anadir al FINAL de X87Math.h (Ftol2Regs / Ftol2RegsDblCpp ya estan declarados en el lote Ped) ---
// Posicion de inicio en porcentaje (0x4D8782..0x4D87A0, solo con el bit 3 de flags y posicion > 0): fild movsx(posicion); fmul dword [0x858C58] (0.01f); fimul dword length; fstp dword (float);
// 0x406CE0: fld dword; fstp qword; floor del CRT del EXE (0x8219F0, ruta SSE2: devuelve el resultado en ST0 con fld qword); _ftol2 (0x821B40). Devuelve EAX de _ftol2 (palabra baja del entero
// de 64 bits truncado) dejando las banderas acumuladas (PE de los dos productos) y los registros x87 muertos del EXE (T-2 = R, T-1 = fraccion, via Ftol2RegsDblCpp / X87Plant*).
unsigned int __cdecl AudioHwPercentPositionCpp(int percent, int length);
