<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 880 grupos / 9863 funciones reconstruidas; 9863 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn114A: catorce funciones sueltas de Placeable, Physical, PedIntelligence, PedGroups, MissionCleanup, eventos, EntryExitManager, CPointRoute y pool de peds (lote 114A)

Estado: **APROBADO dentro del juego** (06/11/2026, worktree `wv-1` sobre main 7f321c843 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn114a/20261106-173314-375442` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn114A-20261106-173314-375442`). Banco par a par dentro del juego, 14 x 16384 = 229376 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 igual (ST0 de GetPedFOVRange incluido) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

| Funcion | Direccion | Bytes | Fuente |
|---|---|---|---|
| `CPlaceable::GetRightVector` (el catalogo decia CCarAI) | 0x41CC70 | 59 | `Entity/Placeable.cpp` |
| `CPlaceable::GetUpVector` (el catalogo decia CCam) | 0x50E420 | 66 | `Entity/Placeable.cpp` |
| `CPhysical::ApplyMoveForce(x, y, z)` | 0x4ABBA0 | 63 | `Entity/Physical.cpp` |
| `CPedIntelligence::GetPedFOVRange` | 0x4ABEA0 | 33 | `PedIntelligence.cpp` |
| `CPedGroups::RegisterKillByPlayer` | 0x5F7E30 | 16 | `PedGroups.cpp` |
| `CMissionCleanup::AddEntityToList(handle, tipo)` (el catalogo decia CTaskSimpleFinishBrain) | 0x4637E0 | 44 | `MissionCleanup.cpp` |
| `CEventDamage::IsSameEventForAI` | 0x4ADBF0 | 30 | `Events/EventDamage.h` (en linea; hook en `EventDamage.cpp`) |
| `CEntryExitManager::WeAreInInteriorTransition` | 0x43E400 | 11 | `EntryExitManager.cpp` |
| `CPointRoute::AddPoint` (nuevo; antes plantilla `Route`) | 0x48E180 | 48 | `PedGeometryAnalyser.cpp/.h` |
| `CPointRoute::RemovePoint` (nuevo; sustituye a `FootstepsRouteRemove`) | 0x5F13A0 | 61 | `PedGeometryAnalyser.cpp/.h` |
| `CPedGroupIntelligence::GetTask` | 0x5F7660 | 46 | `PedGroupIntelligence.cpp` |
| `CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent` | 0x4AE100 | 149 | `Events/EventGroupEvent.cpp` |
| `CEvent::TakesPriorityOverBase` (nuevo: cuerpo por defecto de `TakesPriorityOver`; el catalogo decia CEventSource) | 0x4ABC30 | 36 | `Events/Event.cpp/.h` |
| `IsPedPointerValid` (se engancha como `CPed`) | 0x5E8A30 | 125 | `Entity/Ped/Ped.cpp` |

Doce ya tenian C++ sin hook; el lote las engancha y valida. Cambios de comportamiento para igualar al EXE: `GetPedFOVRange` ya no usa `std::max` (con NaN el EXE devuelve la vista); `BaseEventTakesPriorityOverBaseEvent` calcula las dos prioridades antes de decidir (orden y numero de llamadas virtuales); `TakesPriorityOverBase` consulta primero la prioridad del otro evento; `IsPedPointerValid` usa el indice con signo del pool (`GetIndexUnchecked`) en vez de `IsObjectValid`. Ninguna usa `__asm`/`naked`; no se tocan opciones de compilacion ni `X87Math.cpp`.

## Revision contra el EXE
- **GetRightVector (thiscall, `ret 4`, EAX = destino)**: con matriz copia en crudo los tres dwords de matriz + 0; sin ella `fsin(h)`, `fcos(h)` y escribe z = 0, x = cos, y = sin (floats).
- **GetUpVector**: con matriz copia en crudo matriz + 0x20; sin ella (0, 0, 1.0f).
- **ApplyMoveForce(x,y,z) (`ret 0Ch`)**: pasa los tres dwords en crudo a `ApplyMoveForce(CVector)` (0x5429F0).
- **GetPedFOVRange**: `fld oido(+0xBC); fcomp vista(+0xC0); test ah,41h`: devuelve el oido solo si es estrictamente mayor.
- **RegisterKillByPlayer**: si el byte 0xC098E8 es distinto de cero incrementa el dword 0xC098EC.
- **AddEntityToList (`ret 8`)**: primer hueco de 75 (byte de tipo 0): guarda el byte bajo del tipo y el handle e incrementa el byte +0x258.
- **IsSameEventForAI**: fuente (+0x14) y arma (+0x1C) iguales.
- **WeAreInInteriorTransition**: `[0x96A7CC] != 0`.
- **AddPoint**: contador con signo < 8: copia en crudo el punto a la entrada contador, incrementa y devuelve true.
- **RemovePoint**: desde el indice (con signo) mientras `i < contador - 1` copia la entrada siguiente; luego decrementa. Con indice -1 la entrada -1 solapa el contador (el EXE lo relee cada vuelta): no se prueba con pasadas.
- **GetTask (`ret 8`, ECX sin usar)**: primer par (8 de 0x14 bytes) con el ped; devuelve su tarea o 0.
- **BaseEventTakesPriorityOverBaseEvent**: GetEventType (ranura 1) del propio y del otro; tipos 0x36, 0x37, 0x39, 0x47, 0x4C, 7, 0x4D son prioritarios; si no, salta a la ranura 11 con el evento del otro.
- **TakesPriorityOverBase**: prioridad (ranura 2) del otro y luego la propia; `>=` con signo.
- **IsPedPointerValid**: indice = (ped - almacenamiento) / 0x7C4 con signo, 0 <= indice < capacidad y hueco ocupado; con bit 8 de +0x46C y vehiculo no nulo devuelve `IsEntityPointerValid(vehiculo)`; si no, nodo de colision (+0xB0) o `ped == FindPlayerPed(-1)`.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: 0x5429F0 (funcion 2), 0x533310 y 0x56E210 (funcion 13); las ranuras 1, 2 y 11 de las vtablas de eventos son falsos de una vtabla propia. Corren de verdad: fsin/fcos, el pool de peds (arena con su estructura; el puntero 0xB74490 se apunta a ella y se restaura) y las globales 0xC098E8/0xC098EC/0x96A7CC (se restauran). 16 familias de 1024 casos por funcion (ver `Fixtures.hpp`).

## Fuera de los fixtures
Punteros de objeto nulos, rumbos con |h| > 8 sin matriz, contadores de ruta por debajo de -5, indices de ruta negativos con pasadas, peds fuera del almacenamiento con indice en rango y la exactitud de los colaboradores falseados.

## Prueba negativa
Copias buenas en `D:/Original/wv1_good` (SHA256.txt), restauradas con comprobacion de suma.
- Ronda 1 (constantes y operadores, 12 funciones): el banco FALLA con 9696, 5858, 3923, 8716, 14978, 8279, 1412, 1636, 8664, 4512, 3045 y 4587 pares (GetRightVector ... TakesPriorityOverBase); IsPedPointerValid (`idx > capacidad`) la detecta el `assert` de `Pool.h` que aborta la corrida.
- Ronda 2 (fronteras, orden y signo, 14 funciones): FALLA con 5409, 10526, 13913, 3923, 16384, 13891, 5390, 3993, 12700, 16384, 2651, 14336, 13698 y 1134 pares.
