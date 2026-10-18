<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-941"></a>

## FxEmitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitter_c::InjectHooks() | [source/game_sa/Fx/FxEmitter.cpp:30](../../source/game_sa/Fx/FxEmitter.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) | [source/game_sa/Fx/FxEmitter.cpp:45](../../source/game_sa/Fx/FxEmitter.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Update(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:53](../../source/game_sa/Fx/FxEmitter.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::Reset() | [source/game_sa/Fx/FxEmitter.cpp:60](../../source/game_sa/Fx/FxEmitter.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:65](../../source/game_sa/Fx/FxEmitter.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A3EA0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitter_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:105](../../source/game_sa/Fx/FxEmitter.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A4050: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxEmitter_c::CreateParticles(float currentTime, float deltaTime) | [source/game_sa/Fx/FxEmitter.cpp:141](../../source/game_sa/Fx/FxEmitter.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxEmitter_c::CreateParticles 0x004A41E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x004A2580: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo Rain: source/game_sa/Weather.h:46 (0x00C81324); Posible dato fijo Wind: source/game_sa/Weather.h:44 (0x00C812F0); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| * FxEmitter_c::CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) | [source/game_sa/Fx/FxEmitter.cpp:179](../../source/game_sa/Fx/FxEmitter.cpp#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60) |

<a id="grupo-942"></a>

## FxFrustumInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxFrustumInfo_c::InjectHooks() | [source/game_sa/Fx/FxFrustumInfo.cpp:23](../../source/game_sa/Fx/FxFrustumInfo.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxFrustumInfo_c::IsCollisionRegs(FxSphere_c& sphere) | [source/game_sa/Fx/FxFrustumInfo.cpp:34](../../source/game_sa/Fx/FxFrustumInfo.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsCollision(FxSphere_c& sphere) | [source/game_sa/Fx/FxFrustumInfo.h:22](../../source/game_sa/Fx/FxFrustumInfo.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxFrustumInfo_c::IsCollision 0x004AA030 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-943"></a>

## FxInfoAnimTexture_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAnimTexture_c::FxInfoAnimTexture_c() | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-944"></a>

## FxInfoAttractLine_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractLine_c::FxInfoAttractLine_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractLine_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:29](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-945"></a>

## FxInfoAttractPt_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoAttractPt_c::FxInfoAttractPt_c() | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoAttractPt_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-946"></a>

## FxInfoColourBright_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourBright_c::FxInfoColourBright_c() | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourBright_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-947"></a>

## FxInfoColourRange_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColourRange_c::FxInfoColourRange_c() | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColourRange_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-948"></a>

## FxInfoColour_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoColour_c::FxInfoColour_c() | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoColour_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoColour.cpp:16](../../source/game_sa/Fx/Blueprints/FxInfoColour.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-949"></a>

## FxInfoDir_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoDir_c::FxInfoDir_c() | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoDir.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoDir.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-950"></a>

## FxInfoEmAngle_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmAngle_c::FxInfoEmAngle_c() | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmAngle_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmAngle_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-951"></a>

## FxInfoEmDir_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmDir_c::FxInfoEmDir_c() | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmDir_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-952"></a>

## FxInfoEmLife_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmLife_c::FxInfoEmLife_c() | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmLife_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmLife_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-953"></a>

## FxInfoEmPos_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmPos_c::FxInfoEmPos_c() | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmPos_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-954"></a>

## FxInfoEmRate_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRate_c::FxInfoEmRate_c() | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRate_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-955"></a>

## FxInfoEmRotation_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmRotation_c::FxInfoEmRotation_c() | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmRotation_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-956"></a>

## FxInfoEmSize_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSize_c::FxInfoEmSize_c() | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-957"></a>

## FxInfoEmSpeed_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmSpeed_c::FxInfoEmSpeed_c() | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp:23](../../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-958"></a>

## FxInfoEmWeather_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoEmWeather_c::FxInfoEmWeather_c() | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoEmWeather_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-959"></a>

## FxInfoFlat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFlat_c::FxInfoFlat_c() | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFlat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFlat.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-960"></a>

## FxInfoFloat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFloat_c::FxInfoFloat_c() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:14](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:19](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
