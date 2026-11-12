<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 893 grupos / 10321 funciones reconstruidas; 10321 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1001"></a>

## FxInfoJitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoJitter_c::FxInfoJitter_c() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1002"></a>

## FxInfoManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoManager_c::InjectHooks() | [source/game_sa/Fx/FxInfoManager.cpp:68](../../source/game_sa/Fx/FxInfoManager.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxInfoManager_c::AddFxInfo(int32 info) | [source/game_sa/Fx/FxInfoManager.cpp:95](../../source/game_sa/Fx/FxInfoManager.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::AddFxInfo 0x004A7B00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/FxInfoManager.cpp:195](../../source/game_sa/Fx/FxInfoManager.cpp#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInfoManager_c::Load 0x005C0B70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Conversión de dirección fija: 0x008220AD; Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInfoManager_c::ProcessEmissionInfo(float currentTime, float mult, float totalTime, bool useConst, EmissionInfo_t* emission) | [source/game_sa/Fx/FxInfoManager.cpp:263](../../source/game_sa/Fx/FxInfoManager.cpp#L263) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessEmissionInfo 0x004A4960 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::ProcessMovementInfo(float currentTime, float mult, float totalTime, float length, bool useConst, MovementInfo_t* movementInfo) | [source/game_sa/Fx/FxInfoManager.cpp:277](../../source/game_sa/Fx/FxInfoManager.cpp#L277) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessMovementInfo 0x004A4A10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoManager_c::ProcessRenderInfo(float currentTime, float mult, float totalTime, float length, bool useConst, RenderInfo_t* renderInfo) | [source/game_sa/Fx/FxInfoManager.cpp:291](../../source/game_sa/Fx/FxInfoManager.cpp#L291) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxInfoManager_c::ProcessRenderInfo 0x004A4A80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetInfos() | [source/game_sa/Fx/FxInfoManager.h:50](../../source/game_sa/Fx/FxInfoManager.h#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetInfos() const | [source/game_sa/Fx/FxInfoManager.h:51](../../source/game_sa/Fx/FxInfoManager.h#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1003"></a>

## FxInfoNoise_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoNoise_c::FxInfoNoise_c() | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:10](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1004"></a>

## FxInfoRotSpeed_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoRotSpeed_c::FxInfoRotSpeed_c() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1005"></a>

## FxInfoSelfLit_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSelfLit_c::FxInfoSelfLit_c() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:14](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:19](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1006"></a>

## FxInfoSize_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSize_c::FxInfoSize_c() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:21](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1007"></a>

## FxInfoSmoke_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSmoke_c::FxInfoSmoke_c() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1008"></a>

## FxInfoSpriteRect_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSpriteRect_c::FxInfoSpriteRect_c() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1009"></a>

## FxInfoTrail_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoTrail_c::FxInfoTrail_c() | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoTrail_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoTrail_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1010"></a>

## FxInfoUnderwater_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoUnderwater_c::FxInfoUnderwater_c() | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoUnderwater_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoUnderwater_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1011"></a>

## FxInfoWind_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoWind_c::FxInfoWind_c() | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoWind_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoWind_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |

<a id="grupo-1012"></a>

## FxInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfo_c::~FxInfo_c() | [source/game_sa/Fx/Blueprints/FxInfo.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfo.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1013"></a>

## FxInterpInfo255_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfo255_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo255_c::FxInterpInfo255_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo255_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo255_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo255_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfo255_c::GetVal 0x004A8B80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1014"></a>

## FxInterpInfo32_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfo32_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo32_c::FxInterpInfo32_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo32_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo32_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo32_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfo32_c::GetVal 0x004A89C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1015"></a>

## FxInterpInfoFloat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfoFloat_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoFloat_c::FxInterpInfoFloat_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoFloat_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoFloat_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoFloat_c::GetVal(int32 attrib, float time, float deltaTime) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L72) | C++ · Envoltura al EXE | Revalidada en migración: FxInterpInfoFloat_c::GetVal-values 0x004A8470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Delegación por dirección 0x004A85C0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FxInterpInfoFloat_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:77](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfoFloat_c::GetVal-values 0x004A8470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1016"></a>

