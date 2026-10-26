<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WaterLevel62: FillQuadsAndTrianglesList, GetWaterDepth, RenderHighDetailWaterRectangle, RenderHighDetailWaterTriangle y TestLineAgainstWater

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\we-4`, HEAD 0ac292a76 + fuentes del lote 62). 20480 pares (5 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados y salida de cada lado igual a la del oráculo independiente. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-waterlevel62\20261020-004948-483613`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CWaterLevel::FillQuadsAndTrianglesList` | 0x6E7B30 | Simple (`RH_ScopedGlobalInstall`) | 4096 | `AddToQuadsAndTrianglesList` 0x6E5750 (sin reconstruir; la DLL la llama por dirección) |
| `CWaterLevel::GetWaterDepth` | 0x6EA960 | Simple | 4096 | `GetWaterLevelNoWaves` 0x6E8580 (con hook), `GetGroundLevel` 0x6EA8A0 (sin reconstruir) |
| `CWaterLevel::RenderHighDetailWaterRectangle` | 0x6EB810 | Simple | 4096 | `CCamera::IsSphereVisible` 0x420C40 (con hook), `SetUpWaterFog` 0x6EA9F0 (con hook), `RenderHighDetailWaterRectangle_OneLayer` 0x6E91D0 (sin reconstruir), `SplitWaterRectangleAlongXLine` 0x6E73A0 (con hook), `RenderWaterRectangle` 0x6EC5D0 (con hook) |
| `CWaterLevel::RenderHighDetailWaterTriangle` | 0x6EDDC0 | Simple | 4096 | `IsSphereVisible` 0x420C40, `RenderHighDetailWaterTriangle_OneLayer` 0x6E8780 (sin reconstruir), `SplitWaterTriangleAlongXLine` 0x6ECF00 (con hook) |
| `CWaterLevel::TestLineAgainstWater` | 0x6E61B0 | Simple | 4096 | ninguno |

```
bash reversing/_handoff/bench_here.sh WaterLevel62                                      # compila la sonda y corre el banco (candado global del juego)
# o a mano, siempre en serie:
python -B reversing/WaterLevel62/build_probe.py
python -B reversing/WaterLevel62/run_local.py
python -B reversing/WaterLevel62/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/WaterLevel62/fixture_oracle.py                                              # «ok» (autoprueba del oráculo)
python -I reversing/WaterLevel62/fixture_mirror.py [--evidence <carpetaEvidencia>/comparisons.jsonl]   # compila Fixtures.hpp con cl: arenas/constantes C++ == oráculo, cobertura >= mínimos y (con --evidence) filas input del juego == espejo
python -B reversing/WaterLevel62/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-waterlevel62/20261020-004948-483613"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador): `CWaterLevel,FillQuadsAndTrianglesList,0x006E7B30,1,0,1,S`, `CWaterLevel,GetWaterDepth,0x006EA960,1,0,1,S`,
`CWaterLevel,RenderHighDetailWaterRectangle,0x006EB810,1,0,1,S`, `CWaterLevel,RenderHighDetailWaterTriangle,0x006EDDC0,1,0,1,S`, `CWaterLevel,TestLineAgainstWater,0x006E61B0,1,0,1,S`.
`SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque en el clon: `waterlevel62-runtime.start`; salida `waterlevel62-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas cinco funciones respecto a HEAD; `git apply --check` contra los blobs de HEAD correcto)

Ningún borrador del lote 27 afectaba a estas funciones (`git diff HEAD -- source/game_sa/WaterLevel.cpp` en el árbol principal estaba vacío): las cinco eran envolturas `plugin::Call*` hacia el EXE.

