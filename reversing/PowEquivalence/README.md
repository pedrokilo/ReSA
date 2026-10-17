<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PowEquivalence: pow de la DLL frente a `_CIpow` del EXE (12/10/2026)

Cierra el límite que dejaban `PowScaleVector3Cpp` (`CBouncingPanel::ProcessPanel`, lote de nueve archivos) y `PowDecaySpeedCpp` (`CTaskSimpleCarJumpOut::ProcessPed`, lote de trece): usaban el `pow` de la CRT estática de la DLL (`__CIpow`) en lugar del `_CIpow` del EXE (0x822130), y sólo se había visto que coincidían en los pares de sus bancos.

## Análisis estático

- **Mismo algoritmo.** En una máquina con SSE2 y las excepciones de coma flotante enmascaradas, `_CIpow` del EXE salta a 0x829B80 y `__CIpow` de la DLL a `__pow_pentium4`: es la misma rutina de LIBM de Intel. Se comparó el grafo completo de bloques básicos alcanzables desde la entrada (644 instrucciones en el EXE, 637 en la DLL, unos 80 bloques cada uno): **76 bloques son idénticos instrucción a instrucción**, incluida toda la ruta normal. Los que difieren son: el bloque que decide `1` o NaN cuando el exponente es ±0 (el EXE comprueba antes si la base es NaN), los dos bloques que producen ±infinito en el desbordamiento (el EXE carga el infinito; la DLL multiplica el máximo por sí mismo) y las llamadas a los gestores de error, que sólo cambian de destino.
- **Mismas tablas.** Se compararon byte a byte `rcp_t1`, `log2_t1`, `rcp_t2`, `log2_t2`, `rcp_t3`, `log2_t3` (0x410 a 0x1010 bytes), la tabla de exponenciales (0x800 bytes), `clv` y las constantes de la ruta normal: idénticas. Sólo difieren las dos constantes del bloque de desbordamiento.
- **Misma condición de despacho.** Las dos comprueban la bandera SSE2 de su CRT y que las excepciones de MXCSR y del control x87 estén enmascaradas (la DLL exige además redondeo al más cercano, que es el estado normal del juego).

Consecuencia: para cualquier entrada que no pase por los bloques que difieren, EXE y DLL ejecutan exactamente las mismas instrucciones sobre los mismos datos, así que el resultado es idéntico por construcción; los bloques que difieren se activan sólo con entradas especiales (exponente ±0 con base NaN, base ±1 con exponente NaN/±infinito, y resultados que desbordan), que son las que cubren la rejilla de especiales y los barridos por regiones.

## Una diferencia real y su corrección

La primera corrida del banco (`reversing/CppPowEquivalence/runs/PowEquivalence-20261012-102304-169262`, fallo) encontró un caso de `PowScaleVector3Cpp`: `damp` = NaN con `paso` = 0. El EXE devuelve NaN y la DLL 1 (regla de C99). Una rejilla exploratoria de 28224 pares de valores especiales (±0, subnormales, ±1, ±infinito, NaN silenciosos y de señalización, extremos) mostró que **sólo hay tres casos de diferencia de valor**, 34 pares en total, y que ni los negativos, ni los ±0/±infinito, ni los subnormales difieren:

| Entrada | EXE | DLL |
| --- | --- | --- |
| ±1 elevado a NaN | ese NaN silenciado | 1 |
| ±1 elevado a ±infinito | NaN indefinido (0xFFF8000000000000) | 1 |
| NaN elevado a ±0 | la base NaN silenciada | 1 |

`X87Math` ofrece ahora `ExeCompatiblePowCpp`, que reproduce esos casos del EXE y delega todo lo demás en `pow`; los dos helpers lo usan.

## Resultado

Banco PowEquivalence con valores **y banderas** (`reversing/CppPowEquivalence2/runs/PowEquivalence-20261012-123641-067229`), 6824 trozos, todos con valores idénticos bit a bit:

