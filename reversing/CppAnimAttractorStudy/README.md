<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Animación y atractores: ensamblador a C++ (11/10/2026)

Conversión de las cuatro funciones con ensamblador de `AnimBlendNode.cpp`, `AnimBlendSequence.cpp`, `PedShelterAttractor.cpp` y `AttractorScanner.cpp`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md).

## Qué se hizo

| Función | Clase / archivo | Solución |
| --- | --- | --- |
| `CalcTheta` (0x4D00E0) | `CAnimBlendNode` | `AnimCalcThetaCpp` en `X87Math.cpp`. Reproduce la `naked` `CalcThetaX87`: producto escalar en el orden del EXE guardado como float y recortado a 1 (con NaN no recorta), `acos` como `fpatan(sqrt((1+x)(1−x)), x)` con la palabra de control `(anterior & 0x300) \| 0x7F` (con `_control87`, restaurada después) y `1/fsin` del acos sin redondear (0 si es exactamente 0). El compilador genera `fsqrt`, `fpatan` y `fsin` en línea. |
| `RemoveQuaternionFlips` (0x4D1190) | `CAnimBlendSequence` | `AnimQuaternionDotNegativeCpp` en `X87Math.cpp` (producto escalar < 0, ordenado: con NaN da false). |
| `GetDisplacement` (0x5EF420) | `CPedShelterAttractor` | Usa `TrigSimpleCosMulCpp` y `TrigSimpleSinMulCpp`, que ya existían: son exactamente `fcos`/`fsin` por el radio y un solo redondeo a float. |
| `Clear` (0x5FFF90) | `CAttractorScanner` | `AttractorScanner.cpp` completo con `/O2 /Oi /arch:IA32 /fp:fast /Zi`, sin PCH ni unity. |

`AnimBlendNode.cpp`, `AnimBlendSequence.cpp` y `PedShelterAttractor.cpp` conservan sus opciones; sólo llaman a los helpers compartidos de `X87Math`.

## Dos problemas que se resolvieron

1. **Producto escalar reordenado y redondeado.** Con `/fp:fast` el compilador reasocia las cuatro sumas ((x+y)+(z+w) en lugar de ((w+z)+y)+x) y cambia el redondeo. Se calcula en una función con `#pragma float_control(precise, on)`, pero entonces, al devolver `float`, la guarda y recarga como float: un producto diminuto negativo (de dos quaterniones con componentes casi nulas) se redondeaba a −0 y dejaba de ser `< 0`, mientras que el EXE compara el valor sin redondear. La primera corrida de Render14 falló 21/512 por esto. La función devuelve ahora `double`, que conserva siempre el signo.
2. **Restos de registros de `Clear`.** El EXE sale de `Clear` con `EAX = this+0x90` y `EDX = 10` (restos del bucle) y sin tocar ECX; su llamador en 0x6034A7 hace `mov eax, ecx` justo después de la llamada, de modo que ECX es parte del contrato. El banco DependenciesTaskPed5 compara EAX, ECX y EDX; una primera versión `void` falló 2048/2048 en EAX y EDX. `Clear` devuelve ahora un `uint64` (EDX:EAX) con `(10 << 32) \| &field_68[10]` y el objeto real termina con `lea eax,[ecx+90h]` y `edx = 10`, sin escribir ECX. Los llamadores en C++ ignoran el valor.

## Comprobaciones en los objetos reales

- `AttractorScanner.obj`: ECX sólo se usa como base de las escrituras; los cuadrados `fld [0x86C52C]; fmul st,st; fstp` se calculan en las posiciones 4 y 7 y la pila x87 queda equilibrada. El compilador no calcula el cuadrado en las otras ocho posiciones (el original sí, y lo sobrescribe): mismo valor y mismas banderas sticky, porque son la misma operación sobre el mismo dato.
- `X87Math.obj`: `AnimOrderedQuaternionDot` hace los productos y sumas en el orden del EXE y devuelve en ST0 sin guardar; `AnimCalcThetaCpp` no llama a la CRT de coma flotante (sólo `_control87`).
- Diferencia conocida: el EXE usa `FCOM` en la comparación con cero de theta y el compilador `FUCOMP`; sólo cambian las banderas sticky de excepción con un NaN silencioso. Los bancos no lo detectan.

## Revalidación

Los bancos históricos completos se repitieron sobre la fuente final con `herramientas/rerun_hist.py`; las evidencias (`summary.json`, `RESULTADO.txt` y el registro comprimido) están en `reversing/CppAnimAttractorUnified/runs/`. Las dos corridas fallidas intermedias se conservan como historia.

| Banco | Función(es) | Pares | Resultado |
| --- | --- | ---: | --- |
| Anim16 (20261011-220528-470951) | `CalcTheta` y las otras 15 del banco | 14080 | aprobado |
| Render14 (20261011-215948-136227) | `RemoveQuaternionFlips` y las otras del banco | 6784 | aprobado |
| CTasks61c (20261011-220713-933257) | `GetDisplacement` y el resto de la lista 61c | 28928 | aprobado |
| DependenciesTaskPed5 (20261011-220112-315950) | `Clear` y otras tres | 8192 | aprobado |

## Límites

No hay cobertura exhaustiva de x87 más allá de los fixtures de cada banco, ni de NaN e infinitos fuera de ellos. Estos bancos no tienen auditor propio: la comprobación es la del analizador de cada banco y el resumen de la corrida. Los colaboradores no se validan por probar a sus consumidores.

## Objetos «antes» y «después» (no se publican)

| Objeto | SHA256 |
| --- | --- |
| AnimBlendNode.actual-before.obj | 2f2fcec2aeb5632e2ea2e31cdd645f1b7b61bcf83ea9ccb757447fbf537839d3 |
| AnimBlendSequence.actual-before.obj | 99a417fd05b8d997f238cdd8f5e69d6bbd75226709c7ccd6f63720d1c2d54273 |
| PedShelterAttractor.actual-before.obj | 1c755822f7ab71c3eda3a4dfb82c752399837527e139cce38e702f3a6c025d57 |
| AttractorScanner.actual-before.obj | a5eff6985bb12bd67f392f35fd75982f44667fa8e990239431f7cf87d14b02c8 |
| AttractorScanner.actual-after.obj | 34b0ecb2885d7abd718469f287528a62a6a7a48af1f5f3d58dff39bc8a4d7d5b |
| X87Math.actual-after.obj | 3b785dbfa49cc7b2dce7c900f82bd14f342d596b8ab850f47a8c544809fb16ee |
