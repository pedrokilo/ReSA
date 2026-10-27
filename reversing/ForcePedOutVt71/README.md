<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ForcePedOutVt71: virtuales de CTaskSimpleCarForcePedOut (vtable 0x86EE94, lote 71)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 1b2235ec3 + `source.patch`). 5 x 16384 = 81920 pares, cero diferencias, todas las compuertas de ABI/ruta/SEH/restauracion en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-forcepedoutvt71\20261026-184051-008179`.

Motivo: con el constructor enganchado (ForcePedOut71) todas las instancias de la clase, tambien las que crea el EXE (Clone 0x649F1D), llevan la vtable de la DLL; sus virtuales pasan a ser codigo de la DLL en vivo y se validan aqui.

Filas de `docs/hooks.csv` (las anade el coordinador): `CTaskSimpleCarForcePedOut,Destructor,0x00647790,1,0,1,S` y tres filas V (`RH_ScopedVMTInstall`, direccion `0x00000000`) para `Clone` (0x649EE0, ranura 1), `MakeAbortable` (0x647780, ranura 6) y `ProcessPed` (0x6477F0, ranura 7).

| Funcion | Direccion | Hook | Colaboradores sustituidos (EXE y cuerpo de la DLL salvo nota) |
|---|---|---|---|
| Destructor (cuerpo) | 0x647790 | S, `RH_ScopedInstall(Destructor)` | `CEntity::CleanUpOldReference` 0x571A00; `CTask::~CTask` 0x61A3A0 solo EXE (en linea en la DLL) |
| Clone | 0x649EE0 | V ranura 1 | `CTask::operator new` 0x61A5A0, constructor 0x647710 |
| MakeAbortable | 0x647780 | V ranura 6 | ninguno |
| ProcessPed | 0x6477F0 | V ranura 7 | `CVehicle::IsDriver(ped)` 0x6D1C40, `CPed::PositionPedOutOfCollision` 0x5E0820, `CCarEnterExit::ComputeOppositeDoorFlag` 0x64E610, `ComputePassengerIndexFromCarDoor` 0x64F1E0 |
| Destructor con borrado | 0x64BF50 (EXE) vs el de la DLL | sin hook (ruta viva) | cuerpo del destructor 0x647790 (fake en ambos), `CTask::operator delete` 0x61A5B0 |

## Desensamblado verificado
- 0x647790: marco SEH; vtable 0x86EE94; si `m_pVehicle` no es nulo, `CleanUpOldReference(this+8)` (0x571A00, ECX = vehiculo); luego 0x61A3A0 (`[this] = 0x86D48C`). No pone a cero el campo. Unico llamador: 0x64BF53 (el destructor con borrado, que no lee EAX/ECX/EDX).
- 0x64BF50 (ranura 0, `ret 4`): llama a 0x647790 y, con el bit 0 del argumento, a `CTask::operator delete` (0x61A5B0); EAX = this.
- 0x649EE0 (Clone): `CTask::operator new(0x10)`; nulo -> devuelve nulo; si no, constructor (m_pVehicle, m_nDoor) y devuelve el objeto.
- 0x647780: `xor al,al; ret 0Ch`.
- 0x6477F0 (ProcessPed, 0x6477F0..0x6478A9, `ret 4`): sin vehiculo -> true. `IsDriver(ped)` -> `PositionPedOutOfCollision(puerta, vehiculo, true)` y true. Si no: `ComputeOppositeDoorFlag(vehiculo, puerta, false)` (solo AL) `&` byte +0x487 (`m_nGettingOutFlags`); si es distinto de cero: conductor (+0x460) no nulo -> false; si no, para i en [0, `ComputePassengerIndexFromCarDoor`) (bucle con signo, lectura cruda de los asientos +0x464) cualquier pasajero no nulo -> false. Si no, `PositionPedOutOfCollision(puerta, vehiculo, true)` y true.

## Cambios de produccion (`source.patch`)
`TaskSimpleCarForcePedOut.h/.cpp`: ProcessPed reconstruido (ya no reenvia al EXE), `Clone` con comprobacion de nulo y `::new (mem)` sin parentesis, `MakeAbortable` (false), y `RH_ScopedInstall(Destructor, 0x647790)` + `RH_ScopedVMTInstall` de Clone, MakeAbortable y ProcessPed. Sin `__asm` ni `naked`. El destructor con borrado de la DLL (ranura 0) lo genera el compilador.

## Banco
Original = bytes del EXE con el hook de la funcion apagado; candidato = DLL con SOLO ese hook encendido. Las virtuales se llaman por la ranura de la vtable del EXE (original: ranura = entrada; candidato: ranura en el ASI). Objeto en arena (guardas de 0x40) con relleno semilla/0xCD/ceros/0xFF, vehiculo nulo/arena/crudo (ProcessPed: nulo o arena, el EXE lo desreferencia), peaton nulo/arena/crudo, puertas validas/extremas, valores de retorno de los colaboradores con basura en los bits altos (IsDriver, ComputeOppositeDoorFlag) y asientos negativos/grandes. Se compara el objeto y las arenas completas, AL/EAX segun lo que lee el llamador, la traza de colaboradores en orden y las compuertas. ECX/EDX solo se registran. 16 familias x 1024 por funcion; `coverage.json` = 50 % de la primera corrida aprobada.
Ruta viva (destructor con borrado): original = 0x64BF50 sobre un objeto con la vtable del EXE; candidato = ranura 0 de la vtable de la DLL sobre un objeto con esa vtable; banderas 0/1/bits altos.

## Hallazgos
- Sin defectos de produccion. Defecto del banco (corregido en la fixture, primera corrida con FALLO y cuelgue): el destructor con borrado de la DLL es el «vector deleting destructor» del compilador; su argumento tiene el bit 1 (borrar como array) y el bit 2 (usar `::operator delete` global), que el EXE ignora. Con basura en esos bits la DLL llama a `::operator delete` con memoria de la arena. Ningun llamador los pasa (la DLL emplea 0 o 1 con `delete`; el EXE, 0 o 1), asi que las banderas del banco enmascaran los bits 1 y 2.
- Tras esa correccion: 0 diferencias, incluida la ruta viva.

No cubierto: el cuerpo de `CTask::~CTask` de la DLL (la vtable final es la de CTask de la DLL, no 0x86D48C: se comprueba su origen, no su valor); `GetTaskType` (comprobado de paso en ForcePedOut71 via la ranura 4); la validez de los colaboradores (otros bancos); los bits 1 y 2 de las banderas del destructor con borrado.

## Comandos
```
bash reversing/_handoff/bench_here.sh ForcePedOutVt71
python -B reversing/ForcePedOutVt71/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/ForcePedOutVt71/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador
```
