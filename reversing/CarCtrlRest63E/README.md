<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63E (tanda 5): ChooseModel

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 a 5). 4096 pares, cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados, `eax` de salida igual (la función devuelve un modelo o -1 y la categoría por puntero) y cobertura de ramas medida sobre los eventos de los colaboradores.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63e\20261020-213238-930999`.

Primera corrida (21:07): 4096 diferencias por dos defectos míos de transcripción, ambos corregidos (ver «Defectos hallados»).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::ChooseModel` | 0x424CE0 | Simple (`RH_ScopedInstall`) | 4096 | `rand` del CRT del EXE 0x821B1E (sin hook), `CGangWars::GangWarFightingGoingOn` 0x443AC0, `CCarCtrl::ChoosePoliceCarModel` 0x421980, `CLoadedCarGroup::PickRandomCar` 0x611C50, `CPopulation::PickGangCar` 0x614490 y la función de 0x407180 (`GetRandomNumberInRange_int32`, estática de General.cpp, con hook; el cuerpo de la DLL se resuelve siguiendo el salto del EXE) |

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63E           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63E/build_probe.py
python -B reversing/CarCtrlRest63E/run_local.py
python -B reversing/CarCtrlRest63E/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63E/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63e/20261020-213238-930999"   # sólo lo ejecuta el coordinador
```
Fila de `docs/hooks.csv` (la añade el coordinador; tipo S): `CCarCtrl,ChooseModel,0x00424CE0,...,S`. `SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque: `carctrlrest63e-runtime.start`.

## Fuente (`source.patch`: sólo los cambios de la tanda 5 respecto a la tanda 4)

`CarCtrl.cpp`: `ChooseModel` en C++, su línea de hook y las declaraciones locales de los ayudantes x87 `Cc63e*`, definidos al final de `X87Math.cpp` (`Cc63eTotalCpp`, `Cc63eRollFailsCpp`, `Cc63eRatioCpp`, `Cc63eTierExceedsCpp`,
`Cc63ePickRollCpp`, `Cc63eGroupRollCpp`; aritmética x87 de 24 bits). Declarados en `CarCtrl.cpp` y no en `X87Math.h` para no recompilar todo el proyecto; al fusionar conviene moverlos al encabezado.

## Revisión contra el EXE (re-desensamblado; direcciones verificadas contra los bytes)

`total = ftol2(((F0 + F1) + F2) + F3)` con F0..F3 = `CPopCycle::m_NumOther/Cops/Gangs/Dealers_Cars` (0xC0BC30..3C); `total <= 0` -> -1. Con el byte 0x96917A: `rand & 0xFFFF` · (1/65536) · 100.0f truncado distinto de 0 -> -1. Se sortea `r = rand()` (sin enmascarar),
`f = float(r · 0x38000100)` (0x858C7C), `t = float(total)` y el byte 0x96915B. Categorías (cada una exige que el acumulado / t sea mayor que f, comparación ordenada con un redondeo a 24 bits por operación):
dealers (`F3 / t > f` y byte 0x96915B == 0): `*cat = 0x19`, modelo = word 0xC0F240 si `byte[0x8E4CD0 + modelo·20] == 1`, si no -1; pandillas (`(F2 + F3) / t > f`, o byte 0x96915B != 0): -1 si 0xC0FCB2; suma de los 10 bytes de
`CPopCycle::m_pCurrZoneInfo` (0xC0BC68, sin comprobar), `<= 0` -> -1; sorteo ponderado (`rand & 0xFFFF` · (1/65536) · suma, `_ftol2`; bucle `while (b[i] < k) { k -= b[i]; i++ }` con `b[0]` releído solo en la primera vuelta); con el byte 0x96915B se sustituye por
`0x407180(0, 9)`; `*cat = i + 14`; `PickGangCar(i) < 0` -> -1; hasta 10 intentos de `rand & 0xFFFF` · (1/65536) · 23.0f (0x858F84) sobre `word[0xC0F074 + (i·23 + k)·2]` (los valores 2000 repiten el sorteo sin consumir intento) devolviendo el primero cuyo `byte[0x8E4CD0 + modelo·20] == 1`;
policía (`(F1 + F2 + F3) / t > f`): `GangWarFightingGoingOn()` o byte 0xC0FCB4 -> -1, si no `*cat = 13` y `ChoosePoliceCarModel(0)` (llamada de cola); resto: `*cat = 0` y el modelo forzado `[0xA4448C]` si no es -1, si no `PickRandomCar(true, false)`.

Llamadores y registros: el único llamador (0x43022A) lee EAX (el modelo); el banco lo compara entero.

## Defectos hallados

- **Producción (corregido antes de aprobar):** la primera versión leía el modelo forzado en `0xA4444C`; la dirección correcta es `0xA4448C` (bytes `A1 8C 44 A4 00`). La corrida lo delató (EXE y DLL devolvían cosas distintas en el mismo caso). Lección: contrastar todas las direcciones absolutas de la transcripción con los bytes del EXE
  (hecho para esta tanda con un barrido de los operandos de 32 bits de la función).
- **Banco (corregido):** `0x407180` tiene hook (`GetRandomNumberInRange_int32`), de modo que el colaborador debía declararse con `hooked=true` y el cuerpo de la DLL resuelto desde el salto del EXE (`collaborators_dll` salía falso en ambos lados).

## Suposiciones y riesgos

- Los modelos del banco son < 600 (región de 12000 bytes de `0x8E4CD0`, un byte de estado cada 20); los valores del resto de la tabla no se prueban. El valor de retorno de los colaboradores `PickGangCar`, `PickRandomCar` y `ChoosePoliceCarModel` se acota al rango de `int16` extendido con signo (los de la DLL son `eModelID`).
- La tabla de grupos solo es alcanzable en los índices 0..11 (el `rand` del EXE da < 32768); el banco garantiza una entrada válida en ese rango para que el bucle de reintentos termine.
- `others_pick` (categoría «otros» con modelo forzado -1) queda en ~2,4 % de los casos; sus ramas están cubiertas pero con poca densidad.
