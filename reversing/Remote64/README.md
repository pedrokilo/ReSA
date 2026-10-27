<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Remote64: CRemote::GivePlayerRemoteControlledCar (0x45AB10) y CRemote::TakeRemoteControlledCarFromPlayer (0x45AE80)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wg-2`, HEAD df26d6fe7 + `source.patch`). 12288 pares (8192 Give + 4096 Take), cero diferencias EXE/DLL, todas las compuertas de
ABI/ruta/restauración en ambos lados, `kCompareDeadX87Registers=true` y salida de cada lado igual a la del oráculo independiente. Evidencia:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-remote64\20261020-160950-409492`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CRemote::GivePlayerRemoteControlledCar(CVector, float, int16)` | 0x45AB10 | Simple (nuevo `CRemote::InjectHooks`) | 8192 | 15 |
| `CRemote::TakeRemoteControlledCarFromPlayer(bool)` | 0x45AE80 | Simple | 4096 | 2 |

Ambas son cdecl, `void`. La clase no tenía `InjectHooks`: se añade (`Remote.h/.cpp`, `InjectHooksMain.cpp`). Faltan las filas en `docs/hooks.csv`:
`CRemote,GivePlayerRemoteControlledCar,0x0045AB10,1,0,1,S` y `CRemote,TakeRemoteControlledCarFromPlayer,0x0045AE80,1,0,1,S`.

## Qué hacen (desensamblado)

**Give**: `modelo = movzx word` (SIN signo); helicóptero (`IsHeliModel` 0x4C5B00 → `operator new` 0x6E2D50 con 0xA18 → ctor 0x6C4190(modelo, 2)), avión (0x4C5B30, 0xA04, ctor 0x6C8E20) o automóvil (0x988, ctor 0x6B0A90(modelo, 2, 1)); se usa el valor
que devuelve el constructor. z = `FindGroundZFor3DCoord(x, y, z + 2.0f, &bool, 0)` + `GetDistanceFromCentreOfMassToBaseOfModel` (suma x87 a 24 bits, dejada en el registro muerto T-1); `SetRotateZOnly(rotación)` sobre la
matriz leída (aunque sea nula); posición en la matriz (+0x30..0x38) o, sin matriz, en el placement (+4..0xC), x e y copiados como enteros; estado 8 (+0x36), `+0x428 |= 8`, `CCarCtrl::JoinCarWithRoadSystem`
(0x42F5A0), campos +0x3B7..+0x3D0, bit 0x10 de +0x428 según `+0x42D & 2` (leído tras la llamada), `CWorld::Add`, estado 9 del vehículo del jugador (dos llamadas a `FindPlayerVehicle`),
`Players[foco].m_pRemoteVehicle = vehículo` + `RegisterReference`, `TheCamera.TakeControl(vehículo, 0x12, 1, 1)` y `SetZoomValueCamStringScript(1)`.

**Take**: si el vehículo remoto tiene `m_nCreatedBy == 2`: `SetVehicleCreatedBy(1)` y `CMissionCleanup::RemoveEntityFromList(handle, 1)` con el handle del pool (índice = división con signo hacia cero del
desplazamiento respecto al almacenamiento entre 0xA18, sin comprobar pertenencia; handle = estado del hueco + (índice << 8)); después `+0x428 &= 0xF7` y las escrituras en el jugador en foco (+0xDD = 1, +0xDE = byte del argumento,
+0xDF = 1, +0xE0 = reloj). Cada acceso relee `PlayerInFocus` y el puntero remoto, como el EXE.

## Registros

Llamadores del EXE: 0x48AAB8 (Give), 0x56EA4B, 0x6B4BE8, 0x6B4C26 (Take). Ninguno lee EAX/ECX/EDX tras la llamada (solo EBX/ESI/EDI, preservados): solo se registran.

## Fuente

`Remote.cpp/.h` en C++ puro. Campos sin nombre en las cabeceras por desplazamiento (`RemoteField`). Ayudante x87 nuevo al final de `X87Math.cpp/.h`: `RemoteGiveGroundZCpp` (suma a 24 bits y registro muerto T-1 con
`X87PlantStmt1Cpp`). Los constructores se llaman por dirección del EXE (como `Plane.cpp`).

## Banco

`Original` = EXE con el hook apagado; candidato = DLL con solo ese hook encendido. Los 17 colaboradores son falsos idénticos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL: `CMatrix::SetRotateZOnly` y
`CMissionCleanup::RemoveEntityFromList` no tienen hook y se parchean por puntero a miembro). Los falsos reescriben campos del vehículo (`JoinCarWithRoadSystem`), devuelven otro vehículo desde el constructor y, en Take, cambian el jugador
en foco y el vehículo remoto dentro de `SetVehicleCreatedBy`/`RemoveEntityFromList` (prueba de las relecturas). Los vehículos (6, 0xA18 bytes cada uno), matrices, pool de vehículos falso (almacenamiento y estados) son objetos del banco; el estado
real (foco, reloj, puntero 0xB74494, dos CPlayerInfo) se fija por caso y se restaura. `fixture_oracle.py` modela las funciones desde el desensamblado y también el registro x87 muerto ST7 de Give. Familias: Give 8 (automóvil/helicóptero/avión,
constructor devuelve otro vehículo, sin matriz, vehículo del jugador = el nuevo), Take 8 (creado por misión o no, índice negativo, sin alinear, reentradas).

## Alcance y límites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integración real con pools, cámara, mundo ni RenderWare.
- No se cubren los caminos en que el EXE desreferencia sin comprobar: `operator new` nulo en Give (el EXE sigue con vehículo nulo y falla en `[vehículo+0x14]`) y vehículo remoto nulo en Take (`[ecx+0x4A4]`).
- Valores de suelo y distancia: finitos y sin subnormales (la suma a 24 bits de la x87 y la de la DLL coinciden); x, y, z y rotación incluyen NaN (también señalizantes), infinitos y -0.0.
- El argumento bool de Take lleva 0 o 1 en el byte bajo y basura en los altos; `PlayerInFocus` ∈ {0, 1}.
- Los NaN de los falsos que devuelven float no existen (solo finitos).

## Comandos

```
bash reversing/_handoff/bench_here.sh Remote64       # compila la sonda y corre en el juego (candado global)
python -I reversing/Remote64/fixture_oracle.py       # autoprueba del oráculo
python -I reversing/Remote64/fixture_mirror.py       # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/Remote64/run_local.py --analyze <comparisons.jsonl>
python reversing/Remote64/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Depuración: `remote64-cases.txt` en la carpeta del juego con «primero ultimo» limita los casos de cada función (el analizador final fallará).

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