- `WaterLevel.cpp`: las cinco funciones en C++ y sus cinco líneas `RH_ScopedGlobalInstall` en `CWaterLevel::InjectHooks` (sin cambios en `WaterLevel.h`).
  `GetWaterDepth` sigue llamando por dirección a `GetGroundLevel` (0x6EA8A0), `FillQuadsAndTrianglesList` a `AddToQuadsAndTrianglesList` (0x6E5750) y las dos de render a `..._OneLayer` (0x6E91D0 / 0x6E8780),
  que no están reconstruidas (colaboradores, no la función reconstruida); el resto de colaboradores (`GetWaterLevelNoWaves`, `IsSphereVisible`, `SetUpWaterFog`, `SplitWater*AlongXLine`, `RenderWaterRectangle`, `lerp`) son C++.
- `X87Math.cpp/.h` (bloque «Lote 62» al final, prefijo `Water62`; el archivo se compila `/O2 /Oi /arch:IA32 /fp:fast`): `Water62DepthStoreCpp`, `Water62RectSphereCpp`, `Water62TriSphereCpp`, `Water62SplitRatioCpp` y
  `Water62TestLineCpp` (con sus ayudantes estáticos `Water62Block`, `Water62Cross`, `Water62InQuad`, `Water62FtolCpp`, `Water62RootCpp`). Aritmética x87 de 24 bits con exponente extendido: lo que el EXE deja en registro es `double`
  (cada operación redondea la mantisa a 24 bits), lo que guarda con `fstp dword` es `float`/`volatile float`.

## Fase 2: revisión contra el EXE (re-desensamblado y contrastado instrucción a instrucción tras escribir)

**FillQuadsAndTrianglesList (0x6E7B30..0x6E7D3F).** 12 x 12 bloques (a externo = x, b interno = y) de 500 unidades: `xLo = 500a - 3000`, `xHi = xLo + 500` (floats exactos, `fild; fsub 3000.0f`).
Cuadriláteros (10 bytes desde 0xC21C90): `NumWaterQuads` (0xC22888, con signo) se prueba antes de entrar (`jle`) y **se vuelve a leer tras cada índice** (0x6E7C1A, `jl`); condición `v1.x > xLo && v0.x < xHi && v2.y > yLo && v0.y < yHi`
(`fcomp` ordenadas; vértices de 20 bytes desde 0xC22910 con el índice de 16 bits extendido con signo) -> `0x6E5750(a, b, i, 1)` cdecl. Triángulos (8 bytes desde 0xC22854): `NumWaterTriangles` (0xC22884) se lee **una sola vez por bloque**
(0x6E7C2B, tras los cuadriláteros); `v1.x > xLo && v0.x < xHi && max(v0.y, v2.y) > yLo && min(v0.y, v2.y) < yHi` (mín/máx como int16) -> `0x6E5750(a, b, i, 2)`. Todo son enteros pequeños: la x87 no redondea nada y la DLL usa enteros.
Nadie lee EAX/ECX/EDX de salida (único llamador: 0x6EB5EF, seguido de `call 0x7316A0` -> 0x7F3A90, que escribe ECX antes de leerlo).

**GetWaterDepth (0x6EA960..0x6EA9E3).** `GetWaterLevelNoWaves(x, y, z, &nivel, 0, 0)` donde `&nivel` es **la propia ranura de pila del primer argumento** (que contenía el puntero `vecPos`): si el colaborador devuelve true sin escribir, el nivel son los bits
del puntero (la DLL lo imita con `levelSlot = &vecPos`). AL == 0 -> return 0. `GetGroundLevel(&vecPos, &suelo, 0, 30.0f)`; AL == 0 -> suelo = -100.0f (0x859014). Escrituras en orden: profundidad (`fld nivel; fsub st,st(1); fstp`), nivel
(copia de 32 bits sin pasar por la x87) y suelo (`fld; fstp`: un SNaN sale silenciado); si dos punteros coinciden gana la última. Único llamador (0x6E505D): sólo `test al,al`.

