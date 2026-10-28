<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlWeave63B: CCarCtrl, parte B2, tanda B (recorridos de listas de sector)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-3`, HEAD df26d6fe7 + `CarCtrlWeave63A/source.patch` + `source.patch` de esta tanda). 24576 pares (6 funciones x 4096), cero diferencias EXE/DLL,
compuertas de ABI/ruta/restauración en ambos lados y cobertura mínima cumplida (`minimums.json`). Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlweave63b\20261020-225525-744289`.

| Función | Dirección | Hook (nombre RH) | Colaboradores sustituidos |
|---|---|---|---|
| `SlowCarDownForCarsSectorList` | 0x432420 | `SlowCarDownForCarsSectorList<CPtrListDoubleLink<CVehicle*>>` | 0x534250, 0x412A80, 0x42D0E0 |
| `SlowCarDownForObjectsSectorList` | 0x42D4F0 | `SlowCarDownForObjectsSectorList<CPtrListDoubleLink<CObject*>>` | 0x534250, 0x412A80, 0x426220 |
| `SlowCarDownForPedsSectorList` | 0x425440 | `SlowCarDownForPedsSectorList<CPtrListDoubleLink<CPed*>>` | 0x56E0D0, 0x56E210, 0x412A80, 0x4AB420, 0x5DF8F0, 0x421050, 0x571B70, 0x571A00 |
| `WeaveThroughCarsSectorList` | 0x42D680 | `WeaveThroughCarsSectorList<CPtrListDoubleLink<CVehicle*>>` | 0x534250, 0x426350 |
| `WeaveThroughObjectsSectorList` | 0x42D950 | `WeaveThroughObjectsSectorList<CPtrListDoubleLink<CObject*>>` | 0x411990, 0x426BC0 |
| `WeaveThroughPedsSectorList` | 0x42D7E0 | `WeaveThroughPedsSectorList<CPtrListDoubleLink<CPed*>>` | 0x426970 (WeaveForPed, sin catalogar ni reconstruir) |

Son plantillas (`PtrListType`): el hook se registra para la instanciación de la lista real del sector de cada tipo; el cuerpo es idéntico para cualquier lista de nodos (ítem +0, siguiente +4).
`source.patch` (aplicar tras el de la tanda A) contiene las seis funciones, el include de `Events/EventPotentialGetRunOver.h` y sus seis `RH_ScopedInstall`.

## Revisión propia contra el EXE (fase 2)
Desensamblado completo y marco de pila normalizado (como en la tanda A); cada rama de cada función contrastada con el C++ (filtros de código de exploración, `usa colisión`, modelo, rectángulo, z, `DistAlongLine2D`, orden y número de llamadas). Float a 24 bits.

Discrepancias halladas y corregidas: (1) `entity->GetBoundCentre(out)` (sobrecarga `void(CVector&)`) no pasa por el símbolo enganchado 0x534250 (`CVector GetBoundCentre() const`): el banco lo detectó (la DLL no llamaba al falso); se usa la sobrecarga que devuelve el vector.
(2) El campo `+0xD8` de `CPlayerInfo` que `SlowCarDownForPedsSectorList` compara con el vehículo es `m_pLastTargetVehicle`, no `m_pRemoteVehicle` (+0xB0): detectado en la revisión (el banco fija ese campo por desplazamiento, así que no lo habría visto); el `static_assert` del banco lo fija.
(3) `halfWidth` de la lista de peatones es una variable local que persiste y se multiplica por 1.6 en cada peatón que llega hasta ahí si el tipo de vehículo es 9 (el EXE la guarda en el marco): se reproduce.
(4) El evento `CEventPotentialGetRunOver` construido en la pila: el relleno y la vtable difieren entre el constructor del EXE y el de la DLL; el falso de `CEventGroup::Add` registra solo los campos que escribe el constructor.

## Registros EAX/ECX/EDX
Los llamadores del EXE (0x43467A, 0x4346EF, 0x4346B5, 0x42D07F, 0x432870, 0x4328E7, 0x4328A2, 0x4328C5) no leen EAX/ECX/EDX (el `push ecx` posterior es reserva de espacio sobrescrita por `fstp [esp]`). Solo se registran.

## Banco
Como la tanda A. Arena: 7 entidades (E0 = vehículo), nodos de lista doble, inteligencia de peatón (grupo de eventos en +0x68), código de exploración global (0xB7CD78), reloj, `Players[foco]+0xD8` y `ms_modelInfoPtrs` (restaurados). `fixture_oracle.py` modela el número de llamadas
a GetBoundCentre (filtro de exploración/colisión/modelo/físico) que `run_local.py` exige; el resto lo decide el EXE (EXE == DLL con eventos EN ORDEN). El constructor y el destructor de `CEventPotentialGetRunOver` se ejecutan de verdad en cada lado (EXE: 0x4AE1A0/0x4AE270; DLL: los de C++).

## Suposiciones y límites
- Todos los colaboradores con efectos son falsos; `RegisterReference`/`CleanUpOldReference` no tocan el pool de referencias real.
- Mismos límites x87 que la tanda A (sin infinitos/enormes al cuadrado, un solo valor raro por caso).
- Los falsos no modifican la lista durante el recorrido: el EXE lee `siguiente` antes de cada llamada y la DLL también.

## Comandos
```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlWeave63B
python -I reversing/CarCtrlWeave63B/fixture_oracle.py
python -I reversing/CarCtrlWeave63B/fixture_mirror.py
python -B reversing/CarCtrlWeave63B/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlWeave63B/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador
```