| Prueba | Casos |
| --- | ---: |
| Rejilla de valores especiales (double) frente a `_CIpow` | 28224 |
| `pow(0.96f, x)` para **todos** los 2^32 float `x` (dominio completo de `CTaskSimpleCarJumpOut`) | 4294967296 |
| Regiones: las 512 × 512 combinaciones (signo, exponente binario) de damp y de paso, con 2048 pares de mantisas pseudoaleatorias y extremas cada una | 536870912 |
| `PowScaleVector3Cpp` frente a la secuencia x87 original, damp, paso y vector aleatorios (mitad en el dominio real, mitad patrones de bits cualesquiera) | 67108864 |
| `PowDecaySpeedCpp` frente a la secuencia x87 original, igual | 67108864 |

Los consumidores se repitieron con la fuente final: STasks10 (19136 pares) y World52 (18384), cero diferencias. La primera corrida con banderas (`reversing/CppPowEquivalence2/runs/PowEquivalence-20261012-115733-460642`) falló sólo por las diferencias de banderas descritas más abajo, antes de que el banco las clasificara; los valores fueron idénticos.

## Los tres límites que quedaban

1. **Dominio completo de `PowScaleVector3Cpp` (2^64 pares).** No se enumera, y no hace falta para la conclusión: la identidad de 76 de 80 bloques y de todas las tablas hace idénticas las entradas que no pasan por los bloques que difieren, y éstos se activan sólo con entradas especiales, cubiertas por la rejilla y por el barrido por regiones (todas las combinaciones de signo y exponente binario de las dos entradas, 536 870 912 casos, sin diferencias de valor). Para la base fija de `PowDecaySpeedCpp` el dominio sí está enumerado entero (2^32).
2. **Rama no SSE2.** No aplica: la DLL entera se compila con SSE2 (no hay `/arch` global; los objetos de las tareas contienen instrucciones SSE) y no arranca en una CPU sin él, así que la ruta x87 de `pow` nunca puede elegirse en esta DLL. El banco lo comprueba en el juego antes de cada corrida: bandera SSE2 del EXE (0xC9C400) = 1, bandera de la CRT de la DLL = 1, MXCSR = 0x9FB7 (excepciones enmascaradas) y control x87 = 0x7F: las dos usan la ruta SSE2 (`sse2_path_selected_by_both: true`).
3. **Banderas de excepción.** Ahora se comparan en todas las pruebas de `pow` (MXCSR y estado x87 tras cada llamada). Los valores son siempre idénticos; las banderas coinciden salvo en dos casos, ambos por los bloques que difieren: (a) resultado que desborda a ±infinito con entradas finitas: la DLL lo calcula como máximo·máximo y enciende además la bandera de desbordamiento (0x08) y la de inexacto (0x20), que el EXE no enciende o sólo en parte (los demás bits coinciden); (b) NaN elevado a ±0: el EXE recorre el algoritmo y deja inexacto (0x20), `ExeCompatiblePowCpp` devuelve la base sin recorrerlo. El banco acepta únicamente estas dos clases, exigiendo valor idéntico y que el resto de las banderas coincida, y las cuenta: 1 066 797 634 casos de la clase (a) y 8 de la (b) en total. Cualquier otra diferencia de banderas haría fallar el banco. **No se igualan** porque exigiría reproducir el algoritmo del EXE; no tienen efecto observable: son acumulativas y con todas las excepciones enmascaradas nada las lee: el código del juego no toca MXCSR (en el EXE sólo hay 9 `stmxcsr` y 2 `ldmxcsr`, todos en rutinas de la CRT) y el juego ya corre con banderas acumuladas encendidas (MXCSR = 0x9FB7 en la comprobación de entorno).

## Límites

- La conclusión sobre el dominio completo de `PowScaleVector3Cpp` se apoya en la identidad de código y tablas más cobertura por regiones, no en una enumeración de los 2^64 pares.
- Las banderas de excepción de coma flotante de los dos casos descritos siguen siendo distintas del EXE; el argumento de que son inobservables es de análisis (ausencia de lectores en el EXE y estado del juego), no una prueba exhaustiva de todo el código de la DLL.
- La interacción con una CPU sin SSE2 se descarta por construcción de la DLL, no se ha ejecutado.
- Tras ejecutar el banco no queda estado modificado: no toca memoria del juego salvo el registro de resultados.
