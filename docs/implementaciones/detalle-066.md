<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1301"></a>

## Globales y helpers: source/game_sa/Models/ClumpModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CClumpModelInfo | [source/game_sa/Models/ClumpModelInfo.h:38](../../source/game_sa/Models/ClumpModelInfo.h#L38) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CClumpModelInfo() | [source/game_sa/Models/ClumpModelInfo.h:48](../../source/game_sa/Models/ClumpModelInfo.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRwModelType() const override | [source/game_sa/Models/ClumpModelInfo.h:55](../../source/game_sa/Models/ClumpModelInfo.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1302"></a>

## Globales y helpers: source/game_sa/Models/DamageAtomicModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CDamageAtomicModelInfo | [source/game_sa/Models/DamageAtomicModelInfo.h:11](../../source/game_sa/Models/DamageAtomicModelInfo.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1303"></a>

## Globales y helpers: source/game_sa/Models/LodAtomicModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CLodAtomicModelInfo | [source/game_sa/Models/LodAtomicModelInfo.h:11](../../source/game_sa/Models/LodAtomicModelInfo.h#L11) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1304"></a>

## Globales y helpers: source/game_sa/Models/LodTimeModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CLodAtomicModelInfo | [source/game_sa/Models/LodTimeModelInfo.h:6](../../source/game_sa/Models/LodTimeModelInfo.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1305"></a>

## Globales y helpers: source/game_sa/Models/PedModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CClumpModelInfo | [source/game_sa/Models/PedModelInfo.h:29](../../source/game_sa/Models/PedModelInfo.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPedModelInfo() | [source/game_sa/Models/PedModelInfo.h:54](../../source/game_sa/Models/PedModelInfo.h#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~CPedModelInfo() | [source/game_sa/Models/PedModelInfo.h:55](../../source/game_sa/Models/PedModelInfo.h#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetRace() const | [source/game_sa/Models/PedModelInfo.h:68](../../source/game_sa/Models/PedModelInfo.h#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedType() const | [source/game_sa/Models/PedModelInfo.h:69](../../source/game_sa/Models/PedModelInfo.h#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedStatType() const | [source/game_sa/Models/PedModelInfo.h:70](../../source/game_sa/Models/PedModelInfo.h#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CanPedDriveVehicleClass(eVehicleClass cls) const | [source/game_sa/Models/PedModelInfo.h:71](../../source/game_sa/Models/PedModelInfo.h#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPedTypeFemale() const noexcept | [source/game_sa/Models/PedModelInfo.h:72](../../source/game_sa/Models/PedModelInfo.h#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1306"></a>

## Globales y helpers: source/game_sa/Models/TimeModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAtomicModelInfo | [source/game_sa/Models/TimeModelInfo.h:18](../../source/game_sa/Models/TimeModelInfo.h#L18) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1307"></a>

