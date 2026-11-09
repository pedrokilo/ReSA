<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9609 funciones reconstruidas; 9609 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn90B: ocho funciones pequeñas sin FPU de las que ya llama código reemplazado (lote piloto 90B)

Estado: **APROBADO dentro del juego** (04/11/2026, worktree `wl-1` sobre main 6e80ef26d + los cambios del lote). 32768 pares (4096 por función), cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauración/x87 en ambos lados y salida de CADA lado igual a la del oráculo independiente (`fixture_oracle.py`, integrado en `run_local.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn90b/20261104-012051-971908` (archivada en `reversing/CppTwelveLevel6Final/runs/`).
Lote piloto de la categoría 2 («pequeñas sin FPU», tipo `pequena`, `fpu=0`, `indirectas=0`) del grupo A de `docs/JuegoExe/grupoA.csv`: sirve para medir el coste real (ver «Coste medido»).

| Función | Dirección | Hook | Fuente |
|---|---|---|---|
| `CNodeAddress::operator==` | 0x420920 | `CNodeAddress` / `operator==` / S | `NodeAddress.cpp` (antes `= default` en el `.h`, en línea) |
| `CNodeAddress::operator!=` | 0x420950 | `CNodeAddress` / `operator!=` / S | `NodeAddress.cpp` |
| `CNodeAddress::operator<` | 0x420980 | `CNodeAddress` / `operator<` / S | `NodeAddress.cpp` (no existía en la DLL) |
| `CWorld::AdvanceCurrentScanCode` | 0x4072E0 | `CWorld` / `AdvanceCurrentScanCode` / S | `World.cpp` (antes `inline` al final de `World.h`) |
| `CTaskSimpleScratchHead::CTaskSimpleScratchHead` | 0x48DF30 | `CTaskSimpleScratchHead` / `Constructor` / S | `TaskSimpleScratchHead.h/.cpp` |
| `CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading(float,float,float)` | 0x667E20 | `CTaskSimpleAchieveHeading` / `Constructor` / S | `TaskSimpleAchieveHeading.h/.cpp` (el cuerpo ya existía; solo el hook) |
| `CColModel::CColModel` | 0x40FB60 | `CColModel` / `Constructor` / S | `ColModel.h/.cpp` |
| `CEventDeath::CEventDeath(bool)` | 0x4ADDF0 | `CEventDeath` / `Constructor` / S | `EventDeath.h/.cpp` (se activa `CEventDeath::InjectHooks()` en `InjectHooksMain.cpp`) |

Filas de `docs/hooks.csv`: `CNodeAddress,operator==,0x00420920,1,0,1,S`, `CNodeAddress,operator!=,0x00420950,1,0,1,S`, `CNodeAddress,operator<,0x00420980,1,0,1,S`, `CWorld,AdvanceCurrentScanCode,0x004072E0,1,0,1,S`, `CTaskSimpleScratchHead,Constructor,0x0048DF30,1,0,1,S`, `CTaskSimpleAchieveHeading,Constructor,0x00667E20,1,0,1,S`, `CColModel,Constructor,0x0040FB60,1,0,1,S`, `CEventDeath,Constructor,0x004ADDF0,1,0,1,S`.

Importante: el CSV del grupo A llama «módulo» a lo que adivinó por proximidad de direcciones; varias de estas «pequeñas» eran en realidad funciones inline o constructores que el código C++ ya reproducía sin dirección propia (el EXE sigue llamando a la dirección original). Hacerlas «reemplazadas» significa darles símbolo propio y engancharlas, no escribir lógica nueva.

## Revisión contra el EXE (desensamblado con `dis.sh`)
- **operator==** (0x420920, `ret 4`, AL): `[this].area == [otra].area && [this].nodo == [otra].nodo` (words). **operator!=** (0x420950): el complemento. **operator<** (0x420980): `jb` / `jne` / `jb`, es decir orden lexicográfico **sin signo** (área y luego nodo). Sin colaboradores.
- **AdvanceCurrentScanCode** (0x4072E0, cdecl sin argumentos): `cmp word [0xB7CD78], 0xFFFF; jae` → `call 0x563470` (ClearScanCodes) y el word queda en 1; si no, `inc word`. El word alto del dword vecino no se toca.
- **CTaskSimpleScratchHead** (0x48DF30): llama a `CTaskSimpleRunAnim` 0x61A900 (que llama a `CTask` 0x61A390) con (0, 0x87, 4.0f, 0x1A5, nombre, 0) y deja su vtable (0x85A100). Devuelve this.
- **CTaskSimpleAchieveHeading** (0x667E20, `ret 0Ch`): `CTask` 0x61A390, `[+0x14] &= 0xFE`, `[+8]=rumbo`, `[+0xC]=ritmo`, `[+0x10]=tolerancia`, vtable 0x86FD74. Devuelve this.
- **CColModel** (0x40FB60): `CBoundingBox` 0x40FAF0 (min = +1.0f, max = -1.0f), `[+0x28]=0`, `[+0x2C]=0`, `[+0x29] = (b & 0xFC) | 4`. **No escribe la esfera de límites (+0x18..+0x27)**: queda lo que hubiera en la memoria. Devuelve this.
- **CEventDeath(bool)** (0x4ADDF0, `ret 4`): `[+4]=0`, `[+8]=0` (byte), `[+0xC]` = byte bajo del argumento, vtable 0x85ADC0, `[+0x10]` = `CTimer::m_snTimeInMilliseconds` (0xB7CB84). Devuelve this.

## Hallazgo del banco (primera ejecución)
CColModel falló 3400/4096 al primer intento: el constructor C++ de la DLL ponía a cero la esfera (`CSphere` tiene inicializadores `{}`), el EXE no. Se arregló dejando `m_boundSphere` dentro de una unión anónima en `ColModel.h` (misma disposición y mismo acceso; el constructor deja de inicializarla). Los otros siete pasaron a la primera.

## Banco
Falso idéntico en la dirección del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco), solo para `AdvanceCurrentScanCode`: `CWorld::ClearScanCodes` 0x563470 (cdecl, sin argumentos; registra el word bajo del código de escaneo en el momento de la llamada).
Los constructores NO sustituyen a sus bases: `CTaskSimpleRunAnim` 0x61A900 / `CTask` 0x61A390 en el EXE son direcciones reales (con hook RH activo hacia la DLL), pero en la DLL el constructor C++ llama a la base en línea, sin dirección que parchear. Corren de verdad en ambos lados y se compara la ventana de 0x40 bytes del objeto y sus guardas (vtable normalizada: el EXE deja la suya; la DLL, la de su clase dentro del ASI, comprobado con `VirtualQuery`).
Estado global del banco: el dword de 0xB7CD78 y el de 0xB7CB84 se fijan por caso, se vigilan tras la llamada y se restauran.
Se comparan AL (comparaciones) o EAX == this (constructores), los eventos del falso, los bytes cambiados de las arenas (objeto y la otra dirección), los dos dwords globales y los hashes; todo contra el oráculo Python (que recibe del banco el contenido inicial de la ventana en la entrada y aplica el modelo del desensamblado).
4096 casos por función, 16 familias de 256: comparaciones (iguales, mismo área/distinto nodo y al revés, alias this == otra, frontera de signo 0x7FFF/0x8000, extremos), código de escaneo (0xFFFF, 0xFFFE, cercanos, 0..2, mitades, aleatorio, con y sin basura en el word alto), rellenos del objeto (ruido, 00, FF, CD, A5, ruido con ceros), floats especiales en los argumentos de AchieveHeading (±0, desnormales, ±inf, NaN con varias cargas, señalizante), bool de CEventDeath con basura en los tres bytes altos, tiempos 0 / 0xFFFFFFFF / frontera. Cobertura de ramas con mínimos (`coverage.json`, 80 % de lo medido).