**RenderHighDetailWaterRectangle (0x6EB810..0x6EBEBA).** Centro `(fild(minX + maxX) * 0.5, fild(Y1 + Y2) * 0.5, P1.z)` y radio `sqrt(hy*hy + hx*hx)` con `hx = fild(maxX - minX) * 0.5`, `hy = fild(Y1 - Y2) * 0.5` (24 bits por operación, sumas/restas de enteros con
envoltura); `0x420C40(centro, radio, 0xB6FA74)` (ECX = 0xB6F028) y, **si el byte 0xB6F998 es distinto de cero** (cualquier valor) y el primero dio AL == 0, `0x420C40(centro, radio, 0xB6FABC)`; sigue en AL == 0 -> return. Es exactamente
`CCamera::IsSphereVisible(origen, radio)` (0x420D40, inlinada), que la DLL llama. `minY/maxY` por orden de `Y1/Y2`; `hx = (maxX-minX)/2`, `hy = (maxY-minY)/2` (división truncada), `twice = 2*hy*hx`, `tri = (hy+1)*(hx+1)` (envoltura de 32 bits);
`3*twice < 0x1000 && tri < 0x800` (con signo) -> `SetUpWaterFog(minX, minY, maxX, maxY)` y `0x6E91D0(minX, maxX, Y1, Y2, P1..P4, capa, twice, tri, hx, hy)` para capa 0 y 1; si no `hx > hy` -> `SplitWaterRectangleAlongXLine(minX + (hx/2)*2, ...)`; si no
la partición en Y va en línea: `t = fild(splitY - Y1) / fidiv(Y2 - Y1)` redondeado a float con **un solo redondeo** (por eso `Water62SplitRatioCpp` y no `SplitWaterRectangleAlongYLine`, cuyo `float(a)/float(b)` en SSE redondea antes los enteros > 2^24),
dos `lerp` (0x6E5280) por mitad (se repiten, son puros) y dos `RenderWaterRectangle`. Los CRenPar viajan por valor con sus 16 bytes (el relleno no se compara). Único llamador: dentro de 0x6EC5D0, sin leer registros.

**RenderHighDetailWaterTriangle (0x6EDDC0..0x6EE07F).** Centro `(fild(X1+X2+X3) * 0.33333331f, fild(Y1+Y2+Y3) * 0.33333331f, P1.z)` (0x8594EC = 0x3EAAAAAA) y radio `fild(X2 - X1) * 0.71f` (0x872168; **puede ser negativo**); mismas dos llamadas a 0x420C40.
`hw = (X2-X1)/2`, `sq = hw*hw`, `T = 1 + 2 + .. + (hw+1)` (bucle de suma con envoltura; 0 si `hw+1 < 1`; la DLL usa la fórmula cerrada en 64 bits, idéntica módulo 2^32); `3*sq < 0x1000 && T < 0x800` -> `0x6E8780(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, capa, sq, T, hw)`
para capa 0 y 1; si no `SplitWaterTriangleAlongXLine(X1 + (hw/2)*2, ...)`. No llama a `SetUpWaterFog`.

**TestLineAgainstWater (0x6E61B0..0x6E6793).** `zMin/yMin/xMin = (from < to) ? from : to`, `zMax/yMax/xMax = (from > to) ? from : to` (NaN -> `to`); índices de bloque = 32 bits bajos del `_ftol2` de `v * 0.002f + 6.0f` (24 bits; NaN/inf/fuera de rango -> 0), bucle `ix = bloque(xMin)..bloque(xMax)`, `iy = bloque(yMin)..bloque(yMax)`,
`row = ix*12` con envoltura. Dentro de la cuadrícula (`0 <= row < 144 && 0 <= iy < 12`): palabra de `m_BlockPolyInfo[row+iy]`, tipo = 2 bits altos: **1** cuadrilátero único, **3** combinación (secuencia de `m_PolyCombos` hasta una entrada de tipo 0, sólo se prueban las de tipo 1),
**0 y 2 (triángulo único) no se prueban nunca**. Cuadrilátero: `from.z * to.z < 0` y el cruce con z = 0 (`dx = to.x - from.x`, `dy`, `dz` float, `t = float(|from.z| / (zMax - zMin))`, `X = float(float(dx*t) + from.x)`, `Y = float(dy*t + from.y)`, `Z = float(float(dz)*t + from.z)`) que
**se escribe SIEMPRE en `*intersection`**, acierte o no; devuelve true si `x0 <= X <= x1 && y0 <= Y <= y2` (vértices 0, 1, 2). Fuera de la cuadrícula: `zMin < 0 && zMax > 0`, cruce, y true si `bloque(X) == ix && bloque(Y) == iy`. Callers (0x740EEB, 0x740F36): sólo `test al,al`.

