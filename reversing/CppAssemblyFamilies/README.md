<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Familias de ensamblador: análisis offline

Fuente actual e inventario coinciden: **298 naked + 951 tokens asm = 1249 apariciones**. Cero filas añadidas/eliminadas respecto al inventario congelado leído; no se lo modifica. inventory-family.json conserva por aparición ruta, línea, familia, firma, contexto completo y hash; también hash SHA256 de todos los archivos analizados. analyze_families.py reproduce el análisis en esta carpeta exclusivamente.

Se eliminan comentarios/literales manteniendo offsets; se balancean llaves. Para naked se clasifica su cuerpo completo y se atribuye la misma familia a los tokens asm internos. Para no-naked se clasifica cada bloque asm; una sentencia sin llaves es una unidad propia. Por ello «unidades de contexto» NO son funciones: muchas son bloques dentro del mismo método. No sumar naked y asm como funciones ni confundir una familia textual con equivalencia demostrada.

| Familia (heurística conservadora) | naked | asm | unidades de contexto |
|---|---:|---:|---:|
| absolute_game_or_crt_delegate | 89 | 118 | 89 |
| caller_register_bridge_candidate | 25 | 33 | 33 |
| fsqrt_with_context | 21 | 67 | 67 |
| integer_control_or_register_contract | 35 | 41 | 36 |
| integer_float_conversion_context | 36 | 138 | 138 |
| large_native_body_integer_or_mixed | 6 | 6 | 6 |
| other_transcendental_context | 20 | 93 | 93 |
| other_x87_arithmetic_or_compare | 23 | 269 | 269 |
| seh_or_unwind_special_abi | 9 | 10 | 9 |
| sin_cos_composite_or_store | 21 | 163 | 163 |
| sin_cos_simple_or_mul | 13 | 13 | 13 |

La prioridad de clasificación es delegación absoluta/SEH, sin-cos, FSQRT, int-float, trascendentales, resto x87, constantes, puentes, cuerpo largo y resto entero. Un cuerpo con FSQRT y FPATAN aparece en FSQRT; no significa que sea sqrt simple. Además **101 apariciones** tienen contexto grande (>250 tokens), bandera independiente is_large_native_context: los métodos nativos reales de miles de instrucciones suelen clasificarse por su FP dominante y NO caben todos en la fila large_native_body. No se encontraron retornos constantes aislados con la regla estricta: no inventar un lote.

## Familia inmediata más aprovechable

13 helpers naked (26 apariciones): diez del lote TrigSimple10 ya preparado y tres más: FollowPointRouteCos (TaskComplexFollowPointRoute.cpp), ClimbSinMul y ClimbCosMul (TaskSimpleClimb.cpp). Cuerpo FLD parámetro, FSIN/FCOS, opcional FMUL, RET. Son candidatos a las mismas primitivas TrigSimpleCpp, sujetas a codegen real, equivalencia de callers y ejecución de los bancos pertinentes. Se excluyó SubSinX87 de esta familia porque tiene FSUBR: su orden adicional exige otro contrato. No ejecutar bancos viejos debilitados ni suponer éxito por patrón.

## Familias posteriores

- FSQRT: 67 contextos /88 apariciones. Los cortos de Markers, WaterLevel, PedGroupPlacer, Vehicle y Pad mezclan sumas/productos, stores o branches. Los helpers Asin/Acos (PedIK, Dead, BulletInfo, Handling, Bmx, BreakObject) además contienen FPATAN y constante: requieren preservar orden, precisión y estado x87. Estudiar primero segmentos FLD;FSQRT sin stores y después consumidores, nunca convertir todos por simple std::sqrt(float).
- Int-float: 138 contextos /174 apariciones. Agrupa FILD/FISTP sin distinguir todavía signed/unsigned, ancho, CW/RC y retorno ftol. Caso Shelf demostró que /Od tempstore cambia C1 aun con resultado float idéntico. Separar FILD32, construcción unsigned64 y ftol/CRT antes de migrar.
- Sin-cos compuesto: 163 contextos /184 apariciones. Incluye sumas, divisiones, stores y rotaciones. Requiere contratos por secuencia, sin fusionar ni redondear intermedios.
- Puentes caller/reg: 33 contextos /58 apariciones. Registros especiales, vtables o stacks de caller no se expresan automáticamente en C++ estándar. Identificar firma real y banco ABI por consumidor; no helper asm sustituto ni ocultar ECX/EDX/EAX parcial.
- Enteros/control: 36 contextos /76 apariciones. Los tres getters Fight son candidatos cortos pero el banco verifica ECX/EDX y EAX parcial. No se admiten por mera equivalencia lógica.
- Delegación absoluta: 89 unidades /207 apariciones. Los push dirección;ret invocan GAME o CRT; retirar wrapper manteniendo llamada GAME no cumple reconstrucción independiente. Debe reconstruirse el callee y revalidar el consumidor antes de eliminarlo.
- SEH/unwind: 9 unidades /19 apariciones. Exigen rutas de excepción y ABI, no solo camino normal.

Este documento propone agrupación y prioridades, **no conversiones ni aprobación**. No producción, compilación, GTA ni Git. No tolerancias nuevas ni reducción de gates.