## Globales y helpers: source/game_sa/Models/VehicleModelInfo.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsValidCompRule(int32 nRule) | [source/game_sa/Models/VehicleModelInfo.cpp:1687](../../source/game_sa/Models/VehicleModelInfo.cpp#L1687) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo NewWeatherType: source/game_sa/Weather.h:59 (0x00C8131C); Posible dato fijo OldWeatherType: source/game_sa/Weather.h:60 (0x00C81320) |
| ChooseComponent(int32 rule, int32 comps) | [source/game_sa/Models/VehicleModelInfo.cpp:1697](../../source/game_sa/Models/VehicleModelInfo.cpp#L1697) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CVehicleModelInfo::ChooseComponent-Global 0x004C7FB0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetListOfComponentsNotUsedByRules(uint32 compRules, int32 numExtras, int32* outList) | [source/game_sa/Models/VehicleModelInfo.cpp:1735](../../source/game_sa/Models/VehicleModelInfo.cpp#L1735) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1308"></a>

## Globales y helpers: source/game_sa/Models/VehicleModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CVehicleModelInfo | [source/game_sa/Models/VehicleModelInfo.h:143](../../source/game_sa/Models/VehicleModelInfo.h#L143) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| && GetModelDummyPosition(this auto&& self, eVehicleDummy dummy) | [source/game_sa/Models/VehicleModelInfo.h:347](../../source/game_sa/Models/VehicleModelInfo.h#L347) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetFrontSeatPosn() | [source/game_sa/Models/VehicleModelInfo.h:432](../../source/game_sa/Models/VehicleModelInfo.h#L432) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetBackSeatPosn() | [source/game_sa/Models/VehicleModelInfo.h:433](../../source/game_sa/Models/VehicleModelInfo.h#L433) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsVehicleTypeValid() const | [source/game_sa/Models/VehicleModelInfo.h:437](../../source/game_sa/Models/VehicleModelInfo.h#L437) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsAutomobile() const | [source/game_sa/Models/VehicleModelInfo.h:438](../../source/game_sa/Models/VehicleModelInfo.h#L438) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsMonsterTruck() const | [source/game_sa/Models/VehicleModelInfo.h:439](../../source/game_sa/Models/VehicleModelInfo.h#L439) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsQuad() const | [source/game_sa/Models/VehicleModelInfo.h:440](../../source/game_sa/Models/VehicleModelInfo.h#L440) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsHeli() const | [source/game_sa/Models/VehicleModelInfo.h:441](../../source/game_sa/Models/VehicleModelInfo.h#L441) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsPlane() const | [source/game_sa/Models/VehicleModelInfo.h:442](../../source/game_sa/Models/VehicleModelInfo.h#L442) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBoat() const | [source/game_sa/Models/VehicleModelInfo.h:443](../../source/game_sa/Models/VehicleModelInfo.h#L443) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTrain() const | [source/game_sa/Models/VehicleModelInfo.h:444](../../source/game_sa/Models/VehicleModelInfo.h#L444) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFakeAircraft() const | [source/game_sa/Models/VehicleModelInfo.h:445](../../source/game_sa/Models/VehicleModelInfo.h#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBike() const | [source/game_sa/Models/VehicleModelInfo.h:446](../../source/game_sa/Models/VehicleModelInfo.h#L446) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsBMX() const | [source/game_sa/Models/VehicleModelInfo.h:447](../../source/game_sa/Models/VehicleModelInfo.h#L447) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsTrailer() const | [source/game_sa/Models/VehicleModelInfo.h:448](../../source/game_sa/Models/VehicleModelInfo.h#L448) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetWheelSizes(float front, float rear) | [source/game_sa/Models/VehicleModelInfo.h:451](../../source/game_sa/Models/VehicleModelInfo.h#L451) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetGameName(const char* name) | [source/game_sa/Models/VehicleModelInfo.h:455](../../source/game_sa/Models/VehicleModelInfo.h#L455) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFrontWheel(eCarWheel wheel) const | [source/game_sa/Models/VehicleModelInfo.h:461](../../source/game_sa/Models/VehicleModelInfo.h#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsRearWheel(eCarWheel door) const | [source/game_sa/Models/VehicleModelInfo.h:470](../../source/game_sa/Models/VehicleModelInfo.h#L470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSizeOfWheel(eCarWheel wheel) const | [source/game_sa/Models/VehicleModelInfo.h:475](../../source/game_sa/Models/VehicleModelInfo.h#L475) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWheelSize(bool front) | [source/game_sa/Models/VehicleModelInfo.h:479](../../source/game_sa/Models/VehicleModelInfo.h#L479) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVehicleStruct() const | [source/game_sa/Models/VehicleModelInfo.h:484](../../source/game_sa/Models/VehicleModelInfo.h#L484) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1309"></a>

## Globales y helpers: source/game_sa/Models/WeaponModelInfo.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CWeaponModelInfo | [source/game_sa/Models/WeaponModelInfo.h:18](../../source/game_sa/Models/WeaponModelInfo.h#L18) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetWeaponInfo(eWeaponType weapon) | [source/game_sa/Models/WeaponModelInfo.h:33](../../source/game_sa/Models/WeaponModelInfo.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetWeaponInfo() | [source/game_sa/Models/WeaponModelInfo.h:34](../../source/game_sa/Models/WeaponModelInfo.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1310"></a>

## Globales y helpers: source/game_sa/NodeRoute.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RH_ScopedInstall(operator new, 0x41B860); RH_ScopedInstall(operator delete, 0x41B870); } void* CNodeRoute::operator new(uint32 size) | [source/game_sa/NodeRoute.cpp:26](../../source/game_sa/NodeRoute.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1311"></a>

## Globales y helpers: source/game_sa/Occluder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MultiplyMatrixWithVector_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Occluder.cpp:36](../../source/game_sa/Occluder.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcScreenCoorsWithScale(const CVector& in, CVector& out) | [source/game_sa/Occluder.cpp:45](../../source/game_sa/Occluder.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalcClippedScreenCoors(int32 idxFrom, int32 idxTo, CVector& out) | [source/game_sa/Occluder.cpp:52](../../source/game_sa/Occluder.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo OccluderCoors: source/game_sa/Occluder.h:17 (0x00C798E0); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |
| RawAngle(const COccluder* o, size_t offset) | [source/game_sa/Occluder.cpp:65](../../source/game_sa/Occluder.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1312"></a>

## Globales y helpers: source/game_sa/Pad.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NormalizePadAxis(LONG value) | [source/game_sa/Pad.cpp:311](../../source/game_sa/Pad.cpp#L311) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| PadShakeFloatToInteger() | [source/game_sa/Pad.cpp:564](../../source/game_sa/Pad.cpp#L564) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| * DIReleaseMouse() | [source/game_sa/Pad.cpp:1860](../../source/game_sa/Pad.cpp#L1860) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00746F70: fuera del catálogo consultado; ruta efectiva sin resolver |
| InitialiseMouse(bool exclusive) | [source/game_sa/Pad.cpp:1864](../../source/game_sa/Pad.cpp#L1864) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1313"></a>

## Globales y helpers: source/game_sa/PathFind.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PathOrientation14Normalise() | [source/game_sa/PathFind.cpp:1497](../../source/game_sa/PathFind.cpp#L1497) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CPathFind::FindNodeOrientationForCarPlacement 0x00450320 | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1314"></a>

## Globales y helpers: source/game_sa/PedAttractorManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsValidAttractorType(int32 type) | [source/game_sa/PedAttractorManager.cpp:120](../../source/game_sa/PedAttractorManager.cpp#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:294](../../source/game_sa/PedAttractorManager.cpp#L294) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/PedAttractorManager.cpp:303](../../source/game_sa/PedAttractorManager.cpp#L303) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1315"></a>

## Globales y helpers: source/game_sa/PedGroupPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GroupOriginCanBeUsed(const CVector& origin, float radius) | [source/game_sa/PedGroupPlacer.cpp:39](../../source/game_sa/PedGroupPlacer.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028) |
| InstallGroupPed(CPedGroup& group, CPed* ped, int32 groupId, bool leader, bool setLeader) | [source/game_sa/PedGroupPlacer.cpp:48](../../source/game_sa/PedGroupPlacer.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PlaceCircularGroup(ePedType type, uint32 numPeds, const CVector& origin, bool random) | [source/game_sa/PedGroupPlacer.cpp:55](../../source/game_sa/PedGroupPlacer.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1316"></a>

## Globales y helpers: source/game_sa/PedGroups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Task10RemoveMember() | [source/game_sa/PedGroups.cpp:50](../../source/game_sa/PedGroups.cpp#L50) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005F80D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| Task10FlushIntelligence() | [source/game_sa/PedGroups.cpp:56](../../source/game_sa/PedGroups.cpp#L56) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 1 / 1 | Trampolín ASM a 0x005F7350; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1317"></a>

## Globales y helpers: source/game_sa/PedIK.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AsinX87(float x) | [source/game_sa/PedIK.cpp:38](../../source/game_sa/PedIK.cpp#L38) | Ensamblador · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 1 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AtanMinusX87(float y, float x, float rot) | [source/game_sa/PedIK.cpp:62](../../source/game_sa/PedIK.cpp#L62) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SinCosX87(float angle, float& outSin, float& outCos) | [source/game_sa/PedIK.cpp:75](../../source/game_sa/PedIK.cpp#L75) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| SinMulX87(float angle, float k) | [source/game_sa/PedIK.cpp:90](../../source/game_sa/PedIK.cpp#L90) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| DivCosX87(float num, float angle) | [source/game_sa/PedIK.cpp:102](../../source/game_sa/PedIK.cpp#L102) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| RotateXX87(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:114](../../source/game_sa/PedIK.cpp#L114) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| RotateYX87(CMatrix& matrix, float angle) | [source/game_sa/PedIK.cpp:182](../../source/game_sa/PedIK.cpp#L182) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| ArmHeadingDiffX87(float nx, float ny, float rot) | [source/game_sa/PedIK.cpp:248](../../source/game_sa/PedIK.cpp#L248) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1318"></a>

## Globales y helpers: source/game_sa/PedIntelligence.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IncrementAngerAtPlayerRegs(CPedIntelligence* self, uint32 entryEdx, uint8 anger) | [source/game_sa/PedIntelligence.cpp:1032](../../source/game_sa/PedIntelligence.cpp#L1032) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_snTimeInMilliseconds: source/game_sa/Timer.h:44 (0x00B7CB84) |

<a id="grupo-1319"></a>

## Globales y helpers: source/game_sa/Pickup.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupIsVisibleKeepEdx(CPickup* self, uint32 entryEdx) | [source/game_sa/Pickup.cpp:32](../../source/game_sa/Pickup.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1320"></a>

## Globales y helpers: source/game_sa/Pickups.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PickupDistanceTo(const CPickup& pickup, const CVector& point) | [source/game_sa/Pickups.cpp:41](../../source/game_sa/Pickups.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PickupGlowByte(CEntity* entity, float distance, uint32 mask, float angleStep, float radius, float inverseRadius, float scale) | [source/game_sa/Pickups.cpp:201](../../source/game_sa/Pickups.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
