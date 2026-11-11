<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10129 funciones reconstruidas; 10129 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1121"></a>

## Globales y helpers: source/game_sa/Door.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoorMultiply3x3_VM(const CVector& v, const CMatrix& m) | [source/game_sa/Door.cpp:61](../../source/game_sa/Door.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1122"></a>

## Globales y helpers: source/game_sa/Draw.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DoFade() | [source/game_sa/Draw.cpp:53](../../source/game_sa/Draw.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FadeBlue: source/game_sa/Draw.h:22 (0x00C3EFAA); Posible dato fijo FadeGreen: source/game_sa/Draw.h:21 (0x00C3EFA9); Posible dato fijo FadeOutTime: source/game_sa/Draw.cpp:24 (0x008D2BD4); Posible dato fijo FadeRed: source/game_sa/Draw.h:20 (0x00C3EFA8); Posible dato fijo FadeTimer: source/game_sa/Draw.cpp:23 (0x00C16EE0); Posible dato fijo FadeValue: source/game_sa/Draw.h:23 (0x00C3EFAB); Posible dato fijo JustLoadedDontFadeInYet: source/game_sa/Draw.h:9 (0x00C16EDC); Posible dato fijo StillToFadeOut: source/game_sa/Draw.h:10 (0x00C16EDD); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |

<a id="grupo-1123"></a>

## Globales y helpers: source/game_sa/Entity/AnimatedBuilding.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AnimatedBuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:29](../../source/game_sa/Entity/AnimatedBuilding.cpp#L29) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AnimatedBuildingDestructorBody(CAnimatedBuilding* self, void* /*edx*/) | [source/game_sa/Entity/AnimatedBuilding.cpp:33](../../source/game_sa/Entity/AnimatedBuilding.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimatedBuilding::DestructorBody 0x00537A70 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140AAnimatedBuildingEntityDestructor() | [source/game_sa/Entity/AnimatedBuilding.cpp:60](../../source/game_sa/Entity/AnimatedBuilding.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1124"></a>

## Globales y helpers: source/game_sa/Entity/Building.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BuildingEntityDestructor(CEntity* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:27](../../source/game_sa/Entity/Building.cpp#L27) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BuildingDestructorBody(CBuilding* self, void* /*edx*/) | [source/game_sa/Entity/Building.cpp:31](../../source/game_sa/Entity/Building.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CBuilding::DestructorBody 0x00404180 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote149ABuildingEntityDestructor() | [source/game_sa/Entity/Building.cpp:45](../../source/game_sa/Entity/Building.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1125"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/Dummy.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Lote130ADummyDestructorBody() | [source/game_sa/Entity/Dummy/Dummy.cpp:61](../../source/game_sa/Entity/Dummy/Dummy.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1126"></a>

## Globales y helpers: source/game_sa/Entity/Dummy/DummyObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall DummyObjectConstructorBody(CDummyObject* self, void* /*edx*/) | [source/game_sa/Entity/Dummy/DummyObject.cpp:110](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L110) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CDummyObject::Constructor 0x0059EA00 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DummyObjectAdoptObject(CDummyObject* self, CObject* obj) | [source/game_sa/Entity/Dummy/DummyObject.cpp:120](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ADummyObjectDestructorBody() | [source/game_sa/Entity/Dummy/DummyObject.cpp:153](../../source/game_sa/Entity/Dummy/DummyObject.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1127"></a>

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

<a id="grupo-1128"></a>

## Globales y helpers: source/game_sa/Entity/Object/CutsceneObject.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CutsceneObjectDestructorBody(CCutsceneObject* self, void* /*edx*/) | [source/game_sa/Entity/Object/CutsceneObject.cpp:220](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L220) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote125ACutsceneObjectDestructorBody() | [source/game_sa/Entity/Object/CutsceneObject.cpp:225](../../source/game_sa/Entity/Object/CutsceneObject.cpp#L225) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1129"></a>

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

<a id="grupo-1130"></a>

## Globales y helpers: source/game_sa/Entity/Object/Projectile.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ProjectileDestructorBody(CProjectile* self, void* /*edx*/) | [source/game_sa/Entity/Object/Projectile.cpp:31](../../source/game_sa/Entity/Object/Projectile.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CProjectile::Destructor 0x005A40E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1131"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CivilianPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CivilianPedDestructorBody(CCivilianPed* self, void* /*edx*/) | [source/game_sa/Entity/Ped/CivilianPed.cpp:25](../../source/game_sa/Entity/Ped/CivilianPed.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CCivilianPed::DestructorBody 0x005DDBE0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1132"></a>