### Discrepancias halladas y corregidas
1. **Ninguna en la fuente**: no había borrador (envolturas). La transcripción se contrastó con un oráculo escrito aparte desde el desensamblado (ver abajo) y con el EXE en 20480 pares; el primer pase completo ya dio cero diferencias.
2. Detalles del EXE que un C++ «natural» habría perdido y que el banco vigila: el contador de cuadriláteros se relee en cada índice mientras que el de triángulos no; TestLineAgainstWater escribe `*intersection` en cada candidato aunque no acierte y no prueba los triángulos únicos;
   la partición en Y de RenderHighDetailWaterRectangle redondea `t` una sola vez; el nivel de GetWaterDepth puede ser el puntero `vecPos`; el radio del triángulo puede ser negativo; el espejo se activa con cualquier byte distinto de cero.
3. **Banco (no de la función)**: `small` es una macro de Windows (rompía `struct Ren`); `u64` no existía en el espacio de nombres de las fixtures.

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con **sólo** ese hook encendido (los demás hooks conservan su estado y los colaboradores se sustituyen). Los falsos se instalan como parche de entrada en la dirección del EXE y en el cuerpo de la DLL
(resuelto antes de parchear, siguiendo saltos sólo dentro del ASI), y registran sus argumentos **en orden** (para los de render: enteros y los CRenPar completos: z, bigWaves, smallWaves, flowX, flowY).
Observado por lado: eventos de los falsos con argumentos, resultados (AL), salidas bit a bit, arenas con guardas, atlas de agua sin cambios, contadores tras los falsos, ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado (FCW completa, TOP, etiquetas, ST válidos; MXCSR sin banderas),
ruta (la entrada salta a la DLL sólo en el candidato), `collaborators_dll`, entradas de los colaboradores intactas tras quitar los parches, y restauración del atlas, de los contadores, de la ventana del vértice -1 y del byte del espejo.
EAX/ECX/EDX de salida **sólo se registran** (ningún llamador del EXE los lee; el AL de `bool` sí se compara).

Las funciones de render **no se prueban contra una GPU**: sus colaboradores (los que escriben vértices y llaman a RenderWare) se sustituyen por falsos y se comparan las llamadas con sus argumentos exactos y en orden; los `..._OneLayer` no están reconstruidos y no forman parte de este banco.

## Oráculo, espejo y control negativo

- `fixture_oracle.py`: modelo independiente de las cinco funciones, escrito desde el desensamblado, con un x87 de 24 bits exacto (fracciones, NaN, infinitos, ceros con signo, `_ftol2`, `fstp dword` con desbordamiento/denormales). `run_local.py` exige que la salida de CADA lado (EXE y DLL) sea igual a la suya
  (también los eventos de los falsos y las ramas de GetWaterDepth con el nivel sin escribir, usando el puntero real del objeto que registra el banco) y que la cobertura de las fixtures alcance los mínimos (~50 % de lo medido por `fixture_mirror.py`).
