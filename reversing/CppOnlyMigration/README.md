# Conversión del código de producción a C++ sin ensamblador

Orden del usuario del 10/10/2026: eliminar ensamblador en línea y declaraciones naked de source/game_sa conservando resultados exactos del EXE, sin tolerancias numéricas.

El inventario inicial identificó 185 archivos con 965 apariciones de ensamblador y 312 declaraciones naked, tras excluir comentarios y cadenas. Son apariciones léxicas, no funciones. [inventory.csv](inventory.csv) y [summary.json](summary.json) se regeneran con `python -B contrib/audit_cpp_assembly.py`; el análisis no sustituye un parser C++ ni una comprobación de ramas del preprocesador.

## Conversiones comprobadas

[approvals.json](approvals.json) conserva 34 conversiones de funciones fuente con sus auditorías dentro del juego:

| Función | Entrada EXE | Pares propios |
| --- | --- | ---: |
| FindSequenceElement | 0x421770 | 2048 |
| ClearInterestingVehicleList | 0x423F00 | 256 |
| GetAnimIdForPed | 0x61B460 | 2048 |
| Office_FurnishCenter | 0x599A30 | 4096 |
| Shop_FurnishCeiling | 0x59A130 | 2048 |
| RwTexDictionaryFindHashNamedTexture | 0x734E50 | 2048 |
| FurnishOffice | 0x599AF0 | 2048 |
| Shop_AddShelfInfo | 0x59A140 | 4096 |
| CanSunbathe | 0x632140 | 2048 |
| Get2dEffectStore | 0x4C5A60 | 1024 |
| CloudsSinX87 / CloudsCosX87 / CloudsSinMulX87 | Helpers fuente, tres consumidores | 3072 compartidos |
| BirdsSinMul / BirdsCosMul | Helpers fuente, CBirds::Render | 512 compartidos |
| BrightSinMulX87 / BrightCosMulX87 / PlaneTrailsSinMulX87 / PlaneTrailsCosMulX87 | Helpers fuente, dos consumidores | 1536 compartidos |
| JpMulSinX87 / JpMulCosX87 | Helpers fuente, ProcessPed y RenderJetPack | 1536 compartidos |
| SinX87 / CosX87 de Swim | Helpers fuente, ProcessControlInput | 2048 compartidos |
| FollowPointRouteCosX87 | Helper fuente, CalcBlendRatio | 512 |
| ClimbSinMulX87 / ClimbCosMulX87 | Helpers fuente, seis consumidores | 4608 compartidos |
| 3dMarkersSinMulAdd | Helper fuente, dos consumidores | 4608 compartidos |
| BirdsSinMulAdd / BirdsCosMulAdd / BirdsNegSinDiv | Helpers fuente, tres consumidores | 2560 compartidos |
| FollowNodeRouteCosHalfX87 | Helper fuente, CalcBlendRatio | 1024 |
| SwimSubSinX87 | Helper fuente, SwimmingResistance | 1024 |
| SetUseD3DResourceBuffering | 0x730AC0 | 512 |
| Native_00424100 | 0x424100 | 2048 |

Cada corrida aprobada tiene cero diferencias, resultados no nulos, rutas y restauración verificadas. Estas conversiones vuelven a probar implementaciones catalogadas; no incrementan las 105 aprobaciones de la cohorte de dependencias.

El lote IntegerMinimal2 paso 8704 pares compartidos: 4608 en World4 y 4096 en World10. Ambos cuerpos completos coinciden con el EXE, sin relocaciones. Se comprobaron argumentos fisicos, EAX/ECX/EDX y banderas definidas; AF de XOR es indefinido y no se exige. Partidas, configuracion y EXE se verificaron por hash. El trabajo se detuvo por orden del usuario tras este lote el 11/10/2026. La lista completa de los 177 archivos pendientes esta en [FILES.md](FILES.md), con CSV agrupado en [files.csv](files.csv) y lineas individuales en [inventory.csv](inventory.csv).

El corte aprobado tras TrigCompound6 contiene 930 apariciones de ensamblador y 277 declaraciones naked en 177 archivos. Los tres helpers de nubes comparten una campaña de 3072 pares sobre sus consumidores, sin contar tres veces esos pares ni añadir entradas del EXE. La regresión de 20 métodos de nubes pasó otros 8384 pares. Los diez helpers adicionales comparten 5632 pares en seis consumidores; no se multiplican por helper. Consultar el inventario y el registro de conversiones por separado. La migración completa sigue pendiente.

FindSequenceElement usa aritmética modular de 32 bits, lectura del byte del flag y resto firmado; excluye división por cero y INT_MIN/-1 como el IDIV original. Dos intentos de ClearInterestingVehicleList se rechazaron por alterar ECX/EDX; la versión final escribe directamente los dos slots de vehículo y preserva esos registros en la compilación probada.

InteriorCpp.cpp se compila con símbolos /Zi y sin PCH/unity: /ZI insertaba una reserva de pila que cambiaba las banderas de la función vacía. El banco de interiores instrumenta DeskQuad y acredita el cuerpo convertido, sin validación transitiva de ese colaborador. La prueba de texturas usa diccionarios y colaboradores RW reales, sin stubs. Su corrida anterior todavía usaba ensamblador y no cuenta como conversión; la aprobada es 20261010-235425-798187.