## Globales y helpers: source/game_sa/Entity/Ped/CopPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResolveModelForCopType(uint32_t typeOrModelID) | [source/game_sa/Entity/Ped/CopPed.cpp:62](../../source/game_sa/Entity/Ped/CopPed.cpp#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1133"></a>

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

<a id="grupo-1134"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PedStdBonePositions.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetPedBoneStdPosition(uint32 bone) | [source/game_sa/Entity/Ped/PedStdBonePositions.h:11](../../source/game_sa/Entity/Ped/PedStdBonePositions.h#L11) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1135"></a>

## Globales y helpers: source/game_sa/Entity/Ped/PlayerPed.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetButtonSprintResultsKeepEdx(CPlayerPed* self, uint32 entryEdx, eSprintType sprintType) | [source/game_sa/Entity/Ped/PlayerPed.cpp:83](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlayerPedATanMinus(float x, float y, float compensation) | [source/game_sa/Entity/Ped/PlayerPed.cpp:1922](../../source/game_sa/Entity/Ped/PlayerPed.cpp#L1922) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1136"></a>

## Globales y helpers: source/game_sa/Entity/Physical.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PhysicalDestructorBody(CPhysical* self, void* /*edx*/) | [source/game_sa/Entity/Physical.cpp:39](../../source/game_sa/Entity/Physical.cpp#L39) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote140APhysicalDestructorBody() | [source/game_sa/Entity/Physical.cpp:50](../../source/game_sa/Entity/Physical.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysicalSector(float coordinate) | [source/game_sa/Entity/Physical.cpp:193](../../source/game_sa/Entity/Physical.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PhysicalSectorList(CRepeatSector& sector, eEntityType type) | [source/game_sa/Entity/Physical.cpp:196](../../source/game_sa/Entity/Physical.cpp#L196) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMultiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:1064](../../source/game_sa/Entity/Physical.cpp#L1064) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColSqMag(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1072](../../source/game_sa/Entity/Physical.cpp#L1072) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColDivide(const CVector& v, float divisor) | [source/game_sa/Entity/Physical.cpp:1076](../../source/game_sa/Entity/Physical.cpp#L1076) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagXZY(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1081](../../source/game_sa/Entity/Physical.cpp#L1081) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ApplyColMagZYX(const CVector& v) | [source/game_sa/Entity/Physical.cpp:1085](../../source/game_sa/Entity/Physical.cpp#L1085) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMatPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2279](../../source/game_sa/Entity/Physical.cpp#L2279) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PhysAttachMat3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Physical.cpp:2288](../../source/game_sa/Entity/Physical.cpp#L2288) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1137"></a>

## Globales y helpers: source/game_sa/Entity/Placeable.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) | [source/game_sa/Entity/Placeable.cpp:34](../../source/game_sa/Entity/Placeable.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPlaceable::GetMatrixCopy 0x0041BD80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1138"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Automobile.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| G1Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:56](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G1MultiplyMatrixWithVector(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:65](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:82](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2SetRotateAddPos(CMatrix& mat, const CVector& rot) | [source/game_sa/Entity/Vehicle/Automobile.cpp:93](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAutomobile::SetDoorDamage 0x006B1600 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2UpdateDoorFrameMatrix(const CDoor& door, CMatrix& mat) | [source/game_sa/Entity/Vehicle/Automobile.cpp:103](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2TimeMsPlusFloat(float addend) | [source/game_sa/Entity/Vehicle/Automobile.cpp:112](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:484](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5WheelSpinForAudio(float base, float timeStep, float audio, float rate) | [source/game_sa/Entity/Vehicle/Automobile.cpp:494](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L494) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4Multiply3x3(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1636](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1636) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4MultiplyPoint(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1645](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1645) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4CopyColPoint(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1655](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1655) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4Fsin(float v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1665](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1665) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4Fcos(float v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1669](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4TireTemperatureDecay(float temperatureAboveOne, float timeStep) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1675](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G4PowToFloat(float base, float exponent) | [source/game_sa/Entity/Vehicle/Automobile.cpp:1680](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L1680) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3MouseSteerDecay(float raw, float timeStep) | [source/game_sa/Entity/Vehicle/Automobile.cpp:2173](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L2173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G6HydraulicStickAngleChange(float stickLR, float stickUD, float jump, float& cosOut, float& sinOut) | [source/game_sa/Entity/Vehicle/Automobile.cpp:3835](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L3835) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G6TransformPointExe(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:4109](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L4109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * DisableMatFx(RpMaterial* material, void* data) | [source/game_sa/Entity/Vehicle/Automobile.cpp:4520](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L4520) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Get(CEntity** entities, size_t numEntities) requires std::is_base_of_v&lt;CEntity, T&gt; | [source/game_sa/Entity/Vehicle/Automobile.cpp:4864](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L4864) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TankFireMultiplyVectorMatrix59C810(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/Automobile.cpp:6375](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L6375) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderMultiply3x3(const CMatrix& matrix, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7349](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7349) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVector::Multiply3x3Exe 0x0059C790 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderMultiplyMatrixWithVector(const CMatrix& matrix, const CVector& v) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7354](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7354) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderPlaceableForward(const CPlaceable& placeable) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7361](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7361) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPlaceable::GetForwardVector 0x0041CCB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderSuspension(CAutomobile& car, const CCollisionData* colData) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7370](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderSimpleWheelPoints(CAutomobile& car) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7385](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7385) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderWheelParticles(CAutomobile& car, float speedMagnitude) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7399](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7399) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderMiddleWheelParticles(CAutomobile& car, float speedMagnitude) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7431](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7431) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderRainAndExhaust(CAutomobile& car, float speedRatio) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7464](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7464) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Rain: source/game_sa/Weather.h:46 (0x00C81324) |
| AutoPreRenderTaxiLight(CAutomobile& car, const CVector& local) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7483](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderRancherLight(CAutomobile& car) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7499](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7499) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| AutoPreRenderEmergencyLights(CAutomobile& car) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7520](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7520) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderModelLights(CAutomobile& car) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7611](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7611) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderWheelDirections(CAutomobile& car, CVector& front, CVector& rear) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7631](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7631) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderWheelRotation(CAutomobile& car, const CVehicleModelInfo& modelInfo, const CVector& front, const CVector& rear) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7645](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7645) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderBodyParts(CAutomobile& car, CMatrix& frameMatrix) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7666](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7666) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderForces(CAutomobile& car) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7722](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7722) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderTiltAxles(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, const int32* nodes, size_t count, bool useY) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7736](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7736) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderBfInjection(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, CVehicleModelInfo& modelInfo) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7759](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7759) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderTowTruck(CAutomobile& car, CMatrix& frameMatrix) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7772](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7772) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderForklift(CAutomobile& car, CMatrix& frameMatrix) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7800](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7800) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00A9B0C8 |
| AutoPreRenderHarvester(CAutomobile& car, CMatrix& frameMatrix, CVehicleModelInfo& modelInfo) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7812](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7812) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderModelComponents(CAutomobile& car, CMatrix& frameMatrix, const CCollisionData* colData, CVehicleModelInfo& modelInfo) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7833](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7833) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AutoPreRenderHydraulics(CAutomobile& car, CVehicleModelInfo& modelInfo) | [source/game_sa/Entity/Vehicle/Automobile.cpp:7876](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L7876) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetCurrentAtomicObjectCB(RwObject* object, void* data) | [source/game_sa/Entity/Vehicle/Automobile.cpp:8030](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L8030) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetCurrentAtomicObject(RwFrame* frame) | [source/game_sa/Entity/Vehicle/Automobile.cpp:8038](../../source/game_sa/Entity/Vehicle/Automobile.cpp#L8038) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1139"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bike.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CBikeDestructorBodyCall(CBike* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Bike.cpp:52](../../source/game_sa/Entity/Vehicle/Bike.cpp#L52) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CBike::Destructor 0x006B57A0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote164ABody_CBike() | [source/game_sa/Entity/Vehicle/Bike.cpp:66](../../source/game_sa/Entity/Vehicle/Bike.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSteerAngleTanExe(float degrees) | [source/game_sa/Entity/Vehicle/Bike.cpp:140](../../source/game_sa/Entity/Vehicle/Bike.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderSmooth355(float base,float step,float previous,float target) | [source/game_sa/Entity/Vehicle/Bike.cpp:575](../../source/game_sa/Entity/Vehicle/Bike.cpp#L575) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderWeights355(float lateral,float forward,float available,float* lateralOut,float* forwardOut) | [source/game_sa/Entity/Vehicle/Bike.cpp:578](../../source/game_sa/Entity/Vehicle/Bike.cpp#L578) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderJitter355(float speed,int randomValue) | [source/game_sa/Entity/Vehicle/Bike.cpp:581](../../source/game_sa/Entity/Vehicle/Bike.cpp#L581) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRiderForwardLimit355(float lean,float target) | [source/game_sa/Entity/Vehicle/Bike.cpp:584](../../source/game_sa/Entity/Vehicle/Bike.cpp#L584) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeControlBlend355(float oldValue, int32 input, float step, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:732](../../source/game_sa/Entity/Vehicle/Bike.cpp#L732) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeControlProductAdd355(float oldValue, float input, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:736](../../source/game_sa/Entity/Vehicle/Bike.cpp#L736) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeInputSteer355(float degrees, float steer) | [source/game_sa/Entity/Vehicle/Bike.cpp:740](../../source/game_sa/Entity/Vehicle/Bike.cpp#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeForwardSpeed355(const CVector* speed, const CVector* forward) | [source/game_sa/Entity/Vehicle/Bike.cpp:744](../../source/game_sa/Entity/Vehicle/Bike.cpp#L744) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMouseDecay355(float* value) | [source/game_sa/Entity/Vehicle/Bike.cpp:748](../../source/game_sa/Entity/Vehicle/Bike.cpp#L748) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeInputSpeedCap355(CVector* speed) | [source/game_sa/Entity/Vehicle/Bike.cpp:752](../../source/game_sa/Entity/Vehicle/Bike.cpp#L752) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeCopyColPointExe(CColPoint& dst, const CColPoint& src) | [source/game_sa/Entity/Vehicle/Bike.cpp:841](../../source/game_sa/Entity/Vehicle/Bike.cpp#L841) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMultiply3x3Exe(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Bike.cpp:852](../../source/game_sa/Entity/Vehicle/Bike.cpp#L852) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSqMagExe(const CVector& v) | [source/game_sa/Entity/Vehicle/Bike.cpp:998](../../source/game_sa/Entity/Vehicle/Bike.cpp#L998) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeDotZYXExe(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Bike.cpp:1001](../../source/game_sa/Entity/Vehicle/Bike.cpp#L1001) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMultiply3x3VMExe(const CVector& v, const CMatrix& m) | [source/game_sa/Entity/Vehicle/Bike.cpp:1004](../../source/game_sa/Entity/Vehicle/Bike.cpp#L1004) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeCrossExe(const CVector& a, const CVector& b) | [source/game_sa/Entity/Vehicle/Bike.cpp:1013](../../source/game_sa/Entity/Vehicle/Bike.cpp#L1013) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeMoveHandBone355(RwMatrix* bone, const CVector& delta, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:2025](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2025) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeRoadOrientation355(CMatrix* matrix, float rear, float front, float backX, float by, float bz, float fx, float fy, float fz) | [source/game_sa/Entity/Vehicle/Bike.cpp:2080](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2080) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderChassisHeight355(float lean, float height) | [source/game_sa/Entity/Vehicle/Bike.cpp:2176](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2176) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderContact355(CVector& out, const CVector& point, const CVector& right, float lean, float height) | [source/game_sa/Entity/Vehicle/Bike.cpp:2179](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2179) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderTrig355(float angle, float multiplier, float& sineOut, float& cosineOut) | [source/game_sa/Entity/Vehicle/Bike.cpp:2182](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2182) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderSine355(float angle, float factor) | [source/game_sa/Entity/Vehicle/Bike.cpp:2186](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2186) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikePreRenderForkAngle355(float height, float original, float forkLength) | [source/game_sa/Entity/Vehicle/Bike.cpp:2189](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2189) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeFramePosition355(RwFrame* frame, RwFrame* stop) | [source/game_sa/Entity/Vehicle/Bike.cpp:2418](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2418) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupEnd355(CColLine* line, float upper, float lower, float wheel) | [source/game_sa/Entity/Vehicle/Bike.cpp:2427](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2427) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupHeight355(float force, float suspensionLength, float wheel, float startZ) | [source/game_sa/Entity/Vehicle/Bike.cpp:2431](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2431) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BikeSetupSphere355(CColSphere* sphere,float limit) | [source/game_sa/Entity/Vehicle/Bike.cpp:2436](../../source/game_sa/Entity/Vehicle/Bike.cpp#L2436) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1140"></a>

## Globales y helpers: source/game_sa/Entity/Vehicle/Bmx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BmxMax(float a, float b) | [source/game_sa/Entity/Vehicle/Bmx.cpp:41](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxMultiply3x3_MV(const CMatrix& m, const CVector& v) | [source/game_sa/Entity/Vehicle/Bmx.cpp:46](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| BmxNeedsNewAnim(const CAnimBlendAssociation* a) | [source/game_sa/Entity/Vehicle/Bmx.cpp:55](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CBmxBaseDestructorCall(CBmx* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Bmx.cpp:64](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L64) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CBmxDestructorBodyExe(CBmx* self, void* /*edx*/) | [source/game_sa/Entity/Vehicle/Bmx.cpp:65](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote152ABody_CBmxBaseDestructor() | [source/game_sa/Entity/Vehicle/Bmx.cpp:84](../../source/game_sa/Entity/Vehicle/Bmx.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