## Fuera de los fixtures
Bool de `CEventDeath` con un byte bajo distinto de 0/1 (los cuatro llamadores del EXE apilan 0 o 1; la DLL lo normalizaría), valores de `CTimer` coherentes con el tiempo real, objetos reales de pool (el banco usa arenas propias), y las bases (`CTaskSimpleRunAnim`, `CTask`, `CEvent`, `CBoundingBox`), validadas por sus propios bancos. Los objetos creados desde el EXE con estos constructores pasan a llevar la vtable de la clase de la DLL.

## Prueba negativa
Producción mutada en siete puntos (`NodeAddress.cpp`: `==` sin comparar el nodo, `!=` con `>` en el nodo, `<` con `<=` en el nodo; `World.cpp`: el contador queda en 2 en lugar de 1 tras el reinicio; `TaskSimpleScratchHead.h`: 4.0 → 4.5; `TaskSimpleAchieveHeading.cpp`: el ritmo toma la tolerancia; `ColModel.cpp`: `m_IsActive = false`; `EventDeath.cpp`: tiempo + 1): **20360/32768 pares con diferencias** (operator== 960, != 494, < 1329, AdvanceCurrentScanCode 1213, ScratchHead 4096, AchieveHeading 4076, CColModel 4096, CEventDeath 4096), `resultados-fn90b/20261104-011804-045822`. Restauradas desde la copia buena y comprobadas con `cmp` antes de la corrida final.

