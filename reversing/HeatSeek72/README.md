<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HeatSeek72: CVehicle::ScanAndMarkTargetForHeatSeekingMissile 0x6E0400 (hook S, lote 72)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 95f7d73eb + `source.patch`). 16384 pares (ampliado a 20480, ver la ultima seccion), cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-heatseek72\20261026-203547-474164`. (Primera corrida aprobada: `20261026-203103-504541`, sin `coverage.json`; corrida negativa con una mutacion deliberada: `20261026-203408-471466`, 272 diferencias.)

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `CEntity* CVehicle::ScanAndMarkTargetForHeatSeekingMissile(CEntity*)` | 0x6E0400 (thiscall, `ret 4`, 445 bytes hasta 0x6E05BA) | `CVehicle` / `ScanAndMarkTargetForHeatSeekingMissile` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Vehicle.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,ScanAndMarkTargetForHeatSeekingMissile,0x006E0400,1,0,1,S`.

## Que hace (desensamblado 0x6E0400..0x6E05BA)

1. `target = CWeapon::PickTargetForHeatSeekingMissile(origen = posicion, direccion = matriz + 0x10 sin comprobar nulo, 1.2f (0x3F99999A), ignorar = this, aviones con prioridad = true, previo = entity)` (0x73F910, cdecl).
2. En los aviones (`m_nVehicleSubType` +0x594 == 4) el dword +0x9E4 guarda en el bit 31 la ultima vision y en los 31 bits bajos un 1 si la hora era > 1. `holgura = 1` si es avion y los 31 bits bajos son 0; si no, 0.
3. Con objetivo y `hora - holgura > 1000` (sin signo; con hora 0 y holgura 1 envuelve y SI calcula): se quita el bit 0 (usa colision) de las banderas +0x1C del vehiculo y del objetivo, se llama a `CWorld::GetIsLineOfSightClear(pos vehiculo, pos objetivo, true, true, false, true, false, true, false)` (0x56A490), se restaura SOLO el bit 0 de cada uno (los demas bits quedan como los dejo la llamada) y, en los aviones, se guarda `(hora > 1) | (AL & 1) << 31`.
4. Si no se calcula: visible = bit 31 guardado (avion) o verdadero.
5. No visible: `ClearCrossHairImmediately(0)` (0x742CA0) y devuelve nulo. Visible pero sin objetivo, o con el estado de la entidad (byte +0x36, bits 3..7) distinto de 0 (STATUS_PLAYER): borra la mira y devuelve el objetivo. Si no: `MarkTarget(0, pos objetivo, 255, 255, 255, 100, 1.3f (0x3FA66666), true)` (0x742BF0) y devuelve el objetivo.
Sin x87: todo son enteros, copias de dwords y llamadas; no hace falta ningun ayudante de `X87Math`.

## Cambios de produccion (`source.patch`)

`Vehicle.cpp` solo: descomenta `RH_ScopedInstall(ScanAndMarkTargetForHeatSeekingMissile, 0x6E0400)` y reemplaza la envoltura por el cuerpo en C++ puro. El `.h` no cambia. El estado del avion se accede por desplazamiento 0x9E4 (`CPlane::field_9E4`, el banco comprueba el desplazamiento con `static_assert`).

## Banco

`Original` = bytes del EXE con el hook S apagado; `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI, comprobado en cada lado: `route_ok`). Se llama a la entrada con ECX = vehiculo y un argumento (`entity`).
Colaboradores (4, con hook RH en la DLL, apagado durante el banco) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran TODOS sus argumentos en orden: PickTarget, GetIsLineOfSightClear (ademas registra los valores apuntados y los dwords +0x1C de vehiculo y objetivo en ese instante, y los altera con `mut_self`/`mut_tgt`, incluido el bit 0, para exigir que solo se restaure el bit 0; devuelve `AL | basura << 8`), MarkTarget y ClearCrossHairImmediately.
Se compara EAX (etiquetado), los cambios en 5 arenas (vehiculo de 0xA00 bytes, objetivo, entidad previa, matriz del vehiculo y del objetivo), la traza de eventos y el hash de globales (`CTimer::m_snTimeInMilliseconds` se restaura); ECX/EDX solo se registran (los dos llamadores del EXE, 0x6E3621 y 0x6E36DC, solo leen EAX).
16384 pares = 16 familias de 1024: sin objetivo (no avion / avion), no avion con vision libre / tapada / sin calcular, avion con y sin holgura alrededor del umbral de 1000, avion con vision calculada y con vision guardada, estado distinto de jugador, objetivo = el propio vehiculo, mutacion de banderas, valores especiales (NaN canonicos, infinitos, desnormales, enormes), objetivo sin matriz y mezcla. `coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).
`fixture_oracle.py` es un modelo independiente en Python (desde el desensamblado, no desde la fuente C++): `python -B fixture_oracle.py <comparisons.jsonl>` reproduce EAX, la traza y los bytes cambiados de los 16384 casos en ambos lados (0 diferencias).
Prueba negativa: con `holgura = 0` siempre (mutacion deliberada) el banco falla con 272/16384 diferencias (los casos de avion con hora 0 y bits bajos a cero, y los de hora 1001 con holgura).

