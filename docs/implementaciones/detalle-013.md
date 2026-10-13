<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-241"></a>

## CEventPlayerCommandToGroupAttack

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroupAttack::InjectHooks() | [source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp:22](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPlayerCommandToGroupAttack::Constructor(CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp:32](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroupAttack::Constructor 0x005F6340 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPlayerCommandToGroupAttack::AffectsPedGroup(CPedGroup* pedGroup) | [source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp:39](../../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroupAttack::AffectsPedGroup 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-242"></a>

## CEventPlayerCommandToGroupGather

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPlayerCommandToGroupGather::InjectHooks() | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:23](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPlayerCommandToGroupGather::Constructor(CPed* target) | [source/game_sa/Events/EventPlayerCommandToGroupGather.cpp:32](../../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPlayerCommandToGroupGather::Constructor 0x00609250 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-243"></a>

## CEventPool

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooks() | [source/game_sa/Pools/EventPool.h:35](../../source/game_sa/Pools/EventPool.h#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * New() | [source/game_sa/Pools/EventPool.h:49](../../source/game_sa/Pools/EventPool.h#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPool::New 0x004B5570 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-244"></a>

## CEventPotentialGetRunOver

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialGetRunOver::CEventPotentialGetRunOver(CVehicle* vehicle) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:5](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialGetRunOver::~CEventPotentialGetRunOver() | [source/game_sa/Events/EventPotentialGetRunOver.cpp:10](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialGetRunOver::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialGetRunOver.cpp:14](../../source/game_sa/Events/EventPotentialGetRunOver.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-245"></a>

## CEventPotentialWalkIntoBuilding

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoBuilding::CEventPotentialWalkIntoBuilding() | [source/game_sa/Events/EventPotentialWalkIntoBuilding.cpp:5](../../source/game_sa/Events/EventPotentialWalkIntoBuilding.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoBuilding::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialWalkIntoBuilding.cpp:7](../../source/game_sa/Events/EventPotentialWalkIntoBuilding.cpp#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventPotentialWalkIntoBuilding.h:12](../../source/game_sa/Events/EventPotentialWalkIntoBuilding.h#L12) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventPotentialWalkIntoBuilding.h:13](../../source/game_sa/Events/EventPotentialWalkIntoBuilding.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventPotentialWalkIntoBuilding.h:14](../../source/game_sa/Events/EventPotentialWalkIntoBuilding.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-246"></a>

## CEventPotentialWalkIntoFire

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoFire::InjectHooks() | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:23](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState) | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:33](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoFire::Constructor 0x004B1E20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPotentialWalkIntoFire::Constructor(const CVector& firePos, float fireSize, eMoveState moveState) | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:50](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoFire::Constructor 0x004B1E20 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoFire::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialWalkIntoFire.cpp:56](../../source/game_sa/Events/EventPotentialWalkIntoFire.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoFire::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-247"></a>

## CEventPotentialWalkIntoObject

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoObject::InjectHooks() | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:23](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoObject::CEventPotentialWalkIntoObject(CObject* object, int32 moveState) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:33](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoObject::Constructor 0x004AE5D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoObject::~CEventPotentialWalkIntoObject() | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:39](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPotentialWalkIntoObject::Constructor(CObject* object, int32 moveState) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:44](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoObject::Constructor 0x004AE5D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoObject::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialWalkIntoObject.cpp:50](../../source/game_sa/Events/EventPotentialWalkIntoObject.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoObject::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-248"></a>

## CEventPotentialWalkIntoPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoPed::InjectHooks() | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:24](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoPed::CEventPotentialWalkIntoPed(CPed* ped, const CVector& targetPoint, eMoveState moveState) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:35](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoPed::Constructor 0x004AE6E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoPed::~CEventPotentialWalkIntoPed() | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:44](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoPed::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:49](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoPed::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoPed::TakesPriorityOver(const CEvent& refEvent) | [source/game_sa/Events/EventPotentialWalkIntoPed.cpp:81](../../source/game_sa/Events/EventPotentialWalkIntoPed.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoPed::TakesPriorityOver 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-249"></a>

## CEventPotentialWalkIntoVehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventPotentialWalkIntoVehicle::InjectHooks() | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:23](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoVehicle::CEventPotentialWalkIntoVehicle(CVehicle* vehicle, int32 moveState) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:33](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoVehicle::Constructor 0x004AE320 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoVehicle::~CEventPotentialWalkIntoVehicle() | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:39](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventPotentialWalkIntoVehicle::Constructor(CVehicle* vehicle, int32 moveState) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:44](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoVehicle::Constructor 0x004AE320 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventPotentialWalkIntoVehicle::AffectsPed(CPed* ped) | [source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp:50](../../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventPotentialWalkIntoVehicle::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-250"></a>

## CEventRevived

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventRevived::AffectsPed(CPed* ped) | [source/game_sa/Events/EventRevived.cpp:5](../../source/game_sa/Events/EventRevived.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-251"></a>

## CEventScanner

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventScanner::CEventScanner() | [source/game_sa/Events/EventScanner.cpp:6](../../source/game_sa/Events/EventScanner.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScanner::Clear() | [source/game_sa/Events/EventScanner.cpp:21](../../source/game_sa/Events/EventScanner.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScanner::ScanForEvents(CPed& ped) | [source/game_sa/Events/EventScanner.cpp:26](../../source/game_sa/Events/EventScanner.cpp#L26) | C++ · Envoltura al EXE | Sin registro inequívoco | 0 / 0 | Delegación por dirección 0x00607E30: fuera del catálogo consultado; ruta efectiva sin resolver |
| CEventScanner::ScanForEventsNow(const CPed& ped, bool bDontScan) | [source/game_sa/Events/EventScanner.cpp:31](../../source/game_sa/Events/EventScanner.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & GetAcquaintanceScanner() | [source/game_sa/Events/EventScanner.h:83](../../source/game_sa/Events/EventScanner.h#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-252"></a>

## CEventScriptCommand

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventScriptCommand::InjectHooks() | [source/game_sa/Events/EventScriptCommand.cpp:26](../../source/game_sa/Events/EventScriptCommand.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::CEventScriptCommand(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds) | [source/game_sa/Events/EventScriptCommand.cpp:45](../../source/game_sa/Events/EventScriptCommand.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::Constructor 0x004B0A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::~CEventScriptCommand() | [source/game_sa/Events/EventScriptCommand.cpp:52](../../source/game_sa/Events/EventScriptCommand.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventScriptCommand::Constructor(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds) | [source/game_sa/Events/EventScriptCommand.cpp:57](../../source/game_sa/Events/EventScriptCommand.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::Constructor 0x004B0A00 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::GetEventPriority() const | [source/game_sa/Events/EventScriptCommand.cpp:64](../../source/game_sa/Events/EventScriptCommand.cpp#L64) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::GetEventPriority 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::AffectsPed(CPed* ped) | [source/game_sa/Events/EventScriptCommand.cpp:79](../../source/game_sa/Events/EventScriptCommand.cpp#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::TakesPriorityOver(const CEvent& refEvent) | [source/game_sa/Events/EventScriptCommand.cpp:85](../../source/game_sa/Events/EventScriptCommand.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::TakesPriorityOver 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::IsValid(CPed* ped) | [source/game_sa/Events/EventScriptCommand.cpp:95](../../source/game_sa/Events/EventScriptCommand.cpp#L95) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::IsValid 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventScriptCommand::CloneScriptTask() const | [source/game_sa/Events/EventScriptCommand.cpp:105](../../source/game_sa/Events/EventScriptCommand.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptCommand::CloneScriptTask 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptCommand::Destructor(CEventScriptCommand* self, uint32 entryEdx) | [source/game_sa/Events/EventScriptCommand.cpp:139](../../source/game_sa/Events/EventScriptCommand.cpp#L139) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CEventScriptCommand::Destructor 0x004B0A50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-253"></a>

## CEventScriptedAttractor

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventScriptedAttractor::InjectHooks() | [source/game_sa/Events/EventScriptedAttractor.cpp:23](../../source/game_sa/Events/EventScriptedAttractor.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventScriptedAttractor::CEventScriptedAttractor(C2dEffectPedAttractor* the2dEffect, CEntity* entity) | [source/game_sa/Events/EventScriptedAttractor.cpp:32](../../source/game_sa/Events/EventScriptedAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptedAttractor::Constructor 0x005FEF40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventScriptedAttractor::Constructor(C2dEffectPedAttractor* the2dEffect, CEntity* entity) | [source/game_sa/Events/EventScriptedAttractor.cpp:39](../../source/game_sa/Events/EventScriptedAttractor.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventScriptedAttractor::Constructor 0x005FEF40 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-254"></a>

## CEventSeenCop

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSeenCop::InjectHooks() | [source/game_sa/Events/EventSeenCop.cpp:23](../../source/game_sa/Events/EventSeenCop.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventSeenCop::Constructor1(CPed* cop) | [source/game_sa/Events/EventSeenCop.cpp:32](../../source/game_sa/Events/EventSeenCop.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSeenCop::Constructor1 0x005FF380 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-255"></a>

## CEventSeenPanickedPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSeenPanickedPed::InjectHooks() | [source/game_sa/Events/EventSeenPanickedPed.cpp:23](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventSeenPanickedPed::Constructor(CPed* ped) | [source/game_sa/Events/EventSeenPanickedPed.cpp:32](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSeenPanickedPed::Constructor 0x004B2080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSeenPanickedPed::CEventSeenPanickedPed(CPed* ped) | [source/game_sa/Events/EventSeenPanickedPed.cpp:35](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSeenPanickedPed::Constructor 0x004B2080 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSeenPanickedPed::~CEventSeenPanickedPed() | [source/game_sa/Events/EventSeenPanickedPed.cpp:40](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSeenPanickedPed::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSeenPanickedPed.cpp:45](../../source/game_sa/Events/EventSeenPanickedPed.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSeenPanickedPed::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-256"></a>

## CEventSexyPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSexyPed::CEventSexyPed(CPed* ped) | [source/game_sa/Events/EventSexyPed.cpp:5](../../source/game_sa/Events/EventSexyPed.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSexyPed::CEventSexyPed(CPed* sexyPed, eTaskType taskType) | [source/game_sa/Events/EventSexyPed.cpp:10](../../source/game_sa/Events/EventSexyPed.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSexyPed::~CEventSexyPed() | [source/game_sa/Events/EventSexyPed.cpp:16](../../source/game_sa/Events/EventSexyPed.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSexyPed::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSexyPed.cpp:20](../../source/game_sa/Events/EventSexyPed.cpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_ikChainMan: source/game_sa/Ragdoll/IKChainManager.cpp:29 (0x00C15448) |

<a id="grupo-257"></a>

## CEventSexyVehicle

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSexyVehicle::CEventSexyVehicle(CVehicle* vehicle) | [source/game_sa/Events/EventSexyVehicle.cpp:5](../../source/game_sa/Events/EventSexyVehicle.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSexyVehicle::~CEventSexyVehicle() | [source/game_sa/Events/EventSexyVehicle.cpp:10](../../source/game_sa/Events/EventSexyVehicle.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSexyVehicle::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSexyVehicle.cpp:14](../../source/game_sa/Events/EventSexyVehicle.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-258"></a>

## CEventSignalAtPed

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSignalAtPed::CEventSignalAtPed(CPed* signalAt, int32 initialPause, bool bPlayAnimAtEnd) | [source/game_sa/Events/EventSignalAtPed.cpp:5](../../source/game_sa/Events/EventSignalAtPed.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSignalAtPed::~CEventSignalAtPed() | [source/game_sa/Events/EventSignalAtPed.cpp:13](../../source/game_sa/Events/EventSignalAtPed.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSignalAtPed::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSignalAtPed.cpp:17](../../source/game_sa/Events/EventSignalAtPed.cpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSignalAtPed::IsValid(CPed* ped) | [source/game_sa/Events/EventSignalAtPed.cpp:21](../../source/game_sa/Events/EventSignalAtPed.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSignalAtPed.h:17](../../source/game_sa/Events/EventSignalAtPed.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSignalAtPed.h:18](../../source/game_sa/Events/EventSignalAtPed.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventSignalAtPed.h:19](../../source/game_sa/Events/EventSignalAtPed.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * Clone() const noexcept override | [source/game_sa/Events/EventSignalAtPed.h:21](../../source/game_sa/Events/EventSignalAtPed.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-259"></a>

## CEventSoundLoud

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSoundLoud::CEventSoundLoud(CEntity* entity) | [source/game_sa/Events/EventSoundLoud.cpp:5](../../source/game_sa/Events/EventSoundLoud.cpp#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSoundLoud::~CEventSoundLoud() | [source/game_sa/Events/EventSoundLoud.cpp:10](../../source/game_sa/Events/EventSoundLoud.cpp#L10) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSoundLoud::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSoundLoud.cpp:14](../../source/game_sa/Events/EventSoundLoud.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventType() const override | [source/game_sa/Events/EventSoundLoud.h:16](../../source/game_sa/Events/EventSoundLoud.h#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetEventPriority() const override | [source/game_sa/Events/EventSoundLoud.h:17](../../source/game_sa/Events/EventSoundLoud.h#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLifeTime() override | [source/game_sa/Events/EventSoundLoud.h:18](../../source/game_sa/Events/EventSoundLoud.h#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetSourceEntity() const override | [source/game_sa/Events/EventSoundLoud.h:20](../../source/game_sa/Events/EventSoundLoud.h#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetLocalSoundLevel() override | [source/game_sa/Events/EventSoundLoud.h:21](../../source/game_sa/Events/EventSoundLoud.h#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CloneEditable() const noexcept override | [source/game_sa/Events/EventSoundLoud.h:22](../../source/game_sa/Events/EventSoundLoud.h#L22) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-260"></a>

## CEventSoundQuiet

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CEventSoundQuiet::InjectHooks() | [source/game_sa/Events/EventSoundQuiet.cpp:23](../../source/game_sa/Events/EventSoundQuiet.cpp#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSoundQuiet::CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position) | [source/game_sa/Events/EventSoundQuiet.cpp:34](../../source/game_sa/Events/EventSoundQuiet.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::Constructor 0x005E05B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSoundQuiet::~CEventSoundQuiet() | [source/game_sa/Events/EventSoundQuiet.cpp:47](../../source/game_sa/Events/EventSoundQuiet.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventSoundQuiet::Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position) | [source/game_sa/Events/EventSoundQuiet.cpp:52](../../source/game_sa/Events/EventSoundQuiet.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::Constructor 0x005E05B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CEventSoundQuiet::AffectsPed(CPed* ped) | [source/game_sa/Events/EventSoundQuiet.cpp:59](../../source/game_sa/Events/EventSoundQuiet.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::AffectsPed 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CEventSoundQuiet::CloneEditable() const noexcept | [source/game_sa/Events/EventSoundQuiet.cpp:86](../../source/game_sa/Events/EventSoundQuiet.cpp#L86) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CEventSoundQuiet::CloneEditable 0x00000000 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
