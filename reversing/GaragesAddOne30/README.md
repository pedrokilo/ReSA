<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco GaragesAddOne30: CGarages::AddOne (0x004471E0)

**Estado: PREPARADO (lote 30; no se ha compilado el proyecto ni ejecutado GTA).** Lo único que sí se ha ejecutado: la prueba fuera del juego `offline_check.py` (EXE canónico vs C++ de la DLL vs oráculo, 4096 casos, 0 diferencias; ver abajo), la autoprueba del oráculo y el análisis de `run_local.py` sobre un registro sintético generado con el oráculo. Plantilla: `reversing/PickupsGenNew28` (misma estructura que `QuatGetAxis29`, `FileLoaderLoad2dEffect27`).

Worktree de trabajo: `D:\Original\wc-3` (HEAD `c5318699a`). Fuente: `source.patch` (sólo esta función; `git apply --check` contra `git show HEAD:<archivo>` verificado en un directorio temporal para `Garages.cpp`, `X87Math.cpp`, `X87Math.h`) y `source-both.patch` (AddOne + StoreCarInNearestImpoundingGarage juntos = `git diff HEAD` del worktree; los dos parches individuales añaden al final de `X87Math.cpp/.h` y por tanto no se aplican uno tras otro sin fusión).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh GaragesAddOne30          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/GaragesAddOne30/build_probe.py
python -B reversing/GaragesAddOne30/run_local.py
python -B reversing/GaragesAddOne30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/GaragesAddOne30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -B reversing/GaragesAddOne30/offline_check.py                                               # EXE vs DLL vs oráculo fuera del juego (ya ejecutada: cases 4096 E!=D 0 E!=oracle 0 D!=oracle 0)
python -B reversing/GaragesAddOne30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(AddOne, 0x4471E0)` (clase `CGarages`, hook `Simple`; `HookRoute` lo busca por esos nombres). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `garagesaddone30-runtime.start`; salida `garagesaddone30-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CGarages,AddOne,0x004471E0,1,0,1,S` (la añade el coordinador al registrar); la envoltura `AddOne` desaparece de las listas de envolturas pendientes.

## Fuente (FASE 1)

`source.patch` toca:

- `source/game_sa/Garages.cpp`: se activa `RH_ScopedInstall(AddOne, 0x4471E0)` y `CGarages::AddOne` deja de llamar a `plugin::Call<0x4471E0, ...>`. Se reconstruye en C++ con un ayudante local `GarageQuietNan` (un NaN señal que pasa por `fld`/`fstp` sale silenciado) y los ayudantes x87 de abajo. La firma NO cambia (`void`, `uint8 type`): el borrador del árbol principal la cambiaba a `int32` (el EXE deja el índice en EAX), pero ningún llamador lo lee (ver abajo), así que `Garages.h` no se toca.
- `source/game_sa/X87Math.h` / `X87Math.cpp` (al final, prefijo `Garage`): `GarageSumDiffCpp(a, b, c)` = `float((a + b) - c)`, `GarageDiffCpp(a, b)` = `float(a - b)` y `GarageNormalizeCpp(x, y, &outX, &outY, &length)`: `s = sqrt(y*y + x*x)` sin redondear, `*length = float(s)`, `*outX = float(x / s)` (con `s` en registro) y `*outY = float(y / *length)` (con el float guardado). `noinline`, compilados con las opciones del archivo (`/O2 /Oi /arch:IA32 /fp:fast`). Objeto comprobado con `cl /FAs`: `fld/fadd/fsub/fstp` (SumDiff), `fld/fsub/fstp` (Diff) y `fld, fld, fmul, fmul, faddp, fsqrt, fst, fdivp ST(2), fdiv [stored]` (Normalize), sin guardados intermedios.

## Qué hace el EXE (desensamblado 0x4471E0..0x4475F3, cdecl, `ret` normal)

Pila: +04 x1, +08 y1, +0C z1, +10 frontX, +14 frontY, +18 x2, +1C y2, +20 z2 (floats), +24 type (BYTE), +28 a10 (**no se lee**), +2C name, +30 flags (sólo el byte bajo). `g = aGarages[NumGarages]` (0x96C048 + 0xD8 · NumGarages; sin comprobar el límite de 50).

1. Esquina `d = float((frontX + x2) - x1)` (`fld, fadd [m32], fsub [m32], fstp`). Izquierda = `min(min(x1, frontX), x2)` luego `min(.., d)` con `a < b ? a : b` (`fcomp; test ah,5; jp` ⇒ «menor» sólo si estrictamente menor y ordenado; si no, el segundo operando), derecha igual con `a > b ? a : b` (`test ah,41h; jne`). Cada selección se carga con `fld` y se guarda con `fstp` (**un NaN señal sale silenciado**). Y igual con y1, frontY, y2 → frente/fondo. Escrituras: +28 izquierda, +2C derecha, +30 frente, +34 fondo.
2. +00..+08 = x1, y1, z1 y +1C = z2 por `mov dword` (bits tal cual). A = (float(frontX − x1), float(frontY − y1)) → +0C, +10; B = (float(x2 − x1), float(y2 − y1)) → +14, +18.
3. Normalización de A (0x4474E8..0x447570): relee ay, ax; `s = fsqrt(ay² + ax²)` en registro (24 bits); +20 = float(s); +0C = float(ax / s) con **s del registro**; +10 = float(ay / [+20]) con **el float guardado**. B igual con +24.
4. +4C = +4F = type (byte); `strncpy(g + 0x44, name, 7)` (0x821F40: copia hasta 7 caracteres y rellena con ceros; +4B no se toca); byte +4E: bit 3 = flags&1, bit 4 = flags&2, bit 5 = flags&4, los demás se conservan; `NumGarages += 1`; EAX = índice anterior.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado de 0x4471E0..0x4475F3 contrastado instrucción a instrucción con el C++ y, además, **ejecución real de los bytes del EXE** frente al C++ de la DLL y al oráculo (4096 casos, `offline_check.py`). Discrepancias halladas respecto del borrador del árbol principal (`git diff HEAD -- source/game_sa/Garages.cpp` de D:\Original\gta-reversed-master) y corregidas:

- El borrador hacía todas las sumas, la raíz y las divisiones en `float` SSE: difiere del x87 de 24 bits con exponente extendido cuando `frontX + x2` desborda float pero `(frontX + x2) − x1` no (p. ej. 3e38 + 3e38 − 3e38), cuando `y² + x²` desborda (|x| ≥ 1,8e19 ⇒ SSE da `inf`, la x87 sigue finita) o desborda por abajo (|x| < 1e-19: SSE da 0), y en los NaN: SSE conserva un NaN señal en las copias y toma el primer operando entre dos NaN, el x87 silencia y toma el de mayor significando (el QNaN si el otro es señal). Se sustituyó por los ayudantes de `X87Math.cpp` y `GarageQuietNan`.
- El borrador dividía X e Y por el `m_fWidth` ya guardado como float; el EXE divide X por la raíz **en registro** (sólo difiere cuando la raíz cae en el rango subnormal de float).
- El borrador cambiaba la firma a `int32`; no hace falta (ver llamadores).
- Coinciden con el EXE y no se tocaron: el orden de los operandos de min/max (`x1, frontX, x2, esquina`), la lectura de la esquina como float guardado, la copia en bruto de x1, y1, z1, z2, `a10` sin leer, `strncpy(…, 7)`, las tres banderas, `m_nOriginalType` y el incremento de `NumGarages`.
- Hallazgo durante la prueba fuera del juego (error del arnés y del oráculo, no de la fuente): `std::bit_cast<float>` devuelve en ST0 y silenciaba el sNaN al construir los argumentos (el arnés llama ahora a la DLL con la pila en bruto, como el hook); el oráculo trataba igual un operando de memoria `fadd/fsub [m32]` y uno cargado con `fld` — con dos NaN el x87 prefiere el QNaN al SNaN de memoria (Intel SDM vol. 1, tabla 4-7); corregido en `x87model.py` (`mem()`).

**Llamadores del EXE** (`residue_scan.py 0x4471E0`): uno, `CFileLoader::LoadGarage` en 0x5B45BA (`call 4471E0; add esp,30h; add esp,30h; ret`): ningún llamador lee EAX/ECX/EDX tras la llamada, por lo que **sólo se registran** (`kCompareVolatileRegisters=false`). Tras la conversión de `AddOne`, el llamador en C++ (`FileLoader.cpp`) llama a la función C++ directamente.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Ranura completa** de `aGarages[NumGarages]` (0xD8 bytes en hex, con relleno previo aleatorio), **resto del array intacto** (`rest_ok`), `NumGarages` (= índice + 1) y la **arena del nombre** (32 bytes: guarda 0xCC, el nombre en +8 con terminador 0, guarda 0xCC; `name_ok`).
- Oráculo (`fixture_oracle.py` + `x87model.py`, desde el desensamblado): x87 de 24 bits con exponente extendido (`r24`), `fstp dword` al rango float32, NaN/inf/ceros con signo, reglas de NaN del x87 (un único NaN se devuelve silenciado; SNaN y QNaN → el QNaN; dos QNaN → mayor significando; empate con signos opuestos → comodín `??` y sólo se exige EXE == DLL), selección min/max por comparaciones ordenadas. Autoprueba (ya ejecutada: ok): rectángulos a mano, banderas, nombres largos, esquina que decide, NaN/sNaN, dirección de longitud cero (0/0 = indefinido), 24 bits (16777216 + 1), exponente extendido (3e38, 3e19²) y 3000 rectángulos aleatorios contra una cadena float32 (≤ 30 diferencias de doble redondeo admitidas; el resto exacto).
- ABI: ESP (`ret` normal: la compuerta retira 0x30), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), hook/ruta (incluye que `strncpy` 0x821F40 del EXE no esté enganchado), entradas de código vigiladas (0x4471E0, 0x821F40), array de garajes y `NumGarages` del juego restaurados al final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador los lee).

## Fixtures (`Fixtures.hpp`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos; 1 retícula pequeña (empates y todos los órdenes de los límites); 2 ceros con signo; 3 NaN (≥ 1, 8 variantes incl. señal); 4 infinitos; 5 enormes (≥ 1e19, hasta FLT_MAX); 6 diminutos y subnormales; 7 direcciones degeneradas (longitud cero, 0/0, x/0); 8 redondeo a 24 bits (magnitudes ~1e6..4e7 con diferencias pequeñas); 9 `type`/`flags`/`a10` con basura en bytes altos y barrido de los 8 valores de flags; 10 nombres de 0..14 bytes (7 y 8 exactos, bytes altos); 11 índices frontera 0, 1, 2, 24, 25, 47, 48, 49; 12 bits crudos; 13 retícula con NaN; 14 garajes reales (rectángulo alineado o girado); 15 z1/z2 especiales (NaN, ±0, subnormal, inf). Semillas de pila por caso (64 dwords bajo ESP), EAX/ECX/EDX por caso.

Cobertura calibrada con `fixture_dump.cpp` (compilado con `cl`) + `fixture_oracle.py` (recuentos medidos; mínimos en `run_local.py` con ~45 % de margen, más 256 exactos por familia): all_known 4050, corner_x_left 1229, corner_x_right 1301, corner_y_back 1328, corner_y_front 1166, finite_dirs 3179, finite_in 3223, flags_0..7 474..552, flags_high_garbage 2029, height_inf 245, height_nan 544, height_zero 120, huge_in 543, index_0 187, index_49 55, inf_in 298, left_front 544, left_x2 590, limits_nan 676, limits_unknown 24, name_empty 75, name_high_bytes 3041, name_long 1046, name_short 2498, nan_in 580, negzero_in 298, right_front 687, right_x2 582, snan_in 313, tiny_in 491, type_high_garbage 1550, width_inf 248, width_nan 532, width_zero 163, zero_dir_a 163, zero_dir_b 120.

## Prueba fuera del juego (`offline_check.py`)

Ejecuta los **bytes del EXE** (0x4471E0 reubicado a 0x900000 con su `call` a `strncpy` reapuntado, y los bytes reales de `strncpy` en 0x821F40; la memoria de tablas en sus direcciones fijas reales) y las funciones **C++ de la DLL** (cuerpos extraídos de `Garages.cpp`, `X87Math.cpp` compilado con las opciones del proyecto, el resto con `/Od`) sobre las mismas 4096 fixtures y compara EXE == DLL == oráculo: `cases 4096 E!=D 0 E!=oracle 0 D!=oracle 0`. No sustituye al banco (no prueba el enganche del hook, la ABI de la sonda ni el entorno del juego) pero valida la fuente y el oráculo antes de gastar una corrida. Arena y constantes: `crosscheck` compara el volcado de `Fixtures.hpp` compilado con `cl` (tamaños 0xD8, 50, 0x2A30, direcciones, arena del nombre 32/8) con las constantes del oráculo, y `run_local.py` exige que el `begin` de la corrida repita las mismas.

## Suposiciones y riesgos del banco

- Ni `Core.hpp` ni `LiveComparison.hpp` se han compilado dentro del proyecto (sólo `Core.hpp` con un `stub.hpp` mínimo de `json`/`HookRoute`/`EntryPatch`, sin errores). Si `run_local.py` rechaza por cobertura, comparar primero las filas `input` registradas con el volcado de `fixture_dump.cpp`.
- El banco sustituye el array de garajes (0x96C048, 0x2A30 bytes) y `NumGarages` por los del caso (se guardan y restauran); lo hace dentro de `CGame::Process`, sin código del juego concurrente. Índices 0..49 (el 49 es el último que cabe: el array termina en 0x96EA78 = `LastUpdatedGarageId`).
- `strncpy` del lado candidato es el de la CRT de la DLL; el del EXE (0x821F40) lee por palabras (hasta 3 bytes más allá del terminador): la arena lo admite (guarda de 0xCC).
- Un NaN con el mismo significando y signos opuestos en una operación (`??`) sólo se exige EXE == DLL. sNaN en operandos de memoria se cubre (313 casos).
- El hook de `AddOne` queda activo en producción; los demás hooks (`Init`, `Shutdown`...) conservan su estado y no intervienen.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `x87model.py` (copia idéntica en GaragesImpound30), `fixture_dump.cpp`, `offline_check.py`, `offline_harness.cpp`, `register_hook.py`, `source.patch`, `source-both.patch`, `README.md`.