Los cálculos x87 y las envolturas que preservan registros necesitan soluciones específicas. El núcleo entero experimental de conversión flotante pasó 128976 casos offline; todavía no tiene aprobación dentro de GTA. Las propuestas de memoria y otros lotes siguen pendientes hasta demostrar su equivalencia en el juego.

No mover ensamblador a otro archivo de producción, escribir opcodes en un arreglo ni delegar el cuerpo al EXE para fingir una conversión. Los instrumentos de prueba quedan separados del objetivo de producción. No se reescriben hashes ni pruebas históricas para atribuirles la fuente nueva.

## Estudio de generación de instrucciones

El 11/10 se compiló compiler-math-study.cpp con MSVC 19.51 x86, /O2 /Oi /arch:IA32 /fp:fast. La función C++ `float(sin(x))` generó FLD, FSIN, RET, y `float(sqrt(x))` generó FLD, FSQRT, RET, sin almacenamiento float32 intermedio. sqrtf no estaba disponible como intrínseca en este estudio. Esto permite investigar conversiones que conservan operaciones x87 mediante generación del compilador, con fuente íntegramente C++. No demuestra equivalencia de expresiones mayores ni prueba dentro del juego: hay que inspeccionar cada objeto y contrastar operaciones, ABI, precisión y estado. /fp:fast no se aplica globalmente ni justifica tolerancias.

El segundo lote de interiores pasó los 6144 pares de 20261011-000453-501774 después de rechazar dos corridas con 54 diferencias C1. Shelf se compila en una unidad separada con opciones que producen FILD/FSTP directos de los argumentos. Su banco compara CW, SW completo, tag, registros80 vivos y MXCSR, conserva los 108 bytes raw y verifica restauración física. FIP/FDP de código/datos reubicados se conservan como evidencia sin exigir que las direcciones del EXE y la DLL sean iguales. Alcance: PC24/nearest y colaboradores instrumentados, sin aprobación transitiva. La regresión de las dos funciones anteriores está en un banco separado.

La regresión CppInteriorRegression1 20261011-000903-597439 repitió 6144 pares de Office_FurnishCenter y Shop_FurnishCeiling sobre las opciones finales, cero diferencias y cero registros nulos, con restauración verificada. Conserva la evidencia anterior y añade referencia de regresión en approvals.json.

Get2dEffectStore pasó 1024 pares propios más 1024 de ScratchPad; la regresión de sus otros 28 métodos pasó 5264 pares. /ZI añadía SUB ESP que cambiaba flags; /Zi en ModelInfo.cpp conserva el PCH y evita esa reserva. Se verificó PUSH/MOV EBP, MOV EAX constante, POP EBP y RET, sin alteraciones de flags/ECX/EDX/NV/x87. No se atribuyen esas garantías integrales a los otros 28 métodos.

TrigSimple10 conserva las operaciones FLD/FSIN o FCOS/FMUL cuando corresponde, sin temporales ni llamadas CRT. Ocho cuerpos consumidores, incluidas las lambdas de render, mantienen instrucciones y relocations salvo los destinos autorizados de los helpers. Un fallo del lector COFF se corrigió: .bss tiene payload ausente, no bytes del encabezado. Sus inicializadores de referencias también se compararon. Las cuatro campañas pasaron 5632 pares exactos; 459 casos de aves emitieron flush después de generar geometría y 74 casos de jetpack con ThrustStop alcanzaron ambos frames de sus jetballs. La cobertura se infiere del flujo fuente y las trazas medidas. Estos bancos conservan contratos históricos de resultados, memoria y trazas; no añaden una medición completa de FP108/EFLAGS/registros a cada consumidor. Evidencias en [CppTrigCoffAudit](../CppTrigCoffAudit/README.md).

Memoria superó las ocho calibraciones diagnósticas tras corregir la copia del contexto en app_debug.cpp; sigue pendiente la campaña completa sin logger y MemoryMgr.cpp permanece sin convertir. TrigCompound6 ya fue integrado y revalidado: seis helpers, 9216 pares compartidos en cuatro campañas, cero diferencias y restauración verificada. La cobertura medida incluye 311 casos de actualización de aves, 459 casos de geometría renderizada y 24 casos de la rama SubSin de natación. Los consumidores conservan sus contratos históricos; no se atribuye medición integral de FP108 o registros a cada uno.

TrigExtra3 aprobado: FollowPointRoute pasó 512 pares y Climb 4608, cero diferencias, sin resultados nulos y restauración verificada. Quince cuerpos COFF conservan instrucciones y relocations salvo los tres destinos autorizados. El primer banco Climb alcanzaba entidades externas: se aislaron todas las cabezas de sectores 120x120 y repeat 16x16, con restauración y guardia de ámbito; la campaña final no alcanzó entidades externas. Se acreditaron 51 casos de cosine en rutas y ocho casos de seno/coseno en escalada mediante inferencia de flujo y trazas. Evidencias en CppTrigFollowPointRoute/runs y CppTrigClimb/runs.
