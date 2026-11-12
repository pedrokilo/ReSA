<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 892 grupos / 10295 funciones reconstruidas; 10295 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/CutsceneMgr.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) | [source/game_sa/CutsceneMgr.cpp:165](../../source/game_sa/CutsceneMgr.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aInfoForModel: source/game_sa/Streaming.h:247 (0x008E4CC0) |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/DamageManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PanelShift(uint32 panel) | [source/game_sa/DamageManager.cpp:28](../../source/game_sa/DamageManager.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LightShift(uint32 light) | [source/game_sa/DamageManager.cpp:29](../../source/game_sa/DamageManager.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AeroplaneShift(uint32 frame) | [source/game_sa/DamageManager.cpp:30](../../source/game_sa/DamageManager.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPanelStatusRaw(const CDamageManager* self, uint32 panel) | [source/game_sa/DamageManager.cpp:32](../../source/game_sa/DamageManager.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetPanelStatusRaw(CDamageManager* self, uint32 panel, uint32 status) | [source/game_sa/DamageManager.cpp:36](../../source/game_sa/DamageManager.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLightStatusRaw(const CDamageManager* self, uint32 light) | [source/game_sa/DamageManager.cpp:41](../../source/game_sa/DamageManager.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLightStatusRaw(CDamageManager* self, uint32 light, uint32 status) | [source/game_sa/DamageManager.cpp:45](../../source/game_sa/DamageManager.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetComponentGroupRaw(int32 comp, int32& outGroup, uint8& outIdx) | [source/game_sa/DamageManager.cpp:52](../../source/game_sa/DamageManager.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyDamageRaw(CDamageManager* self, CAutomobile* vehicle, int32 compId, float fIntensity) | [source/game_sa/DamageManager.cpp:78](../../source/game_sa/DamageManager.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_afDamageMultByGroup: source/game_sa/DamageManager.cpp:25 (0x008D32A0) |
| * __fastcall Init_Hook(CDamageManager* self) | [source/game_sa/DamageManager.cpp:124](../../source/game_sa/DamageManager.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall ResetDamageStatusAndWheelDamage_Hook(CDamageManager* self) | [source/game_sa/DamageManager.cpp:129](../../source/game_sa/DamageManager.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyDamage_Hook(CDamageManager* self, void*, CAutomobile* vehicle, int32 compId, float fIntensity, float fColDmgMult) | [source/game_sa/DamageManager.cpp:134](../../source/game_sa/DamageManager.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetComponentGroup_Hook(int32 comp, int32* outGroup, uint8* outIdx) | [source/game_sa/DamageManager.cpp:139](../../source/game_sa/DamageManager.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDoorStatus_Zx(const CDamageManager* self, void*, int32 door) | [source/game_sa/DamageManager.cpp:146](../../source/game_sa/DamageManager.cpp#L146) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDoorStatus_Component_Zx(const CDamageManager* self, void*, uint32 comp) | [source/game_sa/DamageManager.cpp:151](../../source/game_sa/DamageManager.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDoorStatus_Hook(CDamageManager* self, void*, int32 door, uint32 status) | [source/game_sa/DamageManager.cpp:157](../../source/game_sa/DamageManager.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetDoorStatus_Component_Hook(CDamageManager* self, void*, uint32 comp, uint32 status) | [source/game_sa/DamageManager.cpp:163](../../source/game_sa/DamageManager.cpp#L163) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPanelStatus_Hook(const CDamageManager* self, void*, uint32 panel) | [source/game_sa/DamageManager.cpp:169](../../source/game_sa/DamageManager.cpp#L169) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetPanelStatus_Hook(CDamageManager* self, void*, uint32 panel, uint32 status) | [source/game_sa/DamageManager.cpp:173](../../source/game_sa/DamageManager.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLightStatus_Hook(const CDamageManager* self, void*, uint32 light) | [source/game_sa/DamageManager.cpp:177](../../source/game_sa/DamageManager.cpp#L177) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetLightStatus_Hook(CDamageManager* self, void*, uint32 light, uint32 status) | [source/game_sa/DamageManager.cpp:181](../../source/game_sa/DamageManager.cpp#L181) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarNodeIndexFromPanel_Hook(uint32 panel) | [source/game_sa/DamageManager.cpp:186](../../source/game_sa/DamageManager.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCarNodeIndexFromDoor_Hook(uint32 door) | [source/game_sa/DamageManager.cpp:190](../../source/game_sa/DamageManager.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Darkel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SetGameLogicStateNow(eGameLogicState state) | [source/game_sa/Darkel.cpp:68](../../source/game_sa/Darkel.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GameState: source/game_sa/GameLogic.h:64 (0x0096A8B0); Posible dato fijo TimeOfLastEvent: source/game_sa/GameLogic.h:63 (0x0096A8AC) |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/DecisionMakers/Decision.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProbabilityByte(float value) | [source/game_sa/DecisionMakers/Decision.cpp:58](../../source/game_sa/DecisionMakers/Decision.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/DecisionMakers/DecisionMakerTypes.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsU32(const void* p) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:100](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L100) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstanceUnwind(EXCEPTION_RECORD* record, void* establisher, CONTEXT*, void*) | [source/game_sa/DecisionMakers/DecisionMakerTypes.cpp:186](../../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp#L186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:61](../../source/game_sa/Door.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

## Globales y helpers: source/game_sa/Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoFade() | [source/game_sa/Draw.cpp:53](../../source/game_sa/Draw.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FadeBlue: source/game_sa/Draw.h:22 (0x00C3EFAA); Posible dato fijo FadeGreen: source/game_sa/Draw.h:21 (0x00C3EFA9); Posible dato fijo FadeOutTime: source/game_sa/Draw.cpp:24 (0x008D2BD4); Posible dato fijo FadeRed: source/game_sa/Draw.h:20 (0x00C3EFA8); Posible dato fijo FadeTimer: source/game_sa/Draw.cpp:23 (0x00C16EE0); Posible dato fijo FadeValue: source/game_sa/Draw.h:23 (0x00C3EFAB); Posible dato fijo JustLoadedDontFadeInYet: source/game_sa/Draw.h:9 (0x00C16EDC); Posible dato fijo StillToFadeOut: source/game_sa/Draw.h:10 (0x00C16EDD); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Entity/AnimatedBuilding.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AnimatedBuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:29](../../source/game_sa/Entity/AnimatedBuilding.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AnimatedBuildingDestructorBody(CAnimatedBuilding* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:33](../../source/game_sa/Entity/AnimatedBuilding.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimatedBuilding::DestructorBody 0x00537A70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140AAnimatedBuildingEntityDestructor() | [source/game_sa/Entity/AnimatedBuilding.cpp:60](../../source/game_sa/Entity/AnimatedBuilding.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

## Globales y helpers: source/game_sa/Entity/Building.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:27](../../source/game_sa/Entity/Building.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BuildingDestructorBody(CBuilding* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:31](../../source/game_sa/Entity/Building.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBuilding::DestructorBody 0x00404180 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABuildingEntityDestructor() | [source/game_sa/Entity/Building.cpp:45](../../source/game_sa/Entity/Building.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/Dummy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ADummyDestructorBody() | [source/game_sa/Entity/Dummy/Dummy.cpp:61](../../source/game_sa/Entity/Dummy/Dummy.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/DummyObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DummyObjectConstructorBody(CDummyObject* self, void* /*edx*/) | [source/game_sa/Entity/Dummy/DummyObject.cpp:110](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDummyObject::Constructor 0x0059EA00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DummyObjectAdoptObject(CDummyObject* self, CObject* obj) | [source/game_sa/Entity/Dummy/DummyObject.cpp:120](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ADummyObjectDestructorBody() | [source/game_sa/Entity/Dummy/DummyObject.cpp:153](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/Entity/Entity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EntityGetStatusExe(const CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Entity.cpp:47](../../source/game_sa/Entity/Entity.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntity::GetStatus 0x004043E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGetModelIndexExe(const CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Entity.cpp:53](../../source/game_sa/Entity/Entity.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEntity::GetModelIndex 0x00404420 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:190](../../source/game_sa/Entity/Entity.cpp#L190) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_LodSectorCoord(float v) | [source/game_sa/Entity/Entity.cpp:195](../../source/game_sa/Entity/Entity.cpp#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_ClampBoundRect(CRect& r) | [source/game_sa/Entity/Entity.cpp:202](../../source/game_sa/Entity/Entity.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & EntityA_LodList(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:216](../../source/game_sa/Entity/Entity.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aLodPtrLists: source/game_sa/World.h:79 (0x00B99EB8) |
| & EntityA_RepeatSector(int32 x, int32 y) | [source/game_sa/Entity/Entity.cpp:219](../../source/game_sa/Entity/Entity.cpp#L219) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_aRepeatSectors: source/game_sa/World.h:77 (0x00B992B8) |
| EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:223](../../source/game_sa/Entity/Entity.cpp#L223) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_SinTimes(float angle, float factor) | [source/game_sa/Entity/Entity.cpp:231](../../source/game_sa/Entity/Entity.cpp#L231) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityA_AbsLikeExe(float v) | [source/game_sa/Entity/Entity.cpp:235](../../source/game_sa/Entity/Entity.cpp#L235) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Entity.cpp:1049](../../source/game_sa/Entity/Entity.cpp#L1049) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) | [source/game_sa/Entity/Entity.cpp:1132](../../source/game_sa/Entity/Entity.cpp#L1132) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

## Globales y helpers: source/game_sa/Entity/Object/CutsceneObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CutsceneObjectDestructorBody(CCutsceneObject* self, void* /*edx*/) | [source/game_sa/Entity/Object/CutsceneObject.cpp:253](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L253) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ACutsceneObjectDestructorBody() | [source/game_sa/Entity/Object/CutsceneObject.cpp:259](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L259) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Entity/Object/Object.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ObjF(uint32 bits) | [source/game_sa/Entity/Object/Object.cpp:43](../../source/game_sa/Entity/Object/Object.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:56](../../source/game_sa/Entity/Object/Object.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjMultiplyPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:65](../../source/game_sa/Entity/Object/Object.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjSqMag(const CVector& v) | [source/game_sa/Entity/Object/Object.cpp:74](../../source/game_sa/Entity/Object/Object.cpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjFtol(float f) | [source/game_sa/Entity/Object/Object.cpp:79](../../source/game_sa/Entity/Object/Object.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjCeilSteps(float f) | [source/game_sa/Entity/Object/Object.cpp:91](../../source/game_sa/Entity/Object/Object.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObjIsInObjectPool(const CObject* object) | [source/game_sa/Entity/Object/Object.cpp:1542](../../source/game_sa/Entity/Object/Object.cpp#L1542) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Entity/Object/Projectile.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProjectileDestructorBody(CProjectile* self, void* /*edx*/) | [source/game_sa/Entity/Object/Projectile.cpp:31](../../source/game_sa/Entity/Object/Projectile.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CProjectile::Destructor 0x005A40E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CivilianPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CivilianPedDestructorBody(CCivilianPed* self, void* /*edx*/) | [source/game_sa/Entity/Ped/CivilianPed.cpp:25](../../source/game_sa/Entity/Ped/CivilianPed.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCivilianPed::DestructorBody 0x005DDBE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CopPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveModelForCopType(uint32_t typeOrModelID) | [source/game_sa/Entity/Ped/CopPed.cpp:62](../../source/game_sa/Entity/Ped/CopPed.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Entity/Ped/Ped.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedE_Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1279](../../source/game_sa/Entity/Ped/Ped.cpp#L1279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_TransformPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Ped/Ped.cpp:1288](../../source/game_sa/Entity/Ped/Ped.cpp#L1288) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedE_HeadingAdd(const CEntity* e, float add) | [source/game_sa/Entity/Ped/Ped.cpp:1297](../../source/game_sa/Entity/Ped/Ped.cpp#L1297) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PedE_BoneMatrix(CPed* ped, int32 boneTag) | [source/game_sa/Entity/Ped/Ped.cpp:1305](../../source/game_sa/Entity/Ped/Ped.cpp#L1305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FootParticlesInRange(CPed* ped, const CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2629](../../source/game_sa/Entity/Ped/Ped.cpp#L2629) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEntity::GetIsOnScreen 0x00534540 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| FootParticleJitter(float base) | [source/game_sa/Entity/Ped/Ped.cpp:2640](../../source/game_sa/Entity/Ped/Ped.cpp#L2640) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EmitFootRainParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2647](../../source/game_sa/Entity/Ped/Ped.cpp#L2647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |
| EmitFootDustParticles(CPed* ped, CVector* position) | [source/game_sa/Entity/Ped/Ped.cpp:2666](../../source/game_sa/Entity/Ped/Ped.cpp#L2666) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00); Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsPedPointerValid_NotInWorld(CPed* ped) | [source/game_sa/Entity/Ped/Ped.cpp:3834](../../source/game_sa/Entity/Ped/Ped.cpp#L3834) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedCtlExeRand() | [source/game_sa/Entity/Ped/Ped.cpp:4437](../../source/game_sa/Entity/Ped/Ped.cpp#L4437) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedCtlGunflashTimer(int16& alpha, int16 progress) | [source/game_sa/Entity/Ped/Ped.cpp:4447](../../source/game_sa/Entity/Ped/Ped.cpp#L4447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PecCopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Ped/Ped.cpp:5145](../../source/game_sa/Entity/Ped/Ped.cpp#L5145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PecDamageEvent(CPed* ped, CEntity* damager, float damage, uint8 direction) | [source/game_sa/Entity/Ped/Ped.cpp:5160](../../source/game_sa/Entity/Ped/Ped.cpp#L5160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PedStdBonePositions.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPedBoneStdPosition(uint32 bone) | [source/game_sa/Entity/Ped/PedStdBonePositions.h:11](../../source/game_sa/Entity/Ped/PedStdBonePositions.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PlayerPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetButtonSprintResultsKeepEdx(CPlayerPed* self, uint32 entryEdx, eSprintType sprintType) | [source/game_sa/Entity/Ped/PlayerPed.cpp:83](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerPedATanMinus(float x, float y, float compensation) | [source/game_sa/Entity/Ped/PlayerPed.cpp:1922](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L1922) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
