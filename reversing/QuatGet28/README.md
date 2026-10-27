<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatGet28: CQuaternion::Get(RwMatrix*) const (0x59C080)

**Estado: APROBADO** (18/10/2026, corrida `20261018-180758-275555`: 4096 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-quatget28\20261018-180758-275555`; archivado en `reversing/CppTwelveLevel6Final/runs/QuatGet28-20261018-180758-275555/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CQuaternion,Get-matrix,0x0059C080,1,0,1,S` en `docs/hooks.csv`). Corridas previas (sólo nota, no evidencia): `20261018-175254-822774` falló en el oráculo del EXE (arena de matriz de tamaño distinto de la esperada) y `20261018-180454-275013` pasó con una sonda anterior (sha 438ca4f1...).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatGet28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/QuatGet28/build_probe.py
python -B reversing/QuatGet28/run_local.py
python -B reversing/QuatGet28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatGet28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/QuatGet28/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatGet28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar
```
Requiere el ASI del árbol actual con `RH_ScopedOverloadedInstall(Get, "matrix", 0x59C080, ...)` (hook «Get-matrix», categoría/clase `CQuaternion`, `Quaternion.cpp`). Aplicar antes `source.patch` si el árbol no lo contiene (ver «Fuente»). Archivo de arranque en el clon: `quatget28-runtime.start`; salida `quatget28-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene la fila `CQuaternion,Get-matrix,0x0059C080,1,0,1,S` (la añade el coordinador al registrar).

## Fuente (FASE 1)

`source.patch` (git diff unificado contra HEAD, SÓLO esta función; verificado con `git apply --check` sobre `git show HEAD:<archivo>` en un directorio temporal) toca:

- `source/game_sa/Core/Quaternion.cpp`: `CQuaternion::Get(RwMatrix*) const` pasa a ser `RpNanQuatToMatrixCpp(&x, reinterpret_cast<float*>(out))` (ayudante x87 compartido de `X87Math.cpp`, ya existente y usado por `RpAnimBlend.cpp`; sin duplicar código) y se activa el hook `RH_ScopedOverloadedInstall(Get, "matrix", 0x59C080, ...)`. El cuerpo SSE/float anterior (que reasociaba y no coincidía con los redondeos del EXE) se elimina.
- `source/game_sa/Entity/Vehicle/Plane.cpp`: `PlaneQuaternionToMatrix` deja de llamar al EXE por dirección (`0x59C080`) y llama a `quaternion.Get(matrix)` (las dos llamadas de `CPlane::PreRender`).

No se tocan `RpAnimBlend.cpp` (sus dos llamadas ya usan el ayudante directamente; el comentario «el hook de Get está apagado» queda obsoleto) ni `InjectHooksMain.cpp` (su comentario «CQuaternion::Get remains disabled» también queda obsoleto; `CQuaternion::InjectHooks()` ya se llama). Sugerencia de coordinador: limpiar ambos comentarios con el lote.

## Qué hace la función (desensamblado 0x59C080..0x59C157, `ret 4`, thiscall)

`sub esp,20h`; ST0 = q = {x,y,z,w} = `[ecx]`, `[ecx+4]`, `[ecx+8]`, `[ecx+0xC]`; `eax = [esp+24h]` = `RwMatrix*`. x87 a 24 bits (PC del proceso):

```
2x = x+x ; 2y = y+y ; 2z = z+z -> fstp [esp] (Z2, float)          pila: 2y, 2x
A = fl(2x*x)  B = fl(2y*x)  C = fl(Z2*x)  D = fl(2y*y)  E = fl(Z2*y)  F = fl(Z2*z)     (cada una fstp dword)
G = fl(2x*w) ; Y2W = 2y*w (en registro, sin pasar por float) ; Z2W = Z2*w (en registro)
m0 [eax+00] = fl(1 - (F+D))   m4 [eax+10] = fl(B - Z2W)   m8 [eax+20] = fl(Y2W + C)
m1 [eax+04] = fl(Z2W + B)     m5 [eax+14] = fl(1 - (F+A)) m9 [eax+24] = fl(E - G)
m2 [eax+08] = fl(C - Y2W)     m6 [eax+18] = fl(G + E)     m10[eax+28] = fl(1 - (D+A))
```
(1,0 = dword `[0x858624]`, comprobado = 0x3F800000). Todas las lecturas del cuaternion preceden a la primera escritura (0x59C0F3); sólo se escriben esos nueve floats (flags, relleno y posición intactos). Sin colaboradores (ninguna llamada). El estado x87 queda con la pila vacía.

## Revisión propia contra el EXE (FASE 2)

Desensamblado (`dis.sh 0x59C080 0x59C300`) y traza línea a línea de la pila x87 contra `RpNanQuatToMatrixCpp` (ya presente en HEAD, nunca revisado contra esta función en esta sesión):

- 2x y 2y quedan en registro (se usan como `double` sin redondear a float; `x+x` ya está a 24 bits por la x87) y 2z se guarda como float (`fstp [esp]`): **coincide** (`z2 = float(...)`).
- A..F se guardan como float (fstp), G también; `Y2W` y `Z2W` NO se guardan (siguen en ST): **coincide** (`Y2W`/`Z2W` double).
- Orden y operandos de las nueve salidas, incluidos los `fsubr` con 1,0 (`1 - (F+D)`, `1 - (F+A)`, `1 - (D+A)`), `fsub st,st(1)` (`B - Z2W`, `C - Y2W`) y los sumandos: **coincide** con los tres bloques del ayudante, incluidos los destinos `[eax+00,04,08,10,14,18,20,24,28]` frente a `m[0,1,2,4,5,6,8,9,10]`.
- Discrepancias entre el ayudante y el EXE: **ninguna**. La fuente anterior de `Quaternion.cpp` (suma `imag+imag` en float SSE y `Y2R`/`Z2R` redondeados a float) SÍ difería: redondeaba `2y·w` y `2z·w` a float antes de sumarlos y, con SSE, no reproducía subnormales/NaN del x87. Por eso se sustituyó.
- Llamadores del EXE de 0x59C080 (escáner `Dependencies25A/residue_scan.py 0x59C080` + lectura a mano de las instrucciones siguientes): 0x46C793, 0x46C8A6, 0x59C168 (`CQuaternion::Get(x,y,z)`, que relee desde su propia pila y recarga ECX desde `[esp+4Ch]`), 0x6BD9CE, 0x6C0E71, 0x6C97E1, 0x6C981E, 0x6C9990, 0x6C9A97, 0x6C9C20, 0x6C9C8F, 0x6D3549 y las cinco de `RpAnimBlend` (0x4D23C3, 0x4D2706, 0x4D2AA6, 0x4D3273, 0x4D3493; ya en C++). **Ninguno lee EAX/ECX/EDX tras el `call`** (todos los que siguen escriben ECX con `lea`/`mov` antes de usarlo o no lo usan; `EBX` se conserva). Las marcas `ecx` del escáner en 0x6C9990.. son `lea ecx,[esp+70h]` (escritura); las marcas `unwritten=edx` apuntan a llamadas posteriores a 0x59BB60 / 0x59BD10, que escriben EDX antes de leerlo (comprobado en su desensamblado); las `unwritten=ecx` de los llamadores de RpAnimBlend (0x4D2441...) están en funciones que ya son C++ y llaman al ayudante, no al EXE. EAX/ECX/EDX de salida se registran pero no se comparan (`kCompareVolatileRegisters=false`).

Límites de la revisión: no se compiló el proyecto, así que NO se ha leído el objeto real de `X87Math.cpp` para esta función (instrucciones/orden x87 emitidos por el compilador): el coordinador debe contrastar el objeto compilado (`/O2 /Oi /arch:IA32 /fp:fast`, `float_control(precise,on)` en el ayudante) con el desensamblado, y el banco es la prueba definitiva.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Arena de la matriz** (0x140 bytes: guarda 0x40 + RwMatrix 0x40 + guarda 0x40; relleno previo distinto por caso: aleatorio, 0x00, 0xFF o NaN silenciosos) y **arena del cuaternion** (0x30: guardas 0x10 a cada lado), completas, en bits. Cualquier escritura fuera de los nueve floats, o del cuaternion, aparece como diferencia.
- Oráculo (`fixture_oracle.py`, desde el desensamblado): x87 a 24 bits con exponente extendido, redondeo al par con `Fraction`; fstp dword redondea al rango float32 (subnormales con cuanto 2^-149, desbordes a inf); ceros con signo (+0 en cancelación exacta, `-0 + -0 = -0`); inf; NaN (un NaN se propaga tal cual, también en `fsub`; dos NaN: mayor significando; inválidas → indefinido 0xFFC00000). Autoprueba: casos exactos a mano (identidad, 90º, ceros, inf, NaN), 3000 cuaterniones aleatorios contra una cadena float32 por operación y redondeos de float32.
- ABI: ESP (`ret 4`: `popBytes=0`), EBX/ESI/EDI/EBP sembrados y conservados, DF=0, x87 normalizado (FCW completa en 24 bits/nearest/máscaras, TOP, etiquetas, ST válidos; ningún ST válido al salir), MXCSR sin banderas (`& 0xFFC0`).
- **EAX/ECX/EDX de salida: sólo se registran** (`original_residue`, `candidate_residue`; `kCompareVolatileRegisters=false`), por la razón de arriba.
- Rutas (`route_ok`): original → el hook de 0x59C080 NO salta a la DLL; candidato → sí, con el cuerpo (`Body`) en el módulo ASI. Se vigilan 16 bytes de 0x59C080 y del cuerpo de la DLL (`globals_restored`). No hay estado global del juego que modificar ni colaboradores que sustituir (`collaborators_stubbed=false`; `run_local.py` exige colecciones `exe`/`dll` vacías).

## Fixtures (`Fixtures.hpp`; espejo en `fixture_mirror.py`): 4096 casos = 16 familias × 256 (familia = n % 16)

0 típicos (exp -6..1); 1 rango amplio (-30..20); 2 valores simples (±0, ±1, ±0,5, 0,7071, MAX, subnormales, ...); 3 ceros con signo y ±1; 4 diminutos (mitad en el borde de los subnormales, mitad exp -126..-70 y subnormales); 5 enormes (exp 70..127, desbordan a inf); 6 infinitos con ceros/valores; 7 un NaN; 8 varios NaN con significandos distintos (más inf); 9 NaN/inf/ceros mezclados (0·inf); 10 bits crudos (nunca sNaN); 11 mantisas extremas (0x7FFFFF, 1, 0x400000, 0x7FFFFE); 12 dos NaN con el mismo significando y signo opuesto (empate no resuelto); 13 cuaternion superpuesto a la matriz (13 desplazamientos 0..0x30); 14 idem con valores especiales; 15 cancelaciones exactas (B==Z2W, A==D...) para el signo del cero. Semillas de pila por caso (64 dwords bajo ESP), EAX/EDX por caso, ECX = `this`.

Cobertura calibrada con el espejo (`fixture_mirror.py`): alias 512, all_known 3831, finite_in 2633, indefinite_out 700, inf_in 628, nan_in 1062, nan_out 1703, negzero_out 357, overflow_out 567, subnormal_out 131, zero_out 712, inf_out 1119, unknown 265; mínimos en `run_local.py` con ~40-50 % de margen (más 256 exactos por familia).

## Suposiciones y riesgos del banco

- El espejo `fixture_mirror.py` y el oráculo no se han ejecutado contra una sonda real; Fixtures.hpp (C++) y el espejo deben producir las mismas filas `input`. Si `run_local.py` rechaza por cobertura, comparar primero las filas `input` registradas con el espejo antes de bajar un umbral.
- **sNaN no cubierto**: las fixtures sólo generan NaN silenciosos (las reglas x87 de sNaN en operando de memoria frente a registro no se modelan). La DLL con x87 deberá coincidir de todos modos, pero el banco no lo prueba.
- **Empates de NaN** (mismo significando, signo opuesto; familia 12 y algunos casos de 8/9/14): el oráculo deja comodín `??` y sólo se exige EXE == DLL; si la DLL elige el otro operando por reordenación del compilador (conmutatividad de `fadd`/`fmul`) saldría una diferencia real (no un fallo del banco).
- El ayudante se apoya en que el x87 del proceso está a 24 bits (como en el juego): el banco lo fija con `_controlfp(_PC_24|_RC_NEAR)` por lado. Si se llamara con otra precisión, los redondeos intermedios cambiarían (igual que en el EXE).
- Los pares con la arena del cuaternion «alias» dentro de la matriz validan que todas las lecturas preceden a las escrituras también en la DLL (sin `restrict`, el compilador debe respetar el orden).
- `CQuaternion::Get(float*,float*,float*)` (0x59C160) y `Get(RwV3d*,float*)` (0x59C230) siguen en el EXE: 0x59C160 llama a 0x59C080, que con el hook activo cae en la DLL; su comportamiento observable no cambia (relee desde su pila).
- El nombre del hook es `Get-matrix` (sufijo «matrix», como `Set-matrix`): el hook comentado en HEAD usaba sufijo vacío (`Get-`), que `docs/hooks.csv` no contiene.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