- `fixture_mirror.py`: compila `Fixtures.hpp` con `cl` fuera del juego; comprueba las constantes de arena y de tamaño de los arreglos contra las sumas a mano del oráculo, la cobertura contra `run_local.MINIMUMS` y, con `--evidence`, que las filas `input` registradas en el juego son idénticas a las del espejo (verificado: 5 x 4096).
- Control negativo (se aplicó y se revirtió; los fuentes de la corrida aprobada son los del parche): cuatro mutantes en la DLL (`v1.x >= X` -> `>` en la prueba de caja, `hx > hy` -> `>=` en el rectángulo, `yMax > yLo` -> `>=` en FillQuads, `T < 0x800` -> `<=` en el triángulo) -> el banco dio FALLO en
  FillQuadsAndTrianglesList (443/4096), RenderHighDetailWaterRectangle (139/4096) y TestLineAgainstWater (40/4096). El cuarto es equivalente (T = 0x800 exacto no existe: T = (hw+1)(hw+2)/2 salta de 2016 a 2080) y no se detecta con ninguna entrada.

## Fixtures: 5 x 4096 = 16 familias x 256 por función (`Fixtures.hpp`, mismo generador en el juego y en `fixture_mirror.py`)

- **FillQuadsAndTrianglesList:** atlas sintético escrito en las tablas reales (guardadas y restauradas en cada caso): 6–45 vértices con coordenadas aleatorias, junto a los bordes de bloque (±2) y extremos de int16; 0–24 cuadriláteros y 0–6 triángulos con índices repetidos; contadores en memoria 0 / negativo / el real;
  vértice -1 (índice 0xFFFF) con su ventana; el falso cambia `NumWaterQuads` o `NumWaterTriangles` en la llamada k (0, negativo, la mitad, el total) para vigilar la relectura/lectura única.
- **GetWaterDepth:** AL de ambos colaboradores 0/no canónico (con bytes altos), nivel escrito o no, suelo con/sin éxito (-100.0f), nivel y suelo ordinarios, bordes, denormales, enormes, infinitos (inf - inf), un NaN (q/s), ceros con signo; tres punteros de salida nulos/distintos/alias entre sí y dentro de `vecPos`.
- **RenderHighDetailWaterRectangle/Triangle:** rutas pequeña / umbrales (`6hxhy` = 0x1000, `T` = 0x800) / parte en X / parte en Y / ancho negativo o nulo / extremos de 32 bits (envoltura) / invisible; byte del espejo 0, 1, 2, 0x80, 0xFF; AL de la esfera 0/no canónico; CRenPar con NaN/inf/denormales (en la ruta Y, sólo rango ordinario porque el SSE del juego vacía los denormales del `lerp`).
- **TestLineAgainstWater:** líneas que cruzan z = 0 dentro y fuera del mundo, sin cruce, z igual o cero, NaN/inf/enormes (bloque 0), cruce exacto en el medio con bordes de caja iguales, cuadriláteros únicos y combinaciones (con triángulos y entradas de tipo 3 que se saltan, terminador de tipo 0 con id), vértice -1, y `ix` lejano (j·2^30) cuyo `row` envuelve a la fila 0.

## Suposiciones y riesgos

- `lerp` (0x6E5280) no se sustituye: la DLL usa la de `WaterLevel.h` (SSE) y el EXE la suya (x87); coinciden mientras no haya denormales (el juego corre con MXCSR FTZ): las fixtures de la ruta Y no los usan. Esa diferencia ya existía en `RenderWaterRectangle`.
- `GetWaterDepth` con `GetGroundLevel` devolviendo true sin escribir el suelo leería basura de pila (en el EXE y en la DLL): no se prueba. Un `memId`/índice de vértice fuera del arreglo (distinto de -1) leería memoria ajena: no se prueba.
- El bucle `1 + 2 + .. + (hw+1)` del EXE es O(hw): las fixtures con |hw| > 10^6 se dejan invisibles (la DLL usa la fórmula cerrada).
- Los hooks nuevos cambian la ruta de los llamadores en C++ (`RenderWaterRectangle`, `RenderWaterTriangle`, `RenderWater`, etc.): sus bancos históricos siguen válidos porque el comportamiento es idéntico, pero conviene re-correrlos en la regresión final.
