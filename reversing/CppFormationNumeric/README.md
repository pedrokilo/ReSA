<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppFormationNumeric — propuesta numérica offline sin aplicar

Migración de CFormation::FindNearestFreeDestination (0x69B1B0..0x69B230), 8192 pares previstos; psGetMemoryFunctions acompaña sin cambio con64, total8256. Banco derivado de DependenciesFormationMemory8 conserva íntegramente corpus y gates. No aplicar por subagente, compilar, ejecutar GTA ni registrar antes de medir dentro del juego.

## Cálculo propuesto

Fuente C++ normal, sin naked/asm ni delegación a cuerpos GAME o sqrtCRT. Helper de raíz binaria implementado con enteros uint64: normalizar mantisa24/exponente de binary32; construir radicando entero de hasta48bits; raíz entera por pares de bits; redondear con rad>q*q+q. La igualdad con un punto medio no ocurre para este radicando entero. Reconstruye el resultado binary32 correctamente redondeado al más cercano para entrada positiva finita. Se contempla zero/subnormal/infinite/quietNaN por representación; no genera el mismo estado de excepciones hardware que FSQRT.

Comprobación matemática offline:200007 entradas positivas finitas binary32, incluyendo zero/subnormales/minnormal/maxnormal, semilla69, contra una evaluación independiente de raíz y conversión final binary32. Esto NO es validación del juego ni comprobación de todo el código C++ compilado.

El orden EXE importa: restas X/Y/Z; cuadrados Z/Y, sumar Z²+Y²; cuadrado X; sumar +X²; FSQRT69B1FE. Cada operación propuesta fuerza storevolatilefloat para mantener un redondeo PC24, sin FMA. Constante86D270=10000000.0f, usada como mínimo inicial; noFLT_MAX. Igualdad y NaN no reemplazan mínimo: fcomp/testAH,5/JP69B20A..69B211 equivale a seleccionar sólo distancia<minimum. Se conserva primera coincidencia estricta; claimedBYTE nozero salta, count firmado no positivo salta; total suma mínimo incluso sin destino.

## Contrato y límites que no deben ocultarse

EAX int32 de retorno, pila/no volátiles, cost_bits exacto, contador0..24, listas/tablaRWreadonly, guardas y controlx87/TOP se comparan sin relajación. El banco impone PC24/RCnearest e incluye seis semillascost, empates, destinoexacto, todosreclamados, listasvacías/completas, distancia grande1e7 y qNaN7FC12345. Captura ninguna equivalencia universal de CW53/64 ni otrosmodosRC.

Los temporalesfloat32 no reproducen el rango de exponentes extendido de x87: entradas extremas con cuadrados subnormales/underflow u overflow pueden diferir antes de FSQRT. Los fixtures existentes tienen diferencias finitas acotadas y no ejercitan ese problema. La función software de raíz no modifica estado x87 como FSQRT; flags/FIP no son gates de este banco histórico. C++ propuesta es para medir corpus histórico; NO afirmar que resuelve todo x87 ni que cumple igualdad universal del EXE.

El EXE conserva ECX por PUSH/POP. Banco histórico comprueba no volátiles y resultado, no ese registro. Callers binarios deberán revisarse antes de atribuir seguridad fuera del corpus. Diferencias reales en la corrida se corregirán sin retirar cost_bits, NaN, TOP/controlword ni gates.

Archivos: source-proposal.patch y candidate.cpp.txt; LiveComparison.hpp/build_probe.py/run_local.py derivados independientes; sqrt-offline-check.txt matemática. Parchecheck y sintaxisPython correctos. Producción/banco histórico/documentos congelados/runtime_runner intactos.