No cubierto: el vehiculo siempre tiene matriz (el EXE lee `matriz + 0x10` sin comprobar nulo y se cae sin ella; la rama de posicion compacta del vehiculo es inalcanzable); `m_nVehicleSubType == 4` en un vehiculo que no sea `CPlane` (el EXE y la DLL leen el mismo dword +0x9E4; el banco lo hace sobre una ventana de 0xA00 bytes); la validez de los cuatro colaboradores no la prueba este banco; no se prueba el AL con valores distintos de 0/1 devuelto por GetIsLineOfSightClear (el EXE solo guarda el bit 0).

## Ampliacion 2026-10-26 (20480 pares)

Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-heatseek72\20261026-220606-191000` (APROBADO, 20480 pares, 0 diferencias, oraculo Python 0 lados distintos). Corrida negativa (mutacion deliberada): `20261026-220419-351256`, 1191/20480 diferencias. Los 16384 casos originales no cambian (mismas entradas); se anaden 4096 (familias 16..19) y minimos nuevos en `coverage.json` (50 % de la primera corrida; los antiguos no se tocan).

- **CPlane real.** La arena del vehiculo mide `sizeof(CPlane)` = 0xA04 (`static_assert`). El estado del avion es el miembro real `CPlane::field_9E4` (int32, +0x9E4; en las fuentes no tiene otro nombre) y el subtipo `CVehicle::m_nVehicleSubType` (+0x594); el EXE los lee en `[esi+0x9E4]` (6E0464, 6E0534, 6E0540) y `[esi+0x594]` (6E0454). Las familias 18 y 19 (2048 casos) escriben y leen por los miembros de `CPlane` (`HsPlane : CPlane`) y el banco compara `plane_state`/`plane_sub` leidos asi en ambos lados; las demas siguen por desplazamiento (ventana, ahora de 0xA04). No se construye un CPlane con su constructor (arrastra estado del motor): es un CPlane con layout verificado en memoria de la arena.
- **Sin matriz.** Desensamblado: 6E0408 `test ecx,ecx` solo protege la posicion (cae a `esi+4`), pero 6E041B/6E041E hacen `add ecx,0x10; mov edi,[ecx]` y con matriz nula leen 0x10 y se caen antes de cualquier llamada; el segundo `test eax,eax` de 6E04CD es codigo muerto. La rama «vehiculo sin matriz» es inalcanzable por diseno (el EXE se cae); solo el objetivo sin matriz es alcanzable y ya esta cubierto.
- **AL distinto de 0/1.** Familias 16..19: el falso de GetIsLineOfSightClear devuelve AL = 2, 0x80, 0xFF, 0x03, 0x7F, 0xFE, 0x81, 0xC0, 0x40, 0, 1 y aleatorios con bits altos de EAX basura. El EXE decide con `test al,al` (6E054B: visible = AL != 0) y guarda `movzx ecx,al; shl ecx,0x1F` (6E0524: solo el bit 0 de AL en el estado del avion; AL = 2/0x80 → visible esa llamada y bit 31 = 0). EXE y DLL coinciden en todos (la DLL hace `uint32(visible) << 31` sobre un bool que conserva AL): no hay defecto de produccion. La prueba negativa (guardar `visible ? 1 : 0`) da 1191 diferencias, justo los casos de avion con AL par distinto de 0.
- **Colaboradores sustituidos** (este banco no prueba su validez; la prueban otros): `CWeaponEffects::ClearCrossHairImmediately` 0x742CA0 (`reversing/Ped9`, 64 pares) y `CWeaponEffects::MarkTarget` 0x742BF0 (`reversing/Ped9`, 128 pares), `CWorld::GetIsLineOfSightClear` 0x56A490 (`reversing/WldB`, 1024 pares), `CWeapon::PickTargetForHeatSeekingMissile` 0x73F910 (`reversing/PickWeap1`, 256 pares); los cuatro con `validated=1` en `docs/hooks.csv` y en `reversing/validation-registry.json`.
- **Sigue fuera:** un CPlane construido por su constructor/pool; el vehiculo sin matriz (inalcanzable); la validez de los colaboradores en este banco.

## Comandos

```
bash reversing/_handoff/bench_here.sh HeatSeek72                                  # compila la sonda y corre en el juego (candado global)
python -B reversing/HeatSeek72/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/HeatSeek72/fixture_oracle.py <carpeta>/comparisons.jsonl
python -B reversing/HeatSeek72/register_hook.py <carpeta-de-evidencia>            # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `heatseek72-runtime.start`; salida `heatseek72-runtime.jsonl`.
