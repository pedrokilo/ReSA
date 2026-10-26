<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatConjugate29: CQuaternion::Conjugate() (0x4D37D0)

**Estado: APROBADO y registrado (19/10/2026, corrida 20261019-003748-740133, 4096 pares, EXE == DLL bit a bit; informe en resultados-quatconjugate29\20261019-003748-740133).** Antes: preparado. Plantilla: `reversing/QuatGetAxis29` (misma clase y mismo esquema de ruta RH/compuertas).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatConjugate29
# o a mano, siempre en serie:
python -B reversing/QuatConjugate29/build_probe.py
python -B reversing/QuatConjugate29/run_local.py
python -B reversing/QuatConjugate29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatConjugate29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatConjugate29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatConjugate29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(Conjugate, 0x4D37D0)` (hook «Conjugate», clase `CQuaternion`). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `quatconjugate29-runtime.start`; salida `quatconjugate29-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv` que añade el coordinador al registrar: `CQuaternion,Conjugate,0x004D37D0,1,0,1,S`. (`run_local.py`/`build_probe.py` usan `probe_case`, `runtime_runner`, `probe_build` del árbol principal.)

## Fuente (FASE 1)

`source.patch` (git diff contra HEAD, sólo esta función; verificado con `git apply --check` sobre `git show HEAD:source/game_sa/Core/Quaternion.cpp`) toca únicamente `source/game_sa/Core/Quaternion.cpp`: activa `RH_ScopedInstall(Conjugate, 0x4D37D0)` y sustituye la llamada `((void(__thiscall*)(CQuaternion*))0x4D37D0)(this)` por C++. No toca X87Math.cpp/.h (no hace falta un ayudante x87).

Qué hace el EXE (0x4D37D0..0x4D37E6, `ret`, thiscall, ECX = this, sin argumentos):

```
fld dword [ecx]   ; fchs ; fstp dword [ecx]         x = -x
fld dword [ecx+4] ; fchs ; fstp dword [ecx+4]       y = -y
fld dword [ecx+8] ; fchs ; fstp dword [ecx+8]       z = -z       (w, [ecx+0xC], no se toca)
ret
```
Semántica exacta: `fld dword` es exacta salvo para un sNaN, que se convierte en qNaN (bit 22 a 1; la excepción IE está enmascarada) y `fchs` invierte el signo de cualquier valor (NaN incluido); `fstp dword` devuelve a float32 sin pérdida. Resultado: `bits ^ 0x80000000`, más `bits |= 0x00400000` si el operando es NaN (exponente todo unos y mantisa ≠ 0). Es lo que hace el C++ (`std::bit_cast`, sólo enteros, sin tocar el x87: la pila queda vacía como en el EXE; la única diferencia no observable es la bandera IE sticky de FSW que el EXE deja tras un sNaN). Un `x = -x` ingenuo en float daría un sNaN con el signo invertido sin silenciarlo (o, con SSE, el mismo resultado según el compilador): por eso se hace con enteros.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado (`dis.sh 0x4D37D0 0x4D3800`) contrastado con el C++: tres bloques idénticos fld/fchs/fstp sobre [ecx], [ecx+4], [ecx+8], orden x, y, z (no observable, no hay solapamiento), w intacto, pila x87 vacía, `ret` sin limpieza de pila (`popBytes=0`, sin argumentos), sin llamadas ni accesos a globales: **coincide, ninguna discrepancia**. ECX/EAX/EDX no se modifican en el EXE; la DLL (void, `__thiscall`) no los reproduce.

**Llamadores del EXE: ninguno.** `residue_scan.py 0x4D37D0` no devuelve ningún `call`/`jmp` (control: 0x59C080 sí devuelve llamadores) y una búsqueda propia sobre las secciones del EXE de `E8/E9 rel32` hacia 0x4D37D0 y de la dirección como dword tampoco halla nada. Nadie lee EAX/ECX/EDX de salida: sólo se registran (`kCompareVolatileRegisters=false`). En la fuente sólo `Conjugated()` (Quaternion.h) la llama y nadie usa `Conjugated()`.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena completa** (0xC0 bytes: guarda 0x40, ventana 0x40 con el cuaternion en `qoff`, guarda 0x40; relleno previo aleatorio, 0x00, 0xFF o NaN silenciosos) sin comodines: la función es determinista.
- Oráculo (`fixture_oracle.py`, desde el desensamblado): fld/fchs/fstp modelado (sNaN → qNaN, signo invertido, w intacto); los no-NaN se niegan por una vía independiente (floats de Python, `-0.0` incluido) y la autoprueba la compara con la vía de bits sobre 20000 valores aleatorios más los extremos; también cubre desalineado, sNaN y qNaN con carga útil.
- ABI: ESP (`ret`: `popBytes=0`), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir; se ignoran banderas de FSW como IE), MXCSR sin banderas, hook/ruta, entradas de código vigiladas.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador). No hay colaboradores ni errno.

## Fixtures (`Fixtures.hpp`; espejo `fixture_mirror.py`): 4096 casos = 8 familias x 512 (familia = n % 8)

0 normales; 1 especiales (ceros, ±1, subnormales, máximos, infinitos); 2 ceros/subnormales/infinitos con signo; 3 qNaN con cargas útiles; 4 sNaN en x, y, z y w; 5 mezcla de todo; 6 bits crudos (incluye sNaN) con cuaternion desalineado (`qoff` 0..48); 7 xyz con sNaN frecuentes y w con mezcla. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`. Cobertura medida con el espejo: kind_normal 2211, kind_snan 1157 (any_snan 1157), kind_qnan 755, kind_inf 760, kind_sub 1014, kind_zero 142, unaligned 366, w_snan 636; mínimos de `run_local.py` con ~40 % de margen.

## Suposiciones y riesgos

- No se compiló: el espejo Python de las fixtures no se ha contrastado con el generador C++; si las filas `input` difieren, corregir uno de los dos antes de bajar umbrales.
- Se asume que el hardware silencia el sNaN en `fld dword` con las excepciones enmascaradas (FCW por defecto del banco: sólo se fija precisión/redondeo). Si la corrida mostrase otra cosa en la familia 4/7, el que manda es el EXE y el oráculo + C++ deben ajustarse.
- Alcance: esta función no completa la clase `CQuaternion`.
