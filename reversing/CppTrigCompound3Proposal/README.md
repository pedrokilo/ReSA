<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tres variantes seleccionadas: pendiente de admision

El estudio codegen-study.json del lote de seis produjo estas tres secuencias exactas: ScaledSinMulAdd FLD/FMUL/FSIN/FMUL/FADD/RET; SinMulAdd FLD/FSIN/FMUL/FADD/RET; CosMulAdd FLD/FCOS/FMUL/FADD/RET. Es evidencia del estudio host; quedan pendientes el build real y el juego.

Las otras tres variantes siguen rechazadas: NegSinDiv mueve FCHS despues de FDIV; CosHalf llama al CRT y agrega stores; SubSin usa FSUBP y dos slots de FPU.

source-proposal.patch solo sustituye SinMulAddX87 de Markers y BirdsSinMulAdd/BirdsCosMulAdd: tres naked y tres bloques asm. CMake configura un TU nuevo sin PCH ni Unity, con /O2 /Oi /arch:IA32 /fp:fast y /Zi, sin RTC. Las opciones de los consumidores permanecen iguales.

Antes de aplicar, el coordinador debe guardar los objetos antiguos y comparar PlaceMarker, tUser3dMarker::Render, CreateNumberOfBirds y Birds::Render, incluidas lambdas o colaboradores que contengan las llamadas. Fuera de los destinos autorizados se exige equivalencia de bytes y relocations. El helper final debe verificarse con las opciones reales, incluido el prologo EBP de /Oy-.

Los bancos independientes son CppTrigCompoundMarkers (4608 pares previstos) y CppTrigCompoundBirds (1536), con indices, semillas y comprobaciones historicas, snapshots completos y auditoria nested. No conceden garantia adicional sobre todo el status x87 por comparar snapshots.

Propuesta offline: sin cambios de produccion, compilacion, GTA o Git.
