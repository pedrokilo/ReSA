<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1201"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleCollisionDestructorBody(CEventVehicleCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleCollision.cpp:30](../../source/game_sa/Events/EventVehicleCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleCollision::Destructor 0x004AC920 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1202"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageCollision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CollisionDestructorBody(CEventVehicleDamageCollision* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageCollision.cpp:30](../../source/game_sa/Events/EventVehicleDamageCollision.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageCollision::Destructor 0x006A0650 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1203"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDamageWeapon.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WeaponDestructorBody(CEventVehicleDamageWeapon* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDamageWeapon.cpp:30](../../source/game_sa/Events/EventVehicleDamageWeapon.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventVehicleDamageWeapon::Destructor 0x0061C310 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1204"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleDied.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleDiedDestructorBody(CEventVehicleDied* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleDied.cpp:26](../../source/game_sa/Events/EventVehicleDied.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleDied::Destructor 0x004B0DA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1205"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleOnFire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleOnFireDestructorBody(CEventVehicleOnFire* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleOnFire.cpp:26](../../source/game_sa/Events/EventVehicleOnFire.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleOnFire::Destructor 0x004B1170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1206"></a>

## Globales y helpers: source/game_sa/Events/EventVehicleToSteal.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VehicleToStealDestructorBody(CEventVehicleToSteal* self, void* /*edx*/) | [source/game_sa/Events/EventVehicleToSteal.cpp:25](../../source/game_sa/Events/EventVehicleToSteal.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventVehicleToSteal::Destructor 0x004AF700 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1207"></a>

## Globales y helpers: source/game_sa/Events/GroupEventHandler.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsPedInPlayersGroup(CPedGroup* pg, CPed* p) | [source/game_sa/Events/GroupEventHandler.cpp:111](../../source/game_sa/Events/GroupEventHandler.cpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* srcPed) | [source/game_sa/Events/GroupEventHandler.cpp:124](../../source/game_sa/Events/GroupEventHandler.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PickPartnerFollower(CPedGroup* pg, CPed* other) | [source/game_sa/Events/GroupEventHandler.cpp:786](../../source/game_sa/Events/GroupEventHandler.cpp#L786) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PartnerLookAts(CPedGroup* pg, CPed* closest, const char* purpose) | [source/game_sa/Events/GroupEventHandler.cpp:827](../../source/game_sa/Events/GroupEventHandler.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1208"></a>

## Globales y helpers: source/game_sa/Explosion.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| U32ToFloat(uint32 value) | [source/game_sa/Explosion.cpp:56](../../source/game_sa/Explosion.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DoesNeedToVehProcessBombTimer(eExplosionType type) | [source/game_sa/Explosion.cpp:171](../../source/game_sa/Explosion.cpp#L171) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExplosionRand01() | [source/game_sa/Explosion.cpp:184](../../source/game_sa/Explosion.cpp#L184) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1209"></a>

## Globales y helpers: source/game_sa/FileLoader.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeCIacos(float x) | [source/game_sa/FileLoader.cpp:1244](../../source/game_sa/FileLoader.cpp#L1244) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * AddTextureCB(RwTexture* texture, void* dict) | [source/game_sa/FileLoader.cpp:2630](../../source/game_sa/FileLoader.cpp#L2630) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetFilename(const char* filepath) | [source/game_sa/FileLoader.cpp:2652](../../source/game_sa/FileLoader.cpp#L2652) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1210"></a>

## Globales y helpers: source/game_sa/FileMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| createDirectory(const wchar_t* path) | [source/game_sa/FileMgr.cpp:39](../../source/game_sa/FileMgr.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1211"></a>

## Globales y helpers: source/game_sa/Fire.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FireStartBurnTime(uint32 burnTime) | [source/game_sa/Fire.cpp:94](../../source/game_sa/Fire.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FireMatrixTimesPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Fire.cpp:292](../../source/game_sa/Fire.cpp#L292) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FireDistanceZYX(const CVector& a, const CVector& b) | [source/game_sa/Fire.cpp:301](../../source/game_sa/Fire.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1212"></a>

## Globales y helpers: source/game_sa/FireManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FireDistSq3D(const CVector& point, const CVector& firePos) | [source/game_sa/FireManager.cpp:150](../../source/game_sa/FireManager.cpp#L150) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ClampFireGenerations(int8 nGenerations) | [source/game_sa/FireManager.cpp:301](../../source/game_sa/FireManager.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gFireManager: source/game_sa/FireManager.h:81 (0x00B71F80) |
| FireRegisterCorona(uint32 id, uint8 red, uint8 green, uint8 blue, const CVector& pos, float coronaSize, eCoronaFlareType flare) | [source/game_sa/FireManager.cpp:395](../../source/game_sa/FireManager.cpp#L395) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1213"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InlineStretchX(float a) | [source/game_sa/Frontend/MenuManager_Draw.cpp:78](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InlineStretchY(float a) | [source/game_sa/Frontend/MenuManager_Draw.cpp:79](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InlineScaleX(float at640, uint32 factorBits) | [source/game_sa/Frontend/MenuManager_Draw.cpp:83](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| InlineScaleY(float at448, uint32 factorBits) | [source/game_sa/Frontend/MenuManager_Draw.cpp:86](../../source/game_sa/Frontend/MenuManager_Draw.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1214"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Internal.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsSaveSlot(eMenuEntryType slot) | [source/game_sa/Frontend/MenuManager_Internal.h:242](../../source/game_sa/Frontend/MenuManager_Internal.h#L242) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1215"></a>

## Globales y helpers: source/game_sa/Frontend/MenuManager_Pages.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| StretchXK(float x, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:19](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| StretchYK(float y, uint32 kBits) | [source/game_sa/Frontend/MenuManager_Pages.cpp:23](../../source/game_sa/Frontend/MenuManager_Pages.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1216"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point) | [source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp:17](../../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004A4880: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1217"></a>

## Globales y helpers: source/game_sa/Fx/Fx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsFxWithinCameraRange(const CVector& pos, float range) | [source/game_sa/Fx/Fx.cpp:170](../../source/game_sa/Fx/Fx.cpp#L170) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| CreateFxWithinCameraRange(const char* name, const CVector& pos, float range) | [source/game_sa/Fx/Fx.cpp:180](../../source/game_sa/Fx/Fx.cpp#L180) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| FxExeRand() | [source/game_sa/Fx/Fx.cpp:198](../../source/game_sa/Fx/Fx.cpp#L198) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FxCameraDistanceSqZYX(const CVector& pos) | [source/game_sa/Fx/Fx.cpp:204](../../source/game_sa/Fx/Fx.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| FxAddWheelSandOrDust(Fx_c* fx, CVehicle* vehicle, const CVector& pos, bool spinning, float lightMult, bool dust) | [source/game_sa/Fx/Fx.cpp:214](../../source/game_sa/Fx/Fx.cpp#L214) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo m_FrameCounter: source/game_sa/Timer.h:31 (0x00B7CB4C); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |
| FxAddWheelDirt(CVehicle* vehicle, const CVector& pos, float lightMult, FxPrtMult_c mult) | [source/game_sa/Fx/Fx.cpp:264](../../source/game_sa/Fx/Fx.cpp#L264) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo m_FrameCounter: source/game_sa/Timer.h:31 (0x00B7CB4C) |
| FxDebrisMult(const RwRGBA& color, float scale) | [source/game_sa/Fx/Fx.cpp:301](../../source/game_sa/Fx/Fx.cpp#L301) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_Color: source/game_sa/Font.h:85 (0x00C71A60) |
| FxDebrisVelocity() | [source/game_sa/Fx/Fx.cpp:317](../../source/game_sa/Fx/Fx.cpp#L317) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RenderAddTri(CVector pos1, CVector pos2, CVector pos3, RwTexCoords coord1, RwTexCoords coord2, RwTexCoords coord3, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3) | [source/game_sa/Fx/Fx.cpp:907](../../source/game_sa/Fx/Fx.cpp#L907) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |

<a id="grupo-1218"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitter.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterExeRemainder() | [source/game_sa/Fx/FxEmitter.cpp:183](../../source/game_sa/Fx/FxEmitter.cpp#L183) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1219"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitterBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RenderEmitterParticles355(FxEmitterBP_c& emitter, RwCamera* camera, float& brightness, bool heat) | [source/game_sa/Fx/FxEmitterBP.cpp:58](../../source/game_sa/Fx/FxEmitterBP.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B6FA14; Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:85 (0x00C71A60); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |

<a id="grupo-1220"></a>

## Globales y helpers: source/game_sa/Fx/FxInfoManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxInfoInPool() | [source/game_sa/Fx/FxInfoManager.cpp:83](../../source/game_sa/Fx/FxInfoManager.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