## FxInterpInfoU255_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfoU255_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoU255_c::FxInterpInfoU255_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoU255_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoU255_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoU255_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfoU255_c::GetVal 0x004A8800 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1017"></a>

## FxInterpInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfo_c::FxInterpInfo_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.cpp:6](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TruncateLow32(double q) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h:29](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo_c::InterpolateKeys(TKey* const* keys, float* outValues, float delta, float scale) const | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h:45](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1018"></a>

## FxManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxManager_c::InjectHooks() | [source/game_sa/Fx/FxManager.cpp:42](../../source/game_sa/Fx/FxManager.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::FxManager_c() | [source/game_sa/Fx/FxManager.cpp:74](../../source/game_sa/Fx/FxManager.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Constructor 0x004A9470 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::Constructor() | [source/game_sa/Fx/FxManager.cpp:78](../../source/game_sa/Fx/FxManager.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Constructor 0x004A9470 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::Destructor() | [source/game_sa/Fx/FxManager.cpp:106](../../source/game_sa/Fx/FxManager.cpp#L106) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: FxManager_c::Destructor 0x004A90A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::Init() | [source/game_sa/Fx/FxManager.cpp:116](../../source/game_sa/Fx/FxManager.cpp#L116) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Init 0x004A98E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::Exit() | [source/game_sa/Fx/FxManager.cpp:134](../../source/game_sa/Fx/FxManager.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Exit 0x004A9A10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::DestroyFxSystem(FxSystem_c* system) | [source/game_sa/Fx/FxManager.cpp:149](../../source/game_sa/Fx/FxManager.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::DestroyFxSystem 0x004A9810 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxManager_c::DestroyAllFxSystems() | [source/game_sa/Fx/FxManager.cpp:175](../../source/game_sa/Fx/FxManager.cpp#L175) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::DestroyAllFxSystems 0x004A98B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::LoadFxProject(const char* path) | [source/game_sa/Fx/FxManager.cpp:188](../../source/game_sa/Fx/FxManager.cpp#L188) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::LoadFxProject 0x005C2420 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::UnloadFxProject() | [source/game_sa/Fx/FxManager.cpp:232](../../source/game_sa/Fx/FxManager.cpp#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::UnloadFxProject 0x004A9AE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::LoadFxSystemBP(Const char* filename, FILESTREAM file) | [source/game_sa/Fx/FxManager.cpp:247](../../source/game_sa/Fx/FxManager.cpp#L247) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::LoadFxSystemBP 0x005C1F50 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::FindFxSystemBP(const char* name) | [source/game_sa/Fx/FxManager.cpp:262](../../source/game_sa/Fx/FxManager.cpp#L262) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::FindFxSystemBP 0x004A9360 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::CalcFrustumInfo(RwCamera* camera) | [source/game_sa/Fx/FxManager.cpp:278](../../source/game_sa/Fx/FxManager.cpp#L278) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::CalcFrustumInfo 0x004A9140 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::Update(RwCamera* camera, float timeDelta) | [source/game_sa/Fx/FxManager.cpp:301](../../source/game_sa/Fx/FxManager.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Update 0x004A9A80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::Render(RwCamera* camera, bool bHeatHaze) | [source/game_sa/Fx/FxManager.cpp:322](../../source/game_sa/Fx/FxManager.cpp#L322) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::Render 0x004A92A0 | 0 / 0 | Posible dato fijo m_fDNBalanceParam: source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h:24 (0x008D12C0) |
| FxManager_c::ReturnParticle(FxEmitterPrt_c* emitter) | [source/game_sa/Fx/FxManager.cpp:338](../../source/game_sa/Fx/FxManager.cpp#L338) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::ReturnParticle 0x004A93B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::GetParticle(int8 primType) | [source/game_sa/Fx/FxManager.cpp:343](../../source/game_sa/Fx/FxManager.cpp#L343) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::GetParticle 0x004A93C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::FreeUpParticle() | [source/game_sa/Fx/FxManager.cpp:350](../../source/game_sa/Fx/FxManager.cpp#L350) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::FreeUpParticle 0x004A9400 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::SetWindData(CVector* dir, float* speed) | [source/game_sa/Fx/FxManager.cpp:363](../../source/game_sa/Fx/FxManager.cpp#L363) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::SetWindData 0x004A93E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::FxRwMatrixCreate() | [source/game_sa/Fx/FxManager.cpp:369](../../source/game_sa/Fx/FxManager.cpp#L369) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::FxRwMatrixCreate 0x004A9440 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxManager_c::FxRwMatrixDestroy(RwMatrix* matrix) | [source/game_sa/Fx/FxManager.cpp:374](../../source/game_sa/Fx/FxManager.cpp#L374) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::FxRwMatrixDestroy 0x004A9460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::CreateFxSystem(const char* name, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) | [source/game_sa/Fx/FxManager.cpp:379](../../source/game_sa/Fx/FxManager.cpp#L379) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::CreateFxSystem-2 0x004A9BB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::CreateFxSystem(Const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) | [source/game_sa/Fx/FxManager.cpp:386](../../source/game_sa/Fx/FxManager.cpp#L386) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::CreateFxSystem-4 0x004A9BE0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) | [source/game_sa/Fx/FxManager.cpp:393](../../source/game_sa/Fx/FxManager.cpp#L393) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::CreateFxSystem-3 0x004A96B0 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxManager_c::ShouldCreate(FxSystemBP_c* system, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) | [source/game_sa/Fx/FxManager.cpp:412](../../source/game_sa/Fx/FxManager.cpp#L412) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::ShouldCreate 0x004A9500 | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| * FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) | [source/game_sa/Fx/FxManager.cpp:435](../../source/game_sa/Fx/FxManager.cpp#L435) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::CreateFxSystem-1 0x004A95C0 | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |
| * GetFrustumInfo() | [source/game_sa/Fx/FxManager.h:62](../../source/game_sa/Fx/FxManager.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxManager_c::GetFrustumInfo 0x004A9130 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetMemPool() | [source/game_sa/Fx/FxManager.h:83](../../source/game_sa/Fx/FxManager.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Allocate(int32 count) | [source/game_sa/Fx/FxManager.h:86](../../source/game_sa/Fx/FxManager.h#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1019"></a>

## FxMemoryPool_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxMemoryPool_c::InjectHooks() | [source/game_sa/Fx/FxMemoryPool.cpp:26](../../source/game_sa/Fx/FxMemoryPool.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxMemoryPool_c::Init() | [source/game_sa/Fx/FxMemoryPool.cpp:42](../../source/game_sa/Fx/FxMemoryPool.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Init 0x004A9C30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxMemoryPool_c::Exit() | [source/game_sa/Fx/FxMemoryPool.cpp:54](../../source/game_sa/Fx/FxMemoryPool.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Exit 0x004A9C80 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxMemoryPool_c::Reset() | [source/game_sa/Fx/FxMemoryPool.cpp:62](../../source/game_sa/Fx/FxMemoryPool.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Reset 0x004A9C90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * FxMemoryPool_c::GetMem(int32 size, int32 align) | [source/game_sa/Fx/FxMemoryPool.cpp:71](../../source/game_sa/Fx/FxMemoryPool.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::GetMem 0x004A9CA0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxMemoryPool_c::Optimise() | [source/game_sa/Fx/FxMemoryPool.cpp:84](../../source/game_sa/Fx/FxMemoryPool.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Optimise 0x004A9CD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Constructor() | [source/game_sa/Fx/FxMemoryPool.h:30](../../source/game_sa/Fx/FxMemoryPool.h#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Constructor 0x004A9C10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Destructor() | [source/game_sa/Fx/FxMemoryPool.h:32](../../source/game_sa/Fx/FxMemoryPool.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: FxMemoryPool_c::Destructor 0x004A9C20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1020"></a>

## FxPlane_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & operator=(const RwPlane& rhs) | [source/game_sa/Fx/FxPlane.h:10](../../source/game_sa/Fx/FxPlane.h#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
