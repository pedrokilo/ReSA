<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlayerGroup78: CPlayerPed::ProcessGroupBehaviour 0x60D350 (lote 78B)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wy-2`, HEAD e89b38506 + `source.patch`). 20480 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`).
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playergroup78\20261028-041140-618644`.

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `void CPlayerPed::ProcessGroupBehaviour(CPad* pad)` (antes `plugin::CallMethod<0x60D350>`, sin hook) | 0x60D350 | `CPlayerPed` / `ProcessGroupBehaviour` / S (`RH_ScopedInstall`) | `Entity/Ped/PlayerPed.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CPlayerPed,ProcessGroupBehaviour,0x0060D350,1,0,1,S`.

## Que hace (desensamblado 0x60D350..0x60D59A, thiscall `ret 4`, void)
Unico llamador: `CPlayerPed::ProcessControl` (0x60F595), que recarga EAX y no lee ECX/EDX. No es virtual. La unica aritmetica float es el paso de tiempo.
1. Objetivo `ebp` = `[this+0x71C]`; con el byte 0xB6EC2E distinto de cero y sin objetivo, `[this+0x79C]`. Se lee una sola vez. Es peaton si `([obj+0x36] & 7) == 3`; es de tipo 8 si el dword `[obj+0x598] == 8`.
2. Si `FindPlayerVehicle(-1, 0)` (0x56E0D0) es nulo: **cruceta arriba** (`CPad::GetGroupControlForward` 0x541190; el puntero `edi` a los datos del jugador se lee DESPUES de esa llamada y se conserva en toda la rama):
   pulsada: `[edi+0x8A] += low16(_ftol2((paso_sin_recortar 0xB7CB58 * 0.02f 0x858B38) * 1000.0f 0x858C4C))` (add word); con los datos RELEIDOS, si el contador vale exactamente 0x4B0 y el bit 0x100 de `[datos+0x34]` (reclutamiento deshabilitado) esta limpio: `DisbandPlayerGroup` (0x60A0A0).
   soltada: con el contador en (0, 0x4B0): si hay objetivo peaton y (tipo 8 o `0x609380`() verdadero; solo se llama si el tipo no es 8), `MakeThisPedJoinOurGroup(objetivo)` (0x60C840) salvo que el bit 0x100 de `[edi+0x34]` (edi = puntero conservado) este activo; si no, `TellGroupToStartFollowingPlayer(1, 1, 0)` (0x60A1D0). Despues `[datos releidos+0x8A] = 0`.
3. Si el bit 0x100 de `[datos releidos+0x34]` esta activo, termina (sin 2a mitad ni recorrido del pool).
4. Si `FindPlayerVehicle` (2a llamada) es nulo: **cruceta abajo** (`GetGroupControlBack` 0x5411B0; contador `[datos+0x88]`, mismo paso): pulsada, `>= 0x4B0` (sin comprobar el bit 0x100) llama a `DisbandPlayerGroup`; soltada con el contador en (0, 0x4B0): objetivo peaton de tipo 8 -> `MakeThisPedJoinOurGroup` (sin 0x609380 ni comprobar el bit), si no `TellGroupToStartFollowingPlayer(0, 1, 0)`; despues contador = 0.
5. Si `(0xB7CB4C & 0x1F) == 6`: recorre de atras adelante el pool de peatones (0xB74490: objetos, estados, tamano); salta ranuras con el bit 7 del estado; para cada peaton con `[ped+0x598] == 8` lee el nivel de busqueda `[[[FindPlayerPed(-1)+0x480]+0]+0x2C]` (con signo) ANTES de `CPedType::GetPedFlag(6)` (0x608830) y llama a `CAcquaintance::SetAsAcquaintance(4, bandera)` (0x608DA0, nivel > 0) o `ClearAsAcquaintance(4, bandera)` (0x608980) sobre `ped+0x4E0`.

## Cambios de produccion (`source.patch`)
`PlayerPed.cpp`: metodo en C++ puro con `RH_ScopedInstall(ProcessGroupBehaviour, 0x60D350)`; includes de `PedType.h` y `Wanted.h`; reutiliza `JoinGroupFlagsExe()` (0x609380 por su direccion del EXE, sin reconstruir) del lote 75. El paso usa `static_cast<uint32>(static_cast<int64>(...))` (patron de `Bike.cpp`: `_ftol2` de 64 bits, parte baja).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran argumentos EN ORDEN y mutan en 9 sitios el puntero a los datos del jugador, las banderas/contadores de los datos, el objetivo o el nivel de busqueda): FindPlayerVehicle, CPad::GetGroupControlForward/Back, DisbandPlayerGroup, TellGroupToStartFollowingPlayer, MakeThisPedJoinOurGroup, FindPlayerPed, CPedType::GetPedFlag, CAcquaintance::SetAsAcquaintance/ClearAsAcquaintance (todos con hook); sin hook: `0x609380` (falso solo en el EXE, la DLL la llama por esa direccion).
El pool de peatones es un pool falso en arenas (cabecera con la misma disposicion que `CPool`, estados y 6 peatones de 0x7C4 bytes); el puntero global 0xB74490, el byte 0xB6EC2E, el contador 0xB7CB4C y el paso 0xB7CB58 se fijan por caso y se restauran (regiones) tras cada lado. Las constantes `.rdata` 0x858B38 / 0x858C4C se vigilan (0.02f y 1000.0f).
20480 pares: 16 familias de 1024 (base, vehiculo, pulsaciones que llegan a 0x4B0, soltado con objetivo tipo 8 / otro tipo con 0x609380 / seleccion de objetivo / bordes del contador, reclutamiento deshabilitado, cruceta abajo, 2a llamada de vehiculo, pool, fotograma != 6, paso especial, mutaciones) + 1024 de pulsaciones exactas (+-2 y envolturas) + 1024 de pasos especiales (NaN y sNaN, infinitos, desnormales, enormes cerca de 2^31/2^32/2^63, negativos) + 1024 de mutaciones (9 sitios x 15 tipos) + 1024 de pool (tamano 0..6, estados de ranura, tipos 7/9/8, nivel de busqueda con signo y mutado entre la lectura y GetPedFlag).
Los NaN/infinitos/desnormales del paso pasan por el hardware real en el lado del EXE (fld/fmul/_ftol2) y por la DLL; el oraculo calcula el paso con aritmetica racional exacta (mantisa de 24 bits, exponente sin limite, truncacion a int64, indefinido -> 0).

## Pruebas
- Prueba negativa 1 (produccion mutada y restaurada; `PlayerPed.cpp` identico al bueno, `source.patch` regenerado del bueno): cruceta abajo `>= 0x4B0` cambiado por `> 0x4B0`: **549/20480 diferencias de resultado** (corrida 20261028-035628-434608), sin caida; el oraculo discrepa solo en el lado candidato (549 lados).
- Prueba negativa 2: nivel de busqueda leido DESPUES de `GetPedFlag` (el EXE lo lee antes): **221/20480 diferencias de resultado** (corrida 20261028-040256-906156).
- Hallazgos del BANCO: ninguno bloqueante. Produccion: la primera version usaba `target->m_nType` (no existe: el tipo vive en `CEntity::m_info`, se usa `GetType()`); error de compilacion, no de comportamiento.

## Quien valida a cada colaborador y que NO prueba el banco
Cada colaborador tiene su banco (docs/hooks.csv, reversing/validation-registry.json); este banco no valida su correccion: FindPlayerVehicle, FindPlayerPed, CPad (GetGroupControlForward/Back), DisbandPlayerGroup, TellGroupToStartFollowingPlayer, MakeThisPedJoinOurGroup (PlayerGroup75), CPedType::GetPedFlag, CAcquaintance. 0x609380 no tiene hook propio. No cubre: pools de mas de 6 peatones ni el pool real del juego, `FindPlayerPed(-1)` nulo o datos del jugador nulos (el EXE se cae), puntero a datos del jugador nulo, ECX/EDX/EAX de salida (nadie los lee).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh PlayerGroup78`; `python -B reversing/PlayerGroup78/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
