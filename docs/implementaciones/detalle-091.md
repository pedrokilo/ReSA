# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1801"></a>

## Globales y helpers: source/game_sa/TxdStore_Plugin.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * TxdParentConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:22](../../source/game_sa/TxdStore_Plugin.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:28](../../source/game_sa/TxdStore_Plugin.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * TxdParentCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) | [source/game_sa/TxdStore_Plugin.cpp:33](../../source/game_sa/TxdStore_Plugin.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_txdPluginOffset: source/game_sa/TxdStore.h:22 (0x00C88018) |

<a id="grupo-1802"></a>

## Globales y helpers: source/game_sa/VehicleAnimGroupData.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| sVehicleAnimDataGroupFlags() | [source/game_sa/VehicleAnimGroupData.h:34](../../source/game_sa/VehicleAnimGroupData.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| sVehicleAnimDataSpecialFlags() | [source/game_sa/VehicleAnimGroupData.h:53](../../source/game_sa/VehicleAnimGroupData.h#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1803"></a>

## Globales y helpers: source/game_sa/VehicleRecording.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindPlaybackIndexOrEnd(CVehicle* vehicle) | [source/game_sa/VehicleRecording.cpp:50](../../source/game_sa/VehicleRecording.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo bPlaybackGoingOn: source/game_sa/VehicleRecording.h:70 (0x0097D6F0); Posible dato fijo pVehicleForPlayback: source/game_sa/VehicleRecording.h:64 (0x0097D840) |
| * FrameAt(CVehicleStateEachFrame* buffer, uint32 byteOffset) | [source/game_sa/VehicleRecording.cpp:60](../../source/game_sa/VehicleRecording.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| X87UInt(uint32 value) | [source/game_sa/VehicleRecording.cpp:65](../../source/game_sa/VehicleRecording.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1804"></a>

## Globales y helpers: source/game_sa/VisibilityPlugins.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InsertIntoList(CLinkList&lt;CVisibilityPlugins::AlphaObjectInfo&gt;& list, TObject* obj, float dist, CVisibilityPlugins::RenderFunction callback) | [source/game_sa/VisibilityPlugins.cpp:182](../../source/game_sa/VisibilityPlugins.cpp#L182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1805"></a>

## Globales y helpers: source/game_sa/Wanted.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WantedAsInt(eWantedLevel level) | [source/game_sa/Wanted.cpp:69](../../source/game_sa/Wanted.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WantedDistSqZYX(const CVector& from, const CVector& to) | [source/game_sa/Wanted.cpp:74](../../source/game_sa/Wanted.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1806"></a>

## Globales y helpers: source/game_sa/WaterCreature_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingX87_6E4(const CPlaceable* placeable) | [source/game_sa/WaterCreature_c.cpp:39](../../source/game_sa/WaterCreature_c.cpp#L39) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GetForwardX87_6E4(const CPlaceable* placeable) | [source/game_sa/WaterCreature_c.cpp:58](../../source/game_sa/WaterCreature_c.cpp#L58) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SetHeading_6E4(CPlaceable* placeable, float heading) | [source/game_sa/WaterCreature_c.cpp:77](../../source/game_sa/WaterCreature_c.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandomPercent_6E4() | [source/game_sa/WaterCreature_c.cpp:86](../../source/game_sa/WaterCreature_c.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1807"></a>

## Globales y helpers: source/game_sa/WaterLevel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterInterpolateWave(float base, float alongX, float alongY, float fractionX, float fractionY, float level, bool lower) | [source/game_sa/WaterLevel.cpp:191](../../source/game_sa/WaterLevel.cpp#L191) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GetWaterLayerParams(int32 layer) | [source/game_sa/WaterLevel.cpp:319](../../source/game_sa/WaterLevel.cpp#L319) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterVertexColor(const CRGBA& c, uint8 alpha) | [source/game_sa/WaterLevel.cpp:326](../../source/game_sa/WaterLevel.cpp#L326) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterBaseUV(int32 x, int32 y, int32 dy, const WaterLayerParams& l, float& baseU, float& baseV) | [source/game_sa/WaterLevel.cpp:331](../../source/game_sa/WaterLevel.cpp#L331) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterSetVertex(int32 idx, float x, float y, float z, uint32 color, float u, float v) | [source/game_sa/WaterLevel.cpp:339](../../source/game_sa/WaterLevel.cpp#L339) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempBufferVertices: source/game_sa/common.h:287 (0x00C4D958) |
| WaterDebugColor(const CRGBA& c) | [source/game_sa/WaterLevel.cpp:345](../../source/game_sa/WaterLevel.cpp#L345) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterNearestDistance(double x, double y) | [source/game_sa/WaterLevel.cpp:908](../../source/game_sa/WaterLevel.cpp#L908) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| WaterRenderShifts() | [source/game_sa/WaterLevel.cpp:1521](../../source/game_sa/WaterLevel.cpp#L1521) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1808"></a>

## Globales y helpers: source/game_sa/WaterLevel.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| lerp(CRenPar rp1, CRenPar rp2, float t) | [source/game_sa/WaterLevel.h:18](../../source/game_sa/WaterLevel.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1809"></a>

## Globales y helpers: source/game_sa/Weapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsinDegreesX87(float x) | [source/game_sa/Weapon.cpp:594](../../source/game_sa/Weapon.cpp#L594) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| & InstantHitField(void* p, size_t offset) | [source/game_sa/Weapon.cpp:1833](../../source/game_sa/Weapon.cpp#L1833) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitConstant(uintptr_t address) | [source/game_sa/Weapon.cpp:1834](../../source/game_sa/Weapon.cpp#L1834) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstantHitAnimationDuration(CWeaponInfo* wi) | [source/game_sa/Weapon.cpp:1835](../../source/game_sa/Weapon.cpp#L1835) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InstantHitSwayAccumulate355(CVector* endpoint,const CVector* sineBasis,const CVector* cosineBasis,float amplitude,float sine,float cosine) | [source/game_sa/Weapon.cpp:1849](../../source/game_sa/Weapon.cpp#L1849) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| InstantHitOscillation(uint32 milliseconds, float& sine, float& cosine) | [source/game_sa/Weapon.cpp:1900](../../source/game_sa/Weapon.cpp#L1900) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1810"></a>

## Globales y helpers: source/game_sa/WeaponEffects.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinMulX87(float angle, float k) | [source/game_sa/WeaponEffects.cpp:31](../../source/game_sa/WeaponEffects.cpp#L31) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CosMulX87(float angle, float k) | [source/game_sa/WeaponEffects.cpp:42](../../source/game_sa/WeaponEffects.cpp#L42) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| MulDoubleAddX87(float a, double k, float b) | [source/game_sa/WeaponEffects.cpp:54](../../source/game_sa/WeaponEffects.cpp#L54) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AngleSinCosX87(float y, float x, float& outSin, float& outCos, float& outOneMinusCos) | [source/game_sa/WeaponEffects.cpp:66](../../source/game_sa/WeaponEffects.cpp#L66) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1811"></a>

## Globales y helpers: source/game_sa/WeaponInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetBaseComboByName(const char* name) | [source/game_sa/WeaponInfo.cpp:159](../../source/game_sa/WeaponInfo.cpp#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindAnimGroupIndexByName(const char* name, bool& found) | [source/game_sa/WeaponInfo.cpp:179](../../source/game_sa/WeaponInfo.cpp#L179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1812"></a>

## Globales y helpers: source/game_sa/Weather.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SinMulAddX87(float fAng, float vScale, float vAddend) | [source/game_sa/Weather.cpp:84](../../source/game_sa/Weather.cpp#L84) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CosMulAddX87(float fAng, float vScale, float vAddend) | [source/game_sa/Weather.cpp:96](../../source/game_sa/Weather.cpp#L96) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| HalfMinusHalfCosX87(float fAng) | [source/game_sa/Weather.cpp:109](../../source/game_sa/Weather.cpp#L109) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| NegSinX87(float fAng) | [source/game_sa/Weather.cpp:123](../../source/game_sa/Weather.cpp#L123) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| CosX87(float fAng) | [source/game_sa/Weather.cpp:134](../../source/game_sa/Weather.cpp#L134) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Rand() | [source/game_sa/Weather.cpp:145](../../source/game_sa/Weather.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RandMulAddSub(float mul, float add, float sub) | [source/game_sa/Weather.cpp:148](../../source/game_sa/Weather.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Ftol(float f) | [source/game_sa/Weather.cpp:154](../../source/game_sa/Weather.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRainyType(int16 w) | [source/game_sa/Weather.cpp:156](../../source/game_sa/Weather.cpp#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSunnyType(int16 w) | [source/game_sa/Weather.cpp:157](../../source/game_sa/Weather.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsExtraSunnyType(int16 w) | [source/game_sa/Weather.cpp:161](../../source/game_sa/Weather.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsHeatHazeType(int16 w) | [source/game_sa/Weather.cpp:165](../../source/game_sa/Weather.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StepTowards(float& value, float target, float step) | [source/game_sa/Weather.cpp:170](../../source/game_sa/Weather.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1813"></a>

## Globales y helpers: source/game_sa/World.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WldASectorCoord(float coord) | [source/game_sa/World.cpp:306](../../source/game_sa/World.cpp#L306) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldALodSectorCoord(float coord) | [source/game_sa/World.cpp:310](../../source/game_sa/World.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WldASector(int32 x, int32 y) | [source/game_sa/World.cpp:314](../../source/game_sa/World.cpp#L314) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aSectors: source/game_sa/World.h:75 (0x00B7D0B8) |
| & WldARepeatSector(int32 x, int32 y) | [source/game_sa/World.cpp:317](../../source/game_sa/World.cpp#L317) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| WldADistSq(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:321](../../source/game_sa/World.cpp#L321) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldADistKinda(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:337](../../source/game_sa/World.cpp#L337) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldADistNearest(const CVector& a, const CVector& b, bool b2D) | [source/game_sa/World.cpp:353](../../source/game_sa/World.cpp#L353) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldAMultiply3x3VM(const CVector& v, const CMatrix& m) | [source/game_sa/World.cpp:357](../../source/game_sa/World.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldABoundRadius(CEntity* entity) | [source/game_sa/World.cpp:365](../../source/game_sa/World.cpp#L365) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldAForEachList(int32 x, int32 y, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, Fn&& fn) | [source/game_sa/World.cpp:370](../../source/game_sa/World.cpp#L370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WorldCopyColPointExe(CColPoint& dst, const CColPoint& src) | [source/game_sa/World.cpp:447](../../source/game_sa/World.cpp#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldD_MatTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:579](../../source/game_sa/World.cpp#L579) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WorldSectorCoordExe(float coord) | [source/game_sa/World.cpp:2040](../../source/game_sa/World.cpp#L2040) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & WorldSectorExe(int32 x, int32 y) | [source/game_sa/World.cpp:2044](../../source/game_sa/World.cpp#L2044) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aSectors: source/game_sa/World.h:75 (0x00B7D0B8) |
| & WorldRepeatSectorExe(int32 x, int32 y) | [source/game_sa/World.cpp:2047](../../source/game_sa/World.cpp#L2047) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| WldEMultiply3x3MV(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:2083](../../source/game_sa/World.cpp#L2083) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldEMultiply3x3VM(const CVector& v, const CMatrix& m) | [source/game_sa/World.cpp:2092](../../source/game_sa/World.cpp#L2092) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WldEMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/World.cpp:2101](../../source/game_sa/World.cpp#L2101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WorldDampMoveSpeedExe(float* moveSpeed) | [source/game_sa/World.cpp:2402](../../source/game_sa/World.cpp#L2402) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1814"></a>

## Globales y helpers: source/game_sa/World_Helper.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & FindPlayerInfo(int32 playerId) | [source/game_sa/World_Helper.cpp:7](../../source/game_sa/World_Helper.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98) |
| FindPlayerHeight(int32 playerId) | [source/game_sa/World_Helper.cpp:212](../../source/game_sa/World_Helper.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & FindPlayerGroup(int32 playerId) | [source/game_sa/World_Helper.cpp:223](../../source/game_sa/World_Helper.cpp#L223) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1815"></a>

## Globales y helpers: source/game_sa/X87Math.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TrigSimpleSinCpp(float angle) | [source/game_sa/X87Math.cpp:6](../../source/game_sa/X87Math.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigSimpleCosCpp(float angle) | [source/game_sa/X87Math.cpp:12](../../source/game_sa/X87Math.cpp#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigSimpleSinMulCpp(float angle, float k) | [source/game_sa/X87Math.cpp:13](../../source/game_sa/X87Math.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigSimpleCosMulCpp(float angle, float k) | [source/game_sa/X87Math.cpp:14](../../source/game_sa/X87Math.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundScaledSinMulAdd(float angle, float k, float mult, float add) | [source/game_sa/X87Math.cpp:17](../../source/game_sa/X87Math.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundSinMulAdd(float angle, float mult, float add) | [source/game_sa/X87Math.cpp:20](../../source/game_sa/X87Math.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundCosMulAdd(float angle, float mult, float add) | [source/game_sa/X87Math.cpp:23](../../source/game_sa/X87Math.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundNegSinDiv(float angle, float divisor) | [source/game_sa/X87Math.cpp:26](../../source/game_sa/X87Math.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundCosHalf(float angle, float half) | [source/game_sa/X87Math.cpp:31](../../source/game_sa/X87Math.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TrigCompoundSubSin(float z, float angle) | [source/game_sa/X87Math.cpp:38](../../source/game_sa/X87Math.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsEqualOrderedCpp(float first, float second) | [source/game_sa/X87Math.cpp:46](../../source/game_sa/X87Math.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CloudsLessOrderedCpp(float first, float second) | [source/game_sa/X87Math.cpp:49](../../source/game_sa/X87Math.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShadowSqrtCpp(float value) | [source/game_sa/X87Math.cpp:56](../../source/game_sa/X87Math.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ShadowDistanceFadeCpp(float squared, float distance) | [source/game_sa/X87Math.cpp:60](../../source/game_sa/X87Math.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración (helper/consumidores) · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1816"></a>

## Globales y helpers: source/game_sa/cHandlingDataMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & HandlingField(void* record, size_t offset) | [source/game_sa/cHandlingDataMgr.cpp:201](../../source/game_sa/cHandlingDataMgr.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AtofTimes04X87(const char* token) | [source/game_sa/cHandlingDataMgr.cpp:205](../../source/game_sa/cHandlingDataMgr.cpp#L205) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SubtractVelocityStepX87(float value) | [source/game_sa/cHandlingDataMgr.cpp:445](../../source/game_sa/cHandlingDataMgr.cpp#L445) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| TimesOneSixthX87(float value) | [source/game_sa/cHandlingDataMgr.cpp:455](../../source/game_sa/cHandlingDataMgr.cpp#L455) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SinX87(float value) | [source/game_sa/cHandlingDataMgr.cpp:466](../../source/game_sa/cHandlingDataMgr.cpp#L466) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AsinDegreesHandlingX87(float x) | [source/game_sa/cHandlingDataMgr.cpp:482](../../source/game_sa/cHandlingDataMgr.cpp#L482) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1817"></a>

## Globales y helpers: source/game_sa/common.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectCommonHooks() | [source/game_sa/common.cpp:40](../../source/game_sa/common.cpp#L40) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectors(RwV3d* vecsOut, int32 numVectors, const CMatrix& matrix, const RwV3d* vecsIn) | [source/game_sa/common.cpp:92](../../source/game_sa/common.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pWorkingMatrix1: source/game_sa/Game.h:22 (0x00B72920) |
| TransformVectors(RwV3d* vecsOut, int32 numVectors, const CSimpleTransform& transform, const RwV3d* vecsIn) | [source/game_sa/common.cpp:102](../../source/game_sa/common.cpp#L102) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * MakeUpperCase(char* dest, const char* src) | [source/game_sa/common.cpp:150](../../source/game_sa/common.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::MakeUpperCase- 0x007186E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * MakeUpperCase(char* dest) | [source/game_sa/common.cpp:163](../../source/game_sa/common.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::MakeUpperCase-in 0x00718710 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EndsWith(const char* str, const char* with, bool caseSensitive) | [source/game_sa/common.cpp:174](../../source/game_sa/common.cpp#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/common.cpp:202](../../source/game_sa/common.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoors(const CVector& in, CVector& out, float& scaleX, float& scaleY) | [source/game_sa/common.cpp:212](../../source/game_sa/common.cpp#L212) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Common::CalcScreenCoors-VVff 0x0071DA00 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028); Posible dato fijo ms_fFOV: source/game_sa/Draw.h:14 (0x008D5038) |
| CalcScreenCoors(const CVector& in, CVector& out) | [source/game_sa/common.cpp:236](../../source/game_sa/common.cpp#L236) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |
| DistAlongPerpRightLine2D(CVector2D origin, CVector2D dir, CVector2D pt) | [source/game_sa/common.cpp:257](../../source/game_sa/common.cpp#L257) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoesInfiniteLineCrossFiniteLine( CVector2D finiteA, CVector2D finiteB, CVector2D origin, CVector2D dir ) | [source/game_sa/common.cpp:263](../../source/game_sa/common.cpp#L263) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UTF8ToUnicode(const std::string &str) | [source/game_sa/common.cpp:307](../../source/game_sa/common.cpp#L307) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| UnicodeToUTF8(const std::wstring& str) | [source/game_sa/common.cpp:327](../../source/game_sa/common.cpp#L327) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RpAnimBlendClumpGetAssociation(RpClump* clump, std::initializer_list&lt;AnimationId&gt; ids) | [source/game_sa/common.cpp:348](../../source/game_sa/common.cpp#L348) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1818"></a>

## Globales y helpers: source/game_sa/common.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SCREEN_STRETCH_X(float a) | [source/game_sa/common.h:41](../../source/game_sa/common.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_STRETCH_Y(float a) | [source/game_sa/common.h:42](../../source/game_sa/common.h#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_STRETCH_FROM_RIGHT(float a) | [source/game_sa/common.h:43](../../source/game_sa/common.h#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_STRETCH_FROM_BOTTOM(float a) | [source/game_sa/common.h:44](../../source/game_sa/common.h#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_SCALE_X(float a) | [source/game_sa/common.h:54](../../source/game_sa/common.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_SCALE_Y(float a) | [source/game_sa/common.h:55](../../source/game_sa/common.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_SCALE_FROM_RIGHT(float a) | [source/game_sa/common.h:56](../../source/game_sa/common.h#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SCREEN_SCALE_FROM_BOTTOM(float a) | [source/game_sa/common.h:57](../../source/game_sa/common.h#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| sq(T x) | [source/game_sa/common.h:121](../../source/game_sa/common.h#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInRect2D(const CVector2D& point, const CVector2D& min, const CVector2D& max) | [source/game_sa/common.h:136](../../source/game_sa/common.h#L136) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInCircle2D(CVector2D point, CVector2D center, float r) | [source/game_sa/common.h:141](../../source/game_sa/common.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPointInSphere(const CVector& point, const CVector& center, float r) | [source/game_sa/common.h:145](../../source/game_sa/common.h#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DegreesToRadians(float angleInDegrees) | [source/game_sa/common.h:151](../../source/game_sa/common.h#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*(RwTexCoords lhs, float rhs) | [source/game_sa/common.h:156](../../source/game_sa/common.h#L156) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator+(RwTexCoords lhs, RwTexCoords rhs) | [source/game_sa/common.h:161](../../source/game_sa/common.h#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| multiply_weighted(R&& r) | [source/game_sa/common.h:174](../../source/game_sa/common.h#L174) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| multiply_weighted(WeightedValue&lt;T, Y&gt; (&&values)\[N\]) | [source/game_sa/common.h:185](../../source/game_sa/common.h#L185) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RadiansToDegrees(float angleInRadians) | [source/game_sa/common.h:191](../../source/game_sa/common.h#L191) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| lerp(const T& from, const T& to, float t) | [source/game_sa/common.h:196](../../source/game_sa/common.h#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| lerp&lt;RwRGBA&gt;(const RwRGBA& from, const RwRGBA& to, float t) | [source/game_sa/common.h:202](../../source/game_sa/common.h#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| invLerp(float fMin, float fMax, float fVal) | [source/game_sa/common.h:211](../../source/game_sa/common.h#L211) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| approxEqual(float f1, float f2, float epsilon) | [source/game_sa/common.h:216](../../source/game_sa/common.h#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| approxEqual2(float f1, float f2, float epsilon = 0.01F) | [source/game_sa/common.h:221](../../source/game_sa/common.h#L221) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| make_fourcc3(const char* line, const char abc\[3\]) | [source/game_sa/common.h:227](../../source/game_sa/common.h#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| make_fourcc4(const char* line, const char abcd\[4\]) | [source/game_sa/common.h:232](../../source/game_sa/common.h#L232) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MakeFourCC(const char fourcc\[4\]) | [source/game_sa/common.h:237](../../source/game_sa/common.h#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1819"></a>

## HeapBlockDesc

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| HeapBlockDesc::HeapBlockDesc() | [source/game_sa/Memory/HeapBlockDesc.cpp:6](../../source/game_sa/Memory/HeapBlockDesc.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeapBlockDesc::InsertHeapBlock(HeapBlockDesc* desc) | [source/game_sa/Memory/HeapBlockDesc.cpp:17](../../source/game_sa/Memory/HeapBlockDesc.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * HeapBlockDesc::_GetNextLocatedBlock() const | [source/game_sa/Memory/HeapBlockDesc.cpp:21](../../source/game_sa/Memory/HeapBlockDesc.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * HeapBlockDesc::_GetBlockData() const | [source/game_sa/Memory/HeapBlockDesc.cpp:25](../../source/game_sa/Memory/HeapBlockDesc.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HeapBlockDesc::_DumpBlockInfo() const | [source/game_sa/Memory/HeapBlockDesc.cpp:29](../../source/game_sa/Memory/HeapBlockDesc.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1820"></a>

## HeapBlockFlags

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetFlags(bool IsBound = false) | [source/game_sa/Memory/HeapBlockDesc.h:25](../../source/game_sa/Memory/HeapBlockDesc.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
