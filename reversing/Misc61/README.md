<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Misc61: IntegrityCheck, IsSpottedEntity, ProcessPed (SlideToCoord) y GetLightingFromCollision

Estado: **APROBADO dentro del juego** (19/10/2026, worktree `D:\Original\wd-4`, HEAD d349867fb + fuentes del lote 61). 16384 pares (4 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados y salida de cada lado igual a la del oráculo independiente. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-misc61\20261019-222248-576430`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CMemoryHeap::IntegrityCheck` | 0x72E8E0 | Simple (nuevo: `CMemoryHeap::InjectHooks`) | 4096 | ninguno |
| `CSearchLight::IsSpottedEntity` | 0x493900 | Simple (nuevo: `CSearchLight::InjectHooks`) | 4096 | `GetActualScriptThingIndex` 0x4839A0 (con hook), 0x493280 (sin reconstruir; la DLL lo llama por dirección) |
| `CTaskSimpleSlideToCoord::ProcessPed` | 0x66C4E0 | **Virtual** (`RH_ScopedVMTInstall`) | 4096 | `CTaskSimpleRunNamedAnim::ProcessPed` 0x61BF20, `CTaskSimpleStandStill::ProcessPed` 0x62F370, `CGeneral::LimitRadianAngle` 0x53CB50 (los tres con hook) |
| `CTrainNode::GetLightingFromCollision` | 0x6F5F80 | Simple (nuevo: `CTrainNode::InjectHooks`) | 4096 | `CWorld::ProcessVerticalLine` 0x5674E0 (con hook) |

```
bash reversing/_handoff/bench_here.sh Misc61                                           # compila la sonda y corre el banco (candado global del juego)
# o a mano, siempre en serie:
python -B reversing/Misc61/build_probe.py
python -B reversing/Misc61/run_local.py
python -B reversing/Misc61/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/Misc61/fixture_oracle.py                                              # «ok» (autoprueba del oráculo)
python -I reversing/Misc61/fixture_mirror.py                                              # compila Fixtures.hpp con cl: arenas/constantes C++ == oráculo + cobertura >= mínimos
python -B reversing/Misc61/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-misc61/20261019-222248-576430"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador): `CMemoryHeap,IntegrityCheck,0x0072E8E0,1,0,1,S`, `CSearchLight,IsSpottedEntity,0x00493900,1,0,1,S`,
`CTaskSimpleSlideToCoord,ProcessPed,0x00000000,1,0,1,V` (virtual; `original_address` 0x0066C4E0 en el registro) y `CTrainNode,GetLightingFromCollision,0x006F5F80,1,0,1,S`.
`SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque en el clon: `misc61-runtime.start`; salida `misc61-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas cuatro funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

- `Memory/MemoryHeap.cpp/.h`: `CMemoryHeap::InjectHooks` (+ llamada en `InjectHooksMain.cpp`) y `IntegrityCheck` en C++ (era `plugin::CallMethod<0x72E8E0>`). La dirección 0x72E8E0 ya estaba en el comentario del fuente; confirmada con su único llamador, `ParseHeap` 0x72E9E0 (llamada en 0x72EA05).
- `SearchLight.cpp`: `InjectHooks` (+ llamada en `InjectHooksMain.cpp`, bloque Scripts) e `IsSpottedEntity` en C++.
- `TrainNode.cpp/.h`: `InjectHooks` (+ llamada tras `CTrain::InjectHooks`), `GetLightingFromCollision` corregido y envoltorio `GetLightingFromCollisionExeAbi` (destino del hook, nombre «GetLightingFromCollision»).
- `Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp`: `RH_ScopedVirtualClass(…, 0x86FFEC, 9)` y `RH_ScopedVMTInstall(ProcessPed, 0x66C4E0)` activados (el resto de líneas siguen comentadas) y `ProcessPed` reescrito.
- `X87Math.cpp/.h` (bloque «Lote 61» al final): `SlideToCoordShiftCpp` y `SlideToCoordAngleDeltaCpp` (aritmética x87 de 24 bits con exponente extendido; el archivo se compila `/O2 /Oi /arch:IA32 /fp:fast`).

## Fase 2: revisión contra el EXE (re-desensamblado y contrastado instrucción a instrucción tras escribir)

**IntegrityCheck (0x72E8E0..0x72E97A).** Recorre `begin.m_Next` hasta `&end`; si `m_paCommonSizes != 0`, 64 listas `m_ListStart.m_Next` hasta `&m_ListEnd` (paso 0x3C); 20 acumuladores a cero;
bloques de `m_FirstBlock` a `m_LastBlock` (siguiente = bloque + `m_nSize` + 0x10) con `inUse != 0 && memId != -1` → `acum[movsx memId] += size + 0x10` (sin comprobar el rango); cadena `m_LastBlock->m_PrevBlock` hasta 0.
No escribe en el montón: sólo falla (bucle infinito/acceso inválido) si está corrupto. **Reconstruible de forma segura**: depende de estado del montón sólo en lo que lee; el banco usa un montón falso con cada estructura en su propia página
`PAGE_GUARD`, de modo que el **orden de primera lectura de cada página** delata el orden del recorrido (la DLL y el EXE coinciden con el oráculo en los 4096 casos). Los punteros del recorrido y el acumulador son `volatile` para que un build
optimizado no elimine los bucles. Riesgo conocido, igual que en el EXE: un `memId` en uso fuera de `[-1, 19]` escribe fuera del arreglo local (no se prueba). Ningún llamador lee EAX/ECX/EDX (`ParseHeap` usa ESI = this, que la función conserva).

**IsSpottedEntity (0x493900..0x493959).** `0x4839A0(index, 2)` (cdecl); `< 0` (con signo) → 0. Copia la posición (matriz `[entity+14h]` → +0x30, si no `entity+4`) a una variable local y llama a `0x493280(&pos, idx)` (cdecl, sin reconstruir);
devuelve **1/0 según `test al,al`** (no el byte tal cual: la DLL compara `!= 0`). Los dos llamadores (llamadas en 0x49767E y 0x49799F) sólo hacen `test al,al`: nada lee EAX/ECX/EDX.

**GetLightingFromCollision (0x6F5F80..0x6F6020).** Si `[this+9]` (byte, cualquier valor ≠ 0) → `[this+8]`. Si no, `pos = (movsx x · 0.125, movsx y · 0.125, movsx z · 0.125 + 1.0)` y
`ProcessVerticalLine(&pos, -1000.0f, &cp, &entity, true, false, false, false, false, false, nullptr)` (11 argumentos, `add esp,2Ch`); sin colisión devuelve 0x48 **sin** cachear; con colisión cachea y devuelve `cp[+0x25]` (`m_nLightingB`)
y pone `[this+9] = 1`. Los cuatro llamadores (0x6F6E90, 0x6F6EE9, 0x6F951B, 0x6F9534) usan sólo AL (`GetCurrentLighting` 0x59F0C0 lee un byte de la pila).

**ProcessPed (0x66C4E0..0x66C717, marco SEH, `ret 4`).** Orden: `RunNamedAnim::ProcessPed` (0x61BF20, sólo si el bit 1 de `[this+78h]`); temporizador `[this+7Ch]` (−1 → ahora+2000 sin animación, ahora+500 si la animación devolvió AL ≠ 0);
primera vez (bit 0): tarea local `CTaskSimpleStandStill(20000, false, false, 8.0f)`, su `ProcessPed`, si `IsPlayer` (`[ped+598h]` ∈ {0, 1}) `[[ped+47Ch]+14h]->MakeAbortable(ped, 2, nullptr)` (ranura 6), `[ped+55Ch] = [this+70h]`, bit 0 a 0;
posición del ped; `sum = dy·dy + dx·dx` con `dx = px − tx`; `fcompp` con 0.05f² y `test ah,1`: «menor o NaN» → desplazamiento {0, 0}; si no `V = (target − pos) · speed`, `[ped+4D8h] = V·right`, `[ped+4DCh] = V·forward`;
`[this+7Ch] < ahora` (sin signo) → 1; `!animOk` → 0; `!(sum < 0.05f²)` (estricto, NaN → false) → 0; si no `|LimitRadianAngle(cur − aim)| < 0.1f` (estricto, NaN → false). Nada lee ECX/EDX/EAX de salida (llamadas virtuales genéricas; sólo AL).

### Discrepancias halladas y corregidas (borradores del lote 27 / código previo y defectos detectados por el propio banco)
1. **ProcessPed — campo del temporizador:** el borrador usaba `m_Time` (campo de `CTaskSimpleRunNamedAnim`); el EXE usa el dword `+0x7C` = `m_Timer` de la propia clase (comparación sin signo, sumas con envoltura).
2. **ProcessPed — ejes del desplazamiento:** el borrador asignaba `x = forward`, `y = right`; el EXE guarda `[ped+4D8h] = V·right` y `[ped+4DCh] = V·forward`, con la suma `(V.z·m.z + V.y·m.y) + V.x·m.x` y **tres rondas distintas** (D3, V1 y V3 pasan por `fstp dword`; V2 queda en registro: la parte de `forward` usa V3 sin redondear a float y la de `right` el V3 guardado).
3. **ProcessPed — distancia y NaN:** el borrador medía la magnitud 2D del vector ya multiplicado por la velocidad; el EXE mide `px − tx`, `py − ty` con 0.05f² como `float` redondeado, y las dos comparaciones tratan distinto al NaN (primera: NaN → «cerca» → desplazamiento 0; segunda: NaN → false).
4. **ProcessPed — ruta de ángulo:** faltaba `CGeneral::LimitRadianAngle` (el borrador comparaba `|cur − aim|` directamente) y el orden de las condiciones (temporizador antes que `animOk`, luego distancia).
5. **GetLightingFromCollision — resultado:** el borrador cacheaba y devolvía `tColLighting{}` (cero) en lugar de `cp.m_nLightingB`, y pasaba `ProcessVerticalLine` con argumentos por defecto.
6. **GetLightingFromCollision — ABI (hallado por el banco, fallo `_RTC_CheckEsp` en `Gate`):** MSVC devuelve `tColLighting` (tiene constructor) por **puntero oculto** con `ret 4`, mientras el EXE devuelve el byte en AL con `ret`: el hook apunta a `GetLightingFromCollisionExeAbi`, que devuelve `uint8`. Los llamadores de la DLL (`Train.cpp`) no cambian.
7. **IsSpottedEntity (regla a respetar):** el EXE normaliza el AL del colaborador a 1/0 (`mov al,1` / `xor al,al`); la DLL compara `!= 0` en lugar de devolver el byte tal cual (el banco incluye AL = 2, 0x80, 0xFF y AL = 0 con bytes altos ≠ 0).
8. **Banco (no de la función):** las banderas «puntero local» de los falsos usaban la dirección de un evento global en vez de una variable de pila (sólo el oráculo lo detectó, EXE y DLL daban lo mismo); `sizeof(CPed)` es 0x79C (no 0x7A4) y las sumas de arena de entidad/matriz estaban mal (comprobado con `static_assert` y `fixture_mirror.py`).

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con **sólo** ese hook encendido (los demás hooks conservan su estado y los colaboradores se sustituyen). Los falsos se instalan como parche de entrada en la dirección del EXE y en el cuerpo de la DLL
(resuelto antes de parchear, siguiendo saltos sólo dentro del ASI; los virtuales, con `ReversibleHook::Virtual::GetHookOurAddress()`), y registran sus argumentos **en orden**.
Observado por lado: eventos de los falsos con argumentos, cambios byte a byte de todas las arenas con guardas (objeto, entidad, matriz, tarea, ped, inteligencia, tarea primaria falsa), `AL` de salida,
ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado (FCW completa, TOP, etiquetas, ST válidos; MXCSR sin banderas), ruta (la entrada salta a la DLL sólo en el candidato), `collaborators_dll`, entradas de los colaboradores intactas tras quitar los parches, `CTimer::m_snTimeInMilliseconds` restaurado.
EAX/ECX/EDX de salida **sólo se registran** (ningún llamador del EXE los lee; el AL de `bool`/`tColLighting` sí se compara).

## Fixtures: 4 x 4096 = 16 familias x 256 por función (`Fixtures.hpp`, mismo generador en el juego y en `fixture_mirror.py`)

- **IntegrityCheck:** con/sin vector de tamaños comunes, 0–6 nodos libres, 0–10 nodos de tamaños comunes repartidos en las 64 entradas, 0–8 bloques de 1 o 2 páginas (también cero), `inUse` ∈ {0, 1, 2, 0x80, 0xFF}, `memId` −1/0..19 en uso y basura en los libres, cadena `m_PrevBlock` nula, por los bloques o por nodos fantasma, `this` en desplazamiento aleatorio.
- **IsSpottedEntity:** índice de script con bordes (0, −1, 0x80000000, 0x10000…), el 0x4839A0 falso devuelve negativos/positivos/0, el 0x493280 falso devuelve AL = 0, 1, no canónico y AL = 0 con bytes altos ≠ 0; con/sin matriz (la posición no usada es un señuelo); posiciones con ordinarios, bordes, denormales, ±inf, qNaN y sNaN (se copian sin aritmética).
- **GetLightingFromCollision:** coordenadas int16 en bordes (±32768/32767, 0, ±1…), `found` ≠ 0 (varios valores), colisión sí/no con bytes altos basura y AL no canónico, iluminación 0x48/0/aleatoria.
- **ProcessPed:** los cuatro modos primera-vez × animación; ocho regímenes numéricos (cerca, borde de 0.05, lejos, coordenadas grandes, velocidad denormal/cero/enorme, posiciones diminutas, matriz con denormales/ceros/enormes, coordenadas enormes); como mucho un NaN con carga propia por caso (q o s) **sin** infinitos, o infinitos sin NaN; temporizador −1/vencido/igual/futuro/con envoltura; `now` en bordes; AL de la animación no canónico; jugador/no jugador con la inteligencia **inválida** cuando no se usa (el EXE y la DLL no deben tocarla); `LimitRadianAngle` falso con bordes de 0.1f, NaN, ±inf, denormales; sin matriz sólo en rutas sin deslizamiento (el EXE desreferencia la matriz al deslizar).

`fixture_oracle.py` modela las cuatro funciones desde el desensamblado (x87 de 24 bits con exponente extendido, reglas de NaN/indefinido, `fcom` ordenada) y lleva autoprueba (`python -I fixture_oracle.py` → `ok`). Los mínimos de cobertura de `run_local.py` están calibrados con
`fixture_mirror.py` (~50 % de lo medido).

## Suposiciones y riesgos

- **Arenas** (suma a mano, comprobadas por `fixture_mirror.py` y por `static_assert`/medición en `LiveComparison.hpp`): entidad 0x40 + 0x38 + 0x40 = 0xB8; matriz 0x40 + 0x48 + 0x40 = 0xC8; nodo 0x40 + 0x0A + 0x36 = 0x80; tarea 0x40 + 0x80 + 0x40 = 0x100; ped 0x40 + 0x79C + 0x40 = 0x81C; inteligencia 0x40 + 0x40 + 0x40 = 0xC0; tarea primaria 0x40 + 0x20 + 0x40 = 0xA0; montón 48 páginas de 0x1000.
- La tarea local `CTaskSimpleStandStill`, `CPed::IsPlayer` y la llamada virtual `MakeAbortable` no se sustituyen por parche: `IsPlayer` lee sólo `[ped+598h]`; la vtable de la tarea primaria es falsa y traza la ranura 6 (las demás son trampas que registran «unexpected»). El constructor y destructor de `CTaskSimpleStandStill` son código real en ambos lados: el falso de `ProcessPed` comprueba que los campos definidos (+4, +8, +0xC, +0x10, +0x14, +0x15, +0x18, +0x19, +0x1C) son iguales.
- Los 2 bytes de relleno de `CTaskTimer` (+0x16/+0x17) no se comparan (basura de pila distinta en cada lado).
- Se escribe temporalmente `CTimer::m_snTimeInMilliseconds` (restaurado tras cada lado y comprobado al final).
- `PAGE_GUARD` registra sólo la **primera** lectura de cada página; los accesos repetidos a la misma página y los campos concretos leídos dentro de ella los cubre el oráculo sólo a través de las rutas que cambian el orden o el resultado.
- El hook de `ProcessPed` es virtual: sustituye la ranura 7 de la vtable 0x86FFEC y la entrada 0x66C4E0.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `Cases.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
