<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco QuatMultiply29: CQuaternion::Multiply(const CQuaternion& a, const CQuaternion& b) (0x59C270)

**Estado: APROBADO y registrado (19/10/2026, corrida 20261019-005418-687938, 4096 pares, EXE == DLL bit a bit; informe en resultados-quatmultiply29\20261019-005418-687938).** Antes: preparado. Plantilla: `reversing/QuatGetAxis29` (misma clase y mismas fuentes `Quaternion.cpp` / `X87Math`).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh QuatMultiply29          # compila la sonda + corre + resume diferencias
# o a mano, siempre en serie:
python -B reversing/QuatMultiply29/build_probe.py
python -B reversing/QuatMultiply29/run_local.py
python -B reversing/QuatMultiply29/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/QuatMultiply29/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -I reversing/QuatMultiply29/fixture_mirror.py                                              # cobertura de las 4096 fixtures según el oráculo (sin juego)
python -B reversing/QuatMultiply29/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(Multiply, 0x59C270)` (hook «Multiply», clase `CQuaternion`). Aplicar antes `source.patch` si el árbol no lo contiene (`git apply --check` verificado contra la versión HEAD de Quaternion.cpp, X87Math.cpp y X87Math.h). Arranque en el clon: `quatmultiply29-runtime.start`; salida `quatmultiply29-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CQuaternion,Multiply,0x0059C270,1,0,1,S` (la añade el coordinador al registrar) y hay que retirar la entrada `CQuaternion,Multiply,0x0059C270` de `docs/implementaciones/envolturas-pendientes.csv` si existe.

## Fuente (FASE 1)

`CQuaternion::Multiply` (`Quaternion.cpp`) llama a `QuatMultiplyCpp(float* out, const float* a, const float* b)` (bloque propio al final de `X87Math.cpp`, declarado en `X87Math.h`). Sin `__asm`, sin llamadas al EXE. Se ejecuta en la máquina `FlcX87` (24 bits, banderas y residuo x87 como el EXE), con una lectura de memoria (`Fld`/`Fmul`/`Fadd` con referencia) en el mismo punto que cada `fld`/`fmul`/`fadd` del EXE y un `Fstp` inmediato a la salida:

- Pasos 1-3 (`o.x`, `o.y`, `o.z` = diferencias de dos productos) se guardan antes de los pasos 4-6, que releen **de memoria** `a`, `b` y la propia salida (`fadd [ecx]`). Con `out == a` o `out == b` (o solapamiento parcial) las lecturas posteriores ven los valores ya escritos; el EXE y la DLL siguen el mismo orden y el banco lo estresa (familias 9-14).
- `o.w` (paso 7) = `a.w*b.w - ((a.x*b.x + a.z*b.z) + a.y*b.y)`.
- Hook: `RH_ScopedInstall(Multiply, 0x59C270)`.

## Revisión propia contra el EXE (FASE 2)

Se desensambló 0x59C270..0x59C2FD (`dis.sh`) y se contrastó línea a línea con `QuatMultiplyCpp` (comentarios con las direcciones): orden de lecturas/escrituras, operando de cada `fmul`/`fadd`/`fsubp`/`faddp` (`fsubp st(1),st` = `st1 - st0`: `Arith('-',1,1,0,true)`), profundidad de pila (máx. 3 en el paso 7), `ret 8`. Sin discrepancias halladas tras la segunda lectura. Llamadores: ninguno (no hay `call`/`jmp` rel32 ni referencias absolutas a 0x59C270 en el EXE; `residue_scan.py` no halló llamadores): nadie lee EAX/ECX/EDX; el EXE deja EDX = &b, EAX = &a, ECX = this, que la DLL no reproduce (el banco los registra como `original_residue`/`candidate_residue` sin compararlos, `kCompareVolatileRegisters=false`).

## Banco (FASE 3)

- Comparación EXE (hook apagado) frente a DLL (hook encendido) con `HookRoute`, 4096 casos, familia = n % 16: 0 típicos, 1 casi unitarios, 2 exponentes -30..30, 3 cancelación (b1, b2 proporcionales a a1, a2 con desvíos de 1 ulp), 4 extremos (enormes, diminutos, subnormales: desbordes del fstp y exponente extendido), 5 valores especiales, 6 NaN/infinitos/ceros mezclados, 7 ceros con signo y ±1, 8 bits crudos (nunca sNaN), 9 salida == a, 10 salida == b, 11 salida == a == b, 12 a == b, 13 solapamientos parciales, 14 desplazamientos arbitrarios, 15 NaN con cargas útiles y empates.
- Una arena `[guarda 0x40][ventana 0x60][guarda 0x40]` con relleno previo variado (aleatorio, ceros, 0xFF, NaN); a se escribe en `ao` y después b en `bo` (si se solapan gana b). Se compara la arena completa en bits y las guardas.
- Compuertas por lado: ESP, EBX/ESI/EDI/EBP, DF=0, ruta del hook, x87 normalizado (FCW, TOP, etiquetas, ST válidos; sin pila válida), globals (bytes de entrada y cuerpo) y estado x87 restaurados, `fp_difference` vacío. EAX/ECX/EDX sólo registrados. Sin colaboradores sustituidos (la función no llama a nada).
- `fixture_oracle.py`: modelo independiente del desensamblado (Fraction, x87 de 24 bits con exponente extendido, redondeo al par, `fstp` a float32 con desborde/subnormal, reglas de NaN del x87; comodín `??` sólo en empates de NaN con signo opuesto, donde se exige EXE == DLL). Autoprueba: identidad, i·j=k, i·i=-1, 3000 casos contra una cadena float32 por operación, 1400 casos de solapamiento contra una emulación secuencial sobre memoria compartida, NaN, 0·inf, desborde de exponente extendido.
- `run_local.py`: coberturas mínimas calibradas con `fixture_mirror.py` (≈50 % de margen). Resultado del espejo (sin juego): alias 1455, out==a 290, out==b 285, a==b 270, todos iguales 257, parcial 353, finito de entrada 3272, nan_in 574, inf_in 317, nan_out 690, inf_out 874 (624 desde entradas finitas), indefinido 201, ceros 930, -0 76, subnormal 65, desconocidos 127. `analyze` devuelve `None` si el registro no existe o no ha terminado.

## Suposiciones y riesgos

- El espejo Python no se ha comprobado contra el generador C++ (no se compiló): si algún mínimo falla por poco, comparar las filas `input` con el espejo antes de bajar umbrales.
- sNaN no cubierto (las fixtures sólo generan NaN silenciosos).
- `QuatMultiplyCpp` es `noinline` y `/arch:IA32` por archivo (como sus hermanas); `Commit()` deja el estado final x87. Si el banco muestra diferencias de estado x87 (ST muertos), revisar `FlcX87::maxDepth` para esta secuencia (máx. 3).
- La clase `CQuaternion` sigue con envolturas: `Set(x,y,z)`, `Slerp(t)`, `Conjugate`, `Get(float*,float*,float*)`.
