<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PedLeaf69: cuatro hojas sin llamadores (lote 69)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 9a8eb46fa + `source.patch`). 16384 pares (4 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados, `fp_difference` vacío, cobertura por familia (16 x 256 por función) y de ramas (`coverage.json`, medida con `fixture_selfcheck.py`), y el oráculo (`fixture_oracle.py`) coincide con el lado EXE de los 16384 pares.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pedleaf69\20261026-153110-059629`. Plantilla: `reversing/Helpers66A`; contrato: `reversing/_handoff/BRIEF.md`. Los cuatro cuerpos salen de un borrador sin validar del «lote 27» (parche `D:\Original\lote69_A.patch`), contrastado línea a línea con el desensamblado.

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `CPedIntelligence::IsInHearingRange(const CVector&)` | 0x600C00 | `CPedIntelligence` / `IsInHearingRange` / S | ninguno (helper `PedHearingRangeCpp` de `X87Math.cpp`) |
| `CPedIntelligence::HasInterestingEntites()` | 0x602080 | `CPedIntelligence` / `HasInterestingEntites` / S | ninguno |
| `CPedIntelligence::RemoveAllInterestingEntities()` | 0x602320 | `CPedIntelligence` / `RemoveAllInterestingEntities` / S | `CEntity::CleanUpOldReference(CEntity**)` 0x571A00 (con hook) |
| `CPedStats::FindIndexWithPedStat(void*)` (de `void` a `int32`) | 0x608940 | `CPedStats` / `FindIndexWithPedStat` / S | ninguno |

Filas de `docs/hooks.csv` (las añade el coordinador; todas tipo S): `CPedIntelligence,IsInHearingRange,0x00600C00,1,0,1,S`, `CPedIntelligence,HasInterestingEntites,0x00602080,1,0,1,S`,
`CPedIntelligence,RemoveAllInterestingEntities,0x00602320,1,0,1,S`, `CPedStats,FindIndexWithPedStat,0x00608940,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh PedLeaf69        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/PedLeaf69/build_probe.py
python -B reversing/PedLeaf69/run_local.py
python -B reversing/PedLeaf69/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PedLeaf69/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PedLeaf69/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual) con cl (necesita vswhere en PATH), comprueba las constantes de arena y calcula la cobertura prevista (imprime «ok»)
python -B reversing/PedLeaf69/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pedleaf69/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `pedleaf69-runtime.start`; salida `pedleaf69-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas funciones respecto a HEAD)

- `PedIntelligence.cpp`: `IsInHearingRange`, `HasInterestingEntites` y `RemoveAllInterestingEntities` en C++ + tres `RH_ScopedInstall`. `IsInHearingRange` llama a `PedHearingRangeCpp` (X87Math, bloque «Lote 69»).
- `X87Math.cpp/.h`: `PedHearingRangeCpp(posn, ped, range)`: las tres restas, `rango*rango` y `(dz² + dx²) + dy²` en x87 a 24 bits con exponente extendido, `limite > suma` ordenado (falso con NaN). Comprobado en el ensamblador generado (`fld/fsub` x3, `fmul`, `faddp` en el orden z, x, y, `fcompp`).
- `PedStats.cpp/.h`: `FindIndexWithPedStat` pasa de `void` a `int32` (el EXE devuelve el índice en EAX) + `RH_ScopedInstall`.

## Revisión contra el EXE (re-desensamblado)

Ninguna de las cuatro tiene llamadores en el EXE (se buscaron `call`/`jmp` rel32 y referencias absolutas a las cuatro direcciones en `.text`: cero), así que sólo se compara el valor de retorno.

**0x600C00 (IsInHearingRange).** `ret 4`. `ped = [this]`; posición = `[ped+0x14] ? [ped+0x14]+0x30 : ped+4`; `d = posn - pos` (tres `fsub`); `limite = [this+0xBC]²`; suma `(dz² + dx²) + dy²` sin guardar; `fcompp` + `test ah,41h`: verdadero sólo si `limite > suma` (empate y NaN dan falso). `mov eax,1` / `xor eax,eax`.
**0x602080 (HasInterestingEntites).** Recorre las tres palabras desde `this+0x288` (`cmp dword,0`, palabra completa); AL = 1 si alguna no es nula, si no AL = 0 (los bits altos de EAX quedan con el contador).
**0x602320 (RemoveAllInterestingEntities).** Para cada una de las tres ranuras: si no es nula, `ecx = entidad; push &ranura; call 0x571A00` (`CleanUpOldReference`); después la ranura se pone a cero SIEMPRE (también si era nula).
**0x608940 (FindIndexWithPedStat).** cdecl; `ecx = [0xC0BBEC]` (puntero `ms_apPedStats`), compara el argumento con `ecx + 0x34*i` para i = 0..42 (`cmp edx,ecx`, palabra completa); EAX = i al coincidir; si ninguna coincide, `xor eax,eax`. Sin lectura de la memoria apuntada.

## Hallazgos

1. Borrador del lote 27, `IsInHearingRange`: aritmética en `float` SSE (`m_fHearingRange * m_fHearingRange > dz * dz + dx * dx + dy * dy` en el código del juego, con MXCSR FTZ). Los cuadrados desbordan float y los subnormales se vacían, mientras que la x87 del EXE (24 bits, exponente extendido) no: pasa a `PedHearingRangeCpp`. El comentario «Como el original…» del borrador se sustituyó por el detalle del desensamblado.
2. `HasInterestingEntites`, `RemoveAllInterestingEntities` y `FindIndexWithPedStat` del borrador: sin discrepancias respecto al desensamblado (el comentario «Como el original…» de `FindIndexWithPedStat` es fiel).
3. Primera compilación del banco: la suma a mano de la arena de `CPedStat` estaba mal (44 · 0x34 = 0x8F0, no 0x8E8); lo detectó el `static_assert` de `Fixtures.hpp` y la autoprueba (defecto del banco, no de producción). Sin defectos de producción.

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con SOLO ese hook encendido. `CleanUpOldReference` (0x571A00, con hook) se sustituye por el mismo falso en la entrada del EXE y en el cuerpo de la DLL, resuelto ANTES de parchear; registra (entidad, dirección de la ranura, valor de la ranura en ese instante). Se compara por caso toda la memoria que cambia en las arenas (guardas de 0x40), la traza de eventos, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, `collaborators_dll`, un FNV de la región global tocada (`ms_apPedStats`) y el estado restaurado tras quitar los parches. De la salida sólo se exige AL (las dos bool) y EAX completo (`FindIndexWithPedStat`).

- **0 IsInHearingRange**: ped con o sin matriz (la posición no usada es ruido), posición y rango dentro / en la frontera (ordenación de la suma: 63 casos donde otra ordenación daría otro resultado) / exactos (25² = 625, empate) / lejos / enormes (los cuadrados desbordan float) / NaN / infinitos / ceros y subnormales / rango negativo, NaN o infinito.
- **1 HasInterestingEntites**: ocho patrones de ranuras no nulas (entidades de la arena o valores brutos, incluidos con bytes bajos nulos: 0x100, 0x01000000, 0x80000000…), las palabras vecinas (+0x284 y +0x294) a cero, ruido o no nulas.
- **2 RemoveAllInterestingEntities**: los mismos patrones; eventos del falso en orden y valor de la ranura a la llamada; las tres ranuras a cero al salir y el resto de la arena intacto.
- **3 FindIndexWithPedStat**: tabla en una arena de 44 entradas, nula o bruta (incluida con vuelta de 2³²); argumento = entrada i (primera, última, intermedias), justo después, más allá, dentro de una entrada, antes de la tabla, nulo y brutos lejanos.

## Oráculo (`fixture_oracle.py`)

Modelo en Python escrito desde el desensamblado (no desde el C++): x87 de 24 bits exacta con fracciones (`round24`), tablas de etiquetas de punteros y búsqueda del índice módulo 2³². `run_local.py` lo compara con el lado EXE de cada par. Autoprueba: `python -I fixture_oracle.py`.

## Suposiciones y riesgos

- `ms_apPedStats` (0xC0BBEC) se sobrescribe sólo durante la llamada y se restaura antes de comprobar las regiones.
- Con la tabla en la arena, los argumentos brutos son < 0x10000 o >= 0xF0000000 (nunca caen sobre la arena).
- No se prueba que `CleanUpOldReference` (cuerpo real) deje las referencias coherentes: está sustituido por un falso (se valida en su propio banco).
