<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Alternativa sqrt intrínseca propia — sin aplicar ni compilar

Host confirmado por coordinador: float(sqrt(x)) con /O2 /Oi /arch:IA32 /fp:fast produce FLD, FSQRT, RET; sqrtf produce llamadaCRT. Por ello candidate-intrinsic.cpp.txt usa ::sqrt(double), pragma intrinsic(sqrt), funciones C++ normales noinline y ninguna llamada al GAME/CRT en la raíz cuando el codegen se confirme. host-intrinsic-study.cpp facilita verificar ADD/SUB/MUL/ROOT/LESS/STORE sin modificar el repositorio fuente.

## Cambio respecto a raíz software

FSQRT propio conserva hardware de raíz y sus excepciones. Los temporales aritméticos son volatileDOUBLE: en x87 PC24 cada suma/resta/producto ya ha redondeado mantisa24; almacenar en binary64 no añade otro redondeo y conserva rango de exponentes. Diferencias float32 máximas tienen exponente como mucho129; cuadrados258; suma260; mínimos positivos cuadrados−298. Todos caben sin over/underflow en double. Esto corrige la restricción indebida de storesfloat32 intermedios de la propuesta software.

Orden: ΔX,ΔY,ΔZ; Z²,Y²; sumar Z²+Y²; X²; sumar+X²; FSQRT. Se realiza storeFLOAT de la distancia antes de comparación pero se compara distancia extendida con mínimo; sólo si gana se recarga distanciaFLOAT como nuevo mínimo. Así se conserva el orden FST-single/Fcomp y casos de overflow al almacenar una distancia enorme antes de rechazarla. Acumulación final en x87 PC24 seguida de storeFLOAT.

## Revisiones obligatorias antes de ejecutar

No basta el estudio genérico: inspeccionar objeto final Formation.cpp, raíz FSQRT sin CALL CRT ni SQRTSS/SQRTSD, helpers FLD/FADD/FSUB/FMUL y storesDOUBLE; ausencia FMA/reasociación; operandos Δpoint−origin y Z²+Y²+X²; comparación ordenada strict< que excluya igualdad/NaN; storeFLOAT temporal emitido incluso cuando pierde. /fp:fast por sí solo no demuestra ausencia de cambios. Helpers noinline y volatile forman barreras, pero igualmente inspeccionar código final.

/O2 /Oi /arch:IA32 /fp:fast selectivos por TU afectan otras funciones Formation.cpp: no aplicarlos globalmente ni asumir que todas las demás quedan validadas. Si se extrae helperTU propio, incorporarlo a build/manifiesto y auditar conversión de entrada/salida en mainTU. Estado CWPC24 debe permanecer real: esta propuesta no fuerza controlword ni disimula diferencias. PC64 no queda demostrado porque storesDOUBLE redondearían53, y la equivalencia FIP/C0/C2/C3 tampoco se afirma. ECX preservado por original no está gateado por banco y requiere auditoría de callers para ampliar alcance.

Los tres instrumentos del banco existente CppFormationNumeric permanecen intactos; 8256 pares y todos gates. Alternate patch aplicable sobre naked actual y futuro estudio requieren coordinador. No build/GTA hecho por subagente.
