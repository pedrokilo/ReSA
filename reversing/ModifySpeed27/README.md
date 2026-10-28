<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco ModifySpeed27: CAutoPilot::ModifySpeed (0x41B980)

**Estado: APROBADO el 18/10/2026** (corrida 20261018-131101-659386: 4096 pares, cero diferencias, estado y partidas restaurados; EAX/ECX/EDX sólo registrados porque ningún llamador los lee). Evidencia archivada en `reversing/CppTwelveLevel6Final/runs/ModifySpeed27-20261018-131101-659386/` y registrada con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`).

Plantilla: `reversing/Dependencies25A` (Core/Fixtures/LiveComparison/build_probe/run_local/fixture_oracle) y contrato de `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh ModifySpeed27          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/ModifySpeed27/build_probe.py
python -B reversing/ModifySpeed27/run_local.py
python -B reversing/ModifySpeed27/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> ModifySpeed27 # si hace falta archivar
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(ModifySpeed, 0x41B980)` y `CCurves::CalcSpeedScaleFactor` (hook 0x43C710 puede estar encendido o apagado: el banco parchea la dirección del EXE y el cuerpo de la DLL). 4096 pares.

## Qué hace la función (desensamblado 0x41B980..0x41BC6D)

1. `target = target > 0,01f ? target : 0,01f` (`fcom`+`test ah,41h`: también sustituye NaN).
2. `elapsed = (int32)([B7CB84] - field_C(+0xC))`; si es negativo se suma 2^32 (float, 24 bits); `q = elapsed / (int32)m_nSpeedScaleFactor(+0x10)` (`fidiv`), `q` a pila. `m_speed(+0x38) = target`.
3. Salida temprana si `ThePaths.m_pPathNodes[word[+0x14]>>10] == 0` (actual) y luego si `m_pPathNodes[word[+0x16]>>10] == 0` (siguiente). Ya se escribió `m_speed`; no se toca +0xC ni +0x10.
4. Enlaces A y B = `m_pNaviNodes[addr>>10][addr&0x3FF]` (14 bytes). Direcciones `(dir_i8 * 0,01f) * carril_i8` con `_smthCurr(+0x25)`/`_smthNext(+0x26)`. Anchos: `(OneWayLaneOffset + int8 m_nCurrentLane(+0x27)/m_nNextLane(+0x28)) * 5,4f` (0x44DB00: ver abajo).
5. `start = (ax*0,125 + wA*aDirY, ay*0,125 - wA*aDirX, 0)`, `end` análogo con B; `scale = CalcSpeedScaleFactor(&start, &end, aDirX, aDirY, bDirX, bDirY)` (0x43C710).
6. `m_nSpeedScaleFactor = _ftol2(scale * (1000 / m_speed))`; `field_C = _ftol2(now' - newScale * q)` (`now'` = now, +2^32 si es negativo con signo).

`CCarPathLink::OneWayLaneOffset` (0x44DB00, ya inline en la fuente): `opp = flags&7`, `same = (flags>>3)&7` con `flags = byte[+0xB]`; `opp==0` → `0,5 - same*0,5`; `(flags&0x38)==0` → `0,5 - opp*0,5`; si no → `byte[+0xA]*0,011574074 + 0,5`. Constantes del EXE: 0x858B8C=0,5, 0x858EEC=0,011574073694646358, 0x858C50=5,4000001, 0x858C58=0,01f, 0x858C48=0,125, 0x858C4C=1000.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- Objeto `CAutoPilot` completo (0x98 bytes: debe cambiar sólo +0x38, y +0xC/+0x10 cuando no hay salida temprana), guardas antes/después del objeto, las arenas de enlaces (incluida una arena «veneno» a la que apuntan las áreas sin cargar y que no debe leerse ni escribirse), tabla `ThePaths` (0x96F854..0x96FA94) y temporizador sin cambios durante la llamada.
- **Único colaborador: 0x43C710 CCurves::CalcSpeedScaleFactor** (según el coordinador, 0x44DB00 ya no se llama). Falso en la dirección del EXE y en el cuerpo de la DLL (`Resolve` salta los `E9`); registra 11 dwords por llamada: `1`, inicio x/y/z, fin x/y/z, aDirX, aDirY, bDirX, bDirY (bits). Se exige exactamente 1 llamada con esos argumentos cuando hay ramas completas y 0 en salidas tempranas (`calls`/`traps_zero`).
- ESP (`ret 4`: `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0 (EFLAGS capturados justo tras el `call`; sólo DF se compara).
- x87: **no** se exige igualdad cruda de FNSAVE (a diferencia de Dependencies25A, aquí el EXE ejecuta x87 y la DLL puede usar SSE). Se compara FCW completa (precisión 24, redondeo al más cercano, máscaras), TOP, palabra de etiquetas y los ST válidos (no debe quedar ninguno); se ignoran banderas de excepción y C0-C3 de FSW, FIP/FCS/FOP/FDP/FDS y los registros vacíos. MXCSR sin banderas (`& 0xFFC0`). `begin` registra `compare_dead_x87_registers=false` y run_local lo exige así.
- EAX/ECX/EDX de salida: **sólo se registran** (`original_residue`, `candidate_residue`, con EFLAGS), `kCompareVolatileRegisters=false`. Motivo: la función es `void`; `python -I reversing/Dependencies25A/residue_scan.py 0x41B980` da 9 llamadores (0x42E526, 0x42E600, 0x43484B, 0x43487E, 0x434898, 0x4348DC, 0x4348F6, 0x436A0E, 0x436A50) y ninguno lee EAX/ECX/EDX antes de escribirlos (0x42E526 y 0x42E600 salen como `LIMIT`: el escáner no concluyó; en 0x42E600 la instrucción siguiente es `mov cx,[esi+3A4h]`, sólo escribe CX, hay que mirar a mano si se quiere afirmar «nadie lo lee»). Una C++ no puede reproducir esos restos (el EXE deja EAX = entrada de `m_pPathNodes`/_ftol2 según el camino). Si el coordinador quiere forzarlos, poner `kCompareVolatileRegisters=true` (fallará por diseño) y quitar la comprobación de `compare_volatile_registers` en run_local.
- Restauración: `ThePaths` (region guardada/restaurada tras cada lado), `CTimer` (0xB7CB84), entradas de código parcheadas (0x41B980, 0x43C710, cuerpo DLL), hook (`HookRoute::Restore`), x87/MXCSR.

## Fixtures (4096 casos; `Fixtures.hpp`)

- `target`: 0, -0, 0,01f y vecinos, negativos, ±NaN (sólo silenciosos), ±inf, denormal mínimo, FLT_MAX, típicos y bits crudos (nunca sNaN).
- Presencia (`pres`): 12,5 % área actual sin cargar, 12,5 % actual cargada y siguiente sin cargar, el resto ambas cargadas. Las áreas sin cargar tienen `m_pPathNodes[area]=0` y `m_pNaviNodes[area]` = arena veneno.
- Direcciones `area<<10|índice`: áreas distintas, misma área/índice distinto, misma dirección (A y B alias), extremos 0/63 y 0/1023. Cada área usa una arena propia de 1024 enlaces (14 bytes) con el enlace sembrado en su índice.
- Enlaces: posición int16 con extremos, dir int8 (incl. -128/127/0), ancho +0xA, flags +0xB en las tres ramas de 0x44DB00 (opp==0; opp!=0 y same==0; opp!=0 y same!=0) más 0, 0xFF y bits 6-7 libres.
- Carriles `_smthCurr/_smthNext` y `m_nCurrentLane/m_nNextLane` int8 con extremos y negativos; `m_nSpeedScaleFactor` int32 (nunca 0), incluidos negativos, INT_MIN, 0x7FFFFFFF.
- Tiempo: 8 familias (diferencia ≥0 pequeña; diferencia negativa de 32 bits; ahora ≥ 2^31; diferencia enorme; aleatorio; diferencia 0; **empates a 24 bits**; ambos ≥ 2^31).
- Retorno del colaborador: típicos, 0, negativos, enormes (FLT_MAX), minúsculos, NaN, ±inf (→ `_ftol2` devuelve parte baja 0).
- Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

`fixture_oracle.py` modela la función desde el desensamblado con `Fraction` (x87 a 24 bits: un único redondeo por operación) y comprueba cada lado; `run_local.py` exige cobertura mínima (ramas 1/2/3 de 0x44DB00 para A y B, salidas tempranas, diferencia negativa, ahora negativo, misma área, etc.).

## Discrepancia fuente/EXE encontrada (no se editó source/)

**`field_C` (+0xC): doble redondeo en la fuente actual.** El EXE calcula `now' - newScale*q` en el x87 (80 bits) y redondea UNA vez a 24 bits. La fuente hace `(float)(nowD - (double)prod)`: la resta en `double` pierde el signo de un residuo diminuto y el `(float)` posterior resuelve un empate «a par». Ocurre cuando `now'` está justo en un empate de 24 bits (entero impar > 2^24, p. ej. 16777219) y `|newScale*q|` es muy pequeño (p. ej. `field_C = now-1`, `m_nSpeedScaleFactor` ≈ 2^30, newScale = 1): EXE → 16777218, fuente → 16777220 (comprobado en el oráculo; `source_style_differs` cuenta cuántos pares caen ahí). Las 512 fixtures de la familia 6 (`kind==6`) lo provocan a propósito; **se esperan fallos en esas filas** si la fuente no cambia. Es un caso exótico en el juego, pero el banco exige bits idénticos. Arreglo sugerido (para quien edite la fuente): hacer la resta exacta y redondear una vez (p. ej. two-sum: calcular `s = nowD - prod` y el error `e` en double y, si `e != 0` y el significando de `s` es par, empujar `s` un ulp-53 hacia `e` — redondeo a impar — antes del `(float)`), o usar aritmética entera/`long double` sólo si el compilador lo respeta.

Comprobado sin discrepancias: umbral/NaN de `target`; `q` (cociente double→float correcto por 53 ≥ 2·24+2); suma de 2^32 para diferencia y `now` negativos; ramas de salida temprana (orden: primero actual, luego siguiente; `m_speed` ya escrito); direcciones `(dir*0,01f)*carril`; orden de las dos evaluaciones de ancho; `start/end` (x usa Dir**Y**, y resta Dir**X**); orden de argumentos de 0x43C710 (start, end, aDirX, aDirY, bDirX, bDirY); `newScale` (producto float×float exacto en double, un redondeo); `prod = newScale*q` (newScale tiene ≤ 24 bits significativos, producto exacto en double). Offsets de campos coinciden con AutoPilot.h (+0xC, +0x10, +0x14, +0x16, +0x25..+0x28, +0x38).

## Suposiciones a vigilar al compilar

- `Curves.h`, `PathFind.h`, `Timer.h`, `AutoPilot.h` se incluyen desde `LiveComparison.hpp`; `CCurves::CalcSpeedScaleFactor` es estática (puntero de función normal) y, si no está enganchada o está en otra TU, su cuerpo se resuelve igual con `Resolve`. Se asume que `ModifySpeed` llama a `CalcSpeedScaleFactor` directamente (no inlineada entre TUs).
- `NUM_TOTAL_PATH_NODE_AREAS = 72` (tablas contiguas 0x96F854 y 0x96F974, 0x240 bytes guardados); sólo se usan áreas 0..63 (`addr>>10` de 16 bits).
- `sizeof(CCarPathLink)==14` y `sizeof(CAutoPilot)==0x98` (el banco trabaja con esos números a mano).
- `_controlfp(_PC_24|_RC_NEAR, …)` por lado (como en la compuerta de Dependencies25A) deja el x87 a 24 bits, que es el estado del juego; el x87 de la sonda se restaura con `frstor` tras cada lado.
- `m_nSpeedScaleFactor == 0` no se genera (fidiv por cero: excepción enmascarada, no es observable por llamadores y mezcla banderas); no está cubierto.
- No hay arnés `offline/` (no se pudo compilar): la validación sin juego de otras bancas no existe aquí; el oráculo sí se autoprueba (`python -I fixture_oracle.py`) y se comprobó a mano el caso de empate.
- Los llamadores del EXE de 0x41B980 siempre empujan 0 (`push 0`) en los dos que se vieron (0x42E51E, 0x42E5FE): el banco los cubre como `target ≤ 0,01`.
- `build_probe.py` sigue la plantilla (clon `E:\JUEGOS\GTA San Andreas - CVehicle Test`, sólo prueba); la lista de fuentes incluye `AutoPilot.cpp/.h`, `PathFind.h`, `Curves.cpp/.h`, `Timer.h`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `README.md`.
