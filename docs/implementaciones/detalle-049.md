<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/10/2026 (Ciudad de México):** 818 grupos / 8882 funciones reconstruidas; 8882 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-961"></a>

## FxInfoFloat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFloat_c::FxInfoFloat_c() | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:14](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFloat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFloat.cpp:19](../../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-962"></a>

## FxInfoForce_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoForce_c::FxInfoForce_c() | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoForce_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoForce.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoForce.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-963"></a>

## FxInfoFriction_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoFriction_c::FxInfoFriction_c() | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoFriction_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoFriction.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-964"></a>

## FxInfoGroundCollide_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoGroundCollide_c::FxInfoGroundCollide_c() | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoGroundCollide_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-965"></a>

## FxInfoHeatHaze_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoHeatHaze_c::FxInfoHeatHaze_c() | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoHeatHaze_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-966"></a>

## FxInfoJitter_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoJitter_c::FxInfoJitter_c() | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoJitter_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoJitter.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-967"></a>

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

<a id="grupo-968"></a>

## FxInfoNoise_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoNoise_c::FxInfoNoise_c() | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:10](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoNoise_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoNoise.cpp:15](../../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp#L15) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-969"></a>

## FxInfoRotSpeed_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoRotSpeed_c::FxInfoRotSpeed_c() | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoRotSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-970"></a>

## FxInfoSelfLit_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSelfLit_c::FxInfoSelfLit_c() | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:14](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp:19](../../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-971"></a>

## FxInfoSize_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSize_c::FxInfoSize_c() | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSize_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSize.cpp:21](../../source/game_sa/Fx/Blueprints/FxInfoSize.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-972"></a>

## FxInfoSmoke_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSmoke_c::FxInfoSmoke_c() | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSmoke_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-973"></a>

## FxInfoSpriteRect_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoSpriteRect_c::FxInfoSpriteRect_c() | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoSpriteRect_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-974"></a>

## FxInfoTrail_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoTrail_c::FxInfoTrail_c() | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:6](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoTrail_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:12](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoTrail_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoTrail.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-975"></a>

## FxInfoUnderwater_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoUnderwater_c::FxInfoUnderwater_c() | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoUnderwater_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoUnderwater_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-976"></a>

## FxInfoWind_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfoWind_c::FxInfoWind_c() | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:7](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoWind_c::Load(FILESTREAM file, int32 version) | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:13](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInfoWind_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) | [source/game_sa/Fx/Blueprints/FxInfoWind.cpp:18](../../source/game_sa/Fx/Blueprints/FxInfoWind.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |

<a id="grupo-977"></a>

## FxInfo_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInfo_c::~FxInfo_c() | [source/game_sa/Fx/Blueprints/FxInfo.cpp:5](../../source/game_sa/Fx/Blueprints/FxInfo.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-978"></a>

## FxInterpInfo255_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfo255_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo255_c::FxInterpInfo255_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo255_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo255_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo255_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfo255_c::GetVal 0x004A8B80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-979"></a>

## FxInterpInfo32_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfo32_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo32_c::FxInterpInfo32_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfo32_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo32_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfo32_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfo32_c::GetVal 0x004A89C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-980"></a>

## FxInterpInfoFloat_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfoFloat_c::InjectHooks() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:31](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoFloat_c::FxInterpInfoFloat_c() | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:39](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxInterpInfoFloat_c::Load(FILESTREAM file) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:44](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoFloat_c::Allocate(int32 count) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:66](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxInterpInfoFloat_c::GetVal(int32 attrib, float time, float deltaTime) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:72](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L72) | C++ · Envoltura al EXE | Revalidada en migración: FxInterpInfoFloat_c::GetVal-values 0x004A8470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Delegación por dirección 0x004A85C0: fuera del catálogo consultado; ruta efectiva sin resolver |
| FxInterpInfoFloat_c::GetVal(float* outValues, float delta) | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp:77](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: FxInterpInfoFloat_c::GetVal-values 0x004A8470 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