## Bancos repetidos
Citan alguna de estas direcciones (los tres de nodos y `0x667E20`/`0x40FB60` los usa el código C++ de la DLL de esos bancos); se ejecutaron con el árbol del lote (`wl-1`), un banco a la vez:
| Banco | Cita | Resultado |
|---|---|---|
| Car84J | 0x420920, 0x420980 (comparaciones de nodos, «reales» en el EXE) | APROBADO, `resultados-car84j/20261104-012320-127181` |
| Car84L | 0x420920/0x420950/0x420980 | APROBADO |
| EvHA | 0x667E20 (falso `HeadingCtor` en ambos lados) | APROBADO |
| EvHB | 0x667E20 (falso `HeadingCtor`) | APROBADO |
| PickUpNext68 | 0x667E20 (antes «sin hook: real en ambos lados») | APROBADO, `resultados-pickupnext68/20261104-014600-161545` |
| WalkRoundFirst31 | 0x667E20 (solo vigilada) | **FALLO, pero también falla en `origin/main` limpio** (baseline en un worktree aparte, `resultados-walkroundfirst31/20261104-024834-636506`): acceso inválido en `CVector::operator=` desde `CTaskComplexFollowPointRoute::CTaskComplexFollowPointRoute` (DLL), anterior al lote |
| FileLoader47 | 0x40FB60 (su comentario ya anotaba que la DLL ponía a cero la esfera de `CColModel`; con la unión anónima ya no) | **FALLO** en `CFileLoader::LoadTexDictionary` 58/128 (secuencia de eventos de los falsos: el EXE registra un evento 77 donde la DLL repite el 72); no toca ninguna función del lote. **Baseline sobre `origin/main` limpio pendiente**: el candado de compilación estuvo ocupado por otro agente más de 30 min; el worktree limpio `D:/Original/wl-1b` (compilado) queda listo: `python FileLoader47/build_probe.py` y `python FileLoader47/run_local.py` desde `wl-1b/reversing` |
Ningún banco antiguo reescribió su `validation-summary.json`. `ShadowsWave3` solo menciona `AdvanceCurrentScanCode` en un comentario (fija el código lejos de 0xFFFF) y no se repitió.

## Coste medido (hora de pared, 04/11/2026)
| Fase | Tiempo |
|---|---|
| Lectura de docs/CSV, filtro del grupo A, desensamblado de ~25 candidatas y elección de las 8 | ~11 min (00:51-01:02) |
| C++ de las 8 (hooks, envolturas `Constructor`, mover `AdvanceCurrentScanCode` y los operadores fuera del `.h`) | ~5 min (incluido arriba) |
| Primera compilación completa del worktree | 5 min |
| Banco Fn90B (Fixtures, Core copiado de Fn85A, LiveComparison, oráculo, run_local, build_probe, register_hook) | ~12 min de escritura |
| Primera sonda + corrida (11 s + 83 s): 7/8 aprobadas a la primera, CColModel 3400/4096 | 2 min |
| Arreglo de la esfera + recompilación (114 s: la cabecera `ColModel.h` recompila mucho) + corrida (83 s) → APROBADO | ~4 min |
| Prueba negativa (7 mutaciones, sonda + corrida) y restauración | ~5 min |
| Corrida final + registro + `hooks.csv` + auditoría + README | ~8 min |
| **Total hasta aprobado y registrado** | **~33 min, ~4 min por función** |
| Repetición de 7 bancos antiguos | 67 min de pared, casi todo cola del candado de compilación (FileLoader47 esperó ~34 min); trabajo real: sondas 11-343 s y corridas 20-236 s por banco |
| Triaje de fallos previos (worktree limpio: compilación completa 302 s + sonda + corrida) | ~15 min + colas |
Lo fácil: con la plantilla Fn85A el banco es mecánico (el único hueco real fue el modelo de memoria de los constructores: ventana del objeto en la entrada del oráculo y vtable normalizada). Lo difícil: (1) el CSV del grupo A mezcla inline, constructores y funciones que el C++ ya reproduce sin dirección propia: de las ~175 candidatas `pequena`/`fpu=0`/`indirectas=0` más de la mitad eran plantillas de lista, ya instaladas en `Matrix.cpp` pero apagadas, o dependían de una función que usa RNG o de ayudantes sin símbolo; hubo que descartar y reemplazar candidatas leyendo el EXE; (2) los inicializadores `{}` de miembros (`CSphere`) hacen que un constructor C++ escriba más que el del EXE; (3) la cola de los candados compartidos domina el tiempo de reloj cuando hay varios agentes.
