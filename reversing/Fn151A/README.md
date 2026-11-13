<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 903 grupos / 10577 funciones reconstruidas; 10577 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn151A: 24 funciones diminutas del EXE (lote 151A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-1` sobre main 4745701205 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn151a/20261110-042109-080259` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn151A-20261110-042109-080259`, `comparisons.jsonl.gz` de 52 MB). 24 x 16384 = **393216 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, que ademas exige que 0x61A350 conserve EAX/ECX/EDX y 0x5F7200 conserve EDX). SHA256 de la sonda ejecutada: `d77c3831dc9022300f69caf3815d9695c9310d6b7fbc81e68f14f537cc9b9cd9`; del EXE: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 24 direcciones tenia hook (codigo, `docs/hooks.csv` y los worktrees wv-2/wv-3). Las clases del listado eran por cercania y la mayoria erroneas; los nombres salen del desensamblado, de las vtables y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CTaskComplexSeekEntity<XYOffset>` destructor con borrado (vtable 0x86FBB4) | 0x661F10 | 30 | `Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h` | S |
| 1 | `CTaskComplexSeekEntity<XYOffset>::GetSeekPos` | 0x665DC0 | 61 | idem | S |
| 2 | `CInformGroupEvent` copia de ~ (0x4B29A0) | 0x4AC370 | 50 | `InformGroupEvent.cpp` | S (Destructor-2) |
| 3 | `CInformGroupEvent` constructor con 4 argumentos | 0x4AC3B0 | 51 | idem | S (Constructor-2) |
| 4 | `CTaskComplexPartnerShove` cuerpo de destructor | 0x684750 | 11 | `.../TaskComplexPartnerShove.cpp` | S |
| 5 | `CTaskComplexWalkAlongsidePed` destructor con borrado | 0x684760 | 30 | `.../TaskComplexWalkAlongsidePed.cpp` | S |
| 6 | `CTaskComplexWaitForPed` destructor con borrado | 0x684780 | 30 | `.../TaskComplexWaitForPed.cpp` | S |
| 7 | `CTaskComplexBeInCouple` destructor con borrado | 0x6847A0 | 30 | `.../TaskComplexBeInCouple.cpp` | S |
| 8 | `CTaskSimpleFacial` cuerpo de destructor (ya existia sin hook) | 0x690CB0 | 11 | `.../TaskSimpleFacial.cpp` | S |
| 9 | `CTaskComplexFacial` destructor con borrado | 0x692F90 | 30 | `.../TaskComplexFacial.cpp` | S |
| 10 | `CTaskSimpleAffectSecondaryBehaviour` destructor con borrado | 0x692FB0 | 30 | `.../TaskSimpleAffectSecondaryBehaviour.cpp/.h` | S |
| 11 | `CTaskSimpleHoldEntity` destructor con borrado | 0x692FD0 | 30 | `.../TaskSimpleHoldEntity.cpp` | S |
| 12 | `CTaskSimpleChat` constructor (el listado decia ...SitInLeaderCar) | 0x5F65A0 | 51 | `.../TaskSimpleChat.cpp/.h` | S |
| 13 | `CPedGroupDefaultTaskAllocators::Get` | 0x5F7200 | 54 | `Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.cpp` (nuevo) + `.h` | S |
| 14 | `CTaskComplexGoPickUpEntity::GetTaskType` (0x136) | 0x691A40 | 6 | `.../TaskComplexGoPickUpEntity.cpp/.h` | V |
| 15 | `CTaskComplexGoPickUpEntity::Clone` | 0x692C80 | 100 | idem | V |
| 16 | `CEventEscalator` destructor con borrado | 0x602BE0 | 30 | `Events/EventEscalator.cpp` | S |
| 17 | `CEventEscalator::Clone` | 0x602B90 | 80 | idem | V |
| 18 | `CTask` copia de ~ (0x61A3A0) | 0x61A350 | 7 | `Tasks/Task.cpp` | S (Destructor-2) |
| 19 | `CTaskComplex::SetSubTask` (ranura 7 de ~100 vtables) | 0x61A430 | 40 | `Tasks/Task.cpp` | S |
| 20 | `GetPlayerRelationshipRecorder` | 0x61A2E0 | 64 | `PlayerRelationshipRecorder.cpp` | S |
| 21 | `CTaskSimpleLookAbout` cuerpo de destructor (`jmp 0x61A6F0`) | 0x48EA90 | 5 | `.../TaskSimpleLookAbout.cpp` | S |
| 22 | `CTaskSimpleHandsUp` destructor con borrado | 0x48EA40 | 30 | `.../TaskSimpleHandsUp.cpp` | S |
| 23 | `CTaskSimpleLookAbout` destructor con borrado | 0x48EA70 | 30 | idem | S |

24 filas nuevas en `docs/hooks.csv` (V con `0x00000000`); sin `sync_docs`/`hooks2md`. `X87Math.cpp` no se toca.

## Funciones saltadas
Ninguna. Notas: 0x48EA90 es `jmp 0x61A6F0` (cuerpo de destructor con precedente 0x48EA60, no un salto sin logica); 0x61A430 va como S (no V) porque su entrada sirve a unas cien vtables; 0x5F7200 es viable porque el .cpp nuevo se compila con pragma optimize (`source/CMakeLists.txt`) y solo usa EAX (EDX intacto, comprobado en el objeto y en el banco).

## Revision contra el EXE (resumen)
- **GetSeekPos (0x665DC0)**: el calculador (objeto en +0x40) se llama por la vtable con (peaton, entidad +0xC, temporal local) y el temporal se copia a la salida despues. La primera version devolvia el `CVector` con NRVO: la DLL escribia la salida antes de la llamada y, si solapaba la vtable del calculador, el juego se caia (lo detecto el banco). Ahora usa temporal propio y copia.
- **Constructor de Chat (0x5F65A0)**: llama al constructor enganchado 0x61ABE0 (grupo 0, anim 0xC, 4.0, -4.0, duracion, tipo 0x1A0, "Chat", 0), deja la vtable del EXE 0x86C78C y devuelve this.
- **Clone de GoPickUpEntity / CEventEscalator**: CTask::operator new / CEvent::operator new + constructor enganchado (el de CEvent escribe despues la vtable 0x86CD00; GoPickUp devuelve el EAX del constructor).
- **SetSubTask**: borra la subtarea antigua (virtual con 1) antes de guardar la nueva y pone el padre (+4).
- **GetPlayerRelationshipRecorder**: reserva con el operator new del CRT (0x82119A, por esa direccion) y pone a cero solo el dword del peaton y el byte bajo de cada entrada (el constructor C++ ponia a cero la relacion entera).
- **CTaskComplexWaitForPed 0x6833D0** (sin hook ni banco propio): escrito como funcion propia (vtable, `CleanUpOldReference` del ped, ~CTaskComplex); aqui solo se falsea.

## Banco
Plantilla Fn150A. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (H/U/F/E); vtables falsas en las arenas; `MakeCollab` apaga tambien el hook 0x4AC010 porque 0x5FF860 es `jmp 0x4AC010`. Se observan bytes cambiados de las tres arenas, eventos de los falsos, valor devuelto, la variable 0xC17084 y el estado x87. 16 familias de 1024 casos por funcion. Un segundo hook sobre la misma funcion `CTask::Destructor` (para 0x61A350) confundia la ruta del colaborador: la copia usa una funcion propia (`CTaskBody::DestructorCopy`).

## Prueba negativa
Dos rondas de 26 mutaciones (24 funciones; `mutate.py`), 2048 casos por funcion. **Las 24 fallan en ambas** (`negative-runs/`: `ronda1-20261110-044119-117074` con 14689 de 49152 pares con diferencias, `ronda2-20261110-044459-107971` con 17406; lado EXE fuera del oraculo 0). Fuentes restauradas con `mutate.py restore` (SHA256 identico).

## Bancos repetidos (citan direcciones del lote)
Aprobados con el codigo final: `Fn128A` (0x690CB0; adaptado: ya es colaborador con hook, F), `PartnerChatSeq30` (0x5F65A0), `Tasks18` (0x61A430) y `WalkRoundControl31` (0x61A430, en su README). Evidencia refrescada con `refresh_bank.py`. `Helpers65B` (README cita 0x692C80 y 0x61A430) ya no compila por un error previo de su propio `LiveComparison.hpp` (`Addr` con `CAnimBlock*(*)(int32)`): no se repite, pendiente.
