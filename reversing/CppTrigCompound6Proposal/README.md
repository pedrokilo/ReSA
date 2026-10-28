<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Seis helpers x87 compuestos cortos: propuesta offline

Pendiente de objeto real y juego. No aplicada, compilada, ejecutada ni aprobada. Se localizaron seis naked actuales, con un bloque asm cada uno: doce apariciones candidatas. Los diez helpers simples del lote anterior ya no estan en estos cuerpos; no se reutiliza inventario congelado para contarlos.

| Helper actual / firma float cdecl | Instrucciones exactas actuales | Consumidores / bancos |
|---|---|---|
| SinMulAddX87(angle,k,mult,add), 3dMarkers.cpp | FLD [esp+4]; FMUL [esp+8]; FSIN; FMUL [esp+12]; FADD [esp+16]; RET | PlaceMarker725120 / Markers4096; tUser3dMarker::Render7232BF consumido User3dMarkersDraw723240 / Markers512 |
| BirdsSinMulAdd(angle,mult,add) | FLD [esp+4]; FSIN; FMUL [esp+8]; FADD [esp+12]; RET | CreateNumberOfBirds711EF0 / Misc6 1024; Render712810 / Misc6 512 |
| BirdsCosMulAdd(angle,mult,add) | FLD [esp+4]; FCOS; FMUL [esp+8]; FADD [esp+12]; RET | CreateNumberOfBirds711EF0 / Misc6 1024 |
| BirdsNegSinDiv(angle,div) | FLD [esp+4]; FSIN; FCHS; FDIV [esp+8]; RET | Render712810 / Misc6 512 |
| FollowNodeRouteCosHalfX87(angle,half) | FLD [esp+4]; FCOS; FMUL [esp+8]; FADD [esp+8]; RET | CalcBlendRatio66EDC0 / Route19 1024 |
| SubSinX87(z,angle), TaskSimpleSwim.cpp | FLD [esp+8]; FSIN; FSUBR [esp+4]; RET | ProcessSwimmingResistance68A1D0 / Mix11 1024 |

Todos reciben solo float, retornan ST0 y no tienen llamadas GAME, stores intermedios, conversiones unsigned, ftol, atan/sqrt ni registros especiales. Estas secuencias pertenecen a helpers DLL actuales, no entradas independientes del EXE. Direcciones de consumidores obtenidas de fuente/tabla historica, no son aprobacion nueva. El rango candidato tUser3dMarker7232BF exige revisar limite real antes de tratarlo como entrada propia; se conserva consumidor catalogado User3dMarkersDraw.

## Propuesta concreta

TrigCompoundCpp.cpp/.h ofrecen seis cuerpos C++ double equivalentes algebraicamente. TU propio sin PCH/Unity; opciones /O2 /Oi /arch:IA32 /fp:fast y /Zi en Debug, runtime checks desactivados, siguiendo patron CloudsMathCpp. No cambiar opciones de consumidores. Retirar los seis cuerpos antiguos y reemplazar llamadas por funciones propias; no aplicar automaticamente. Mantener caller codegen old/new exacto fuera de destinos calls permitidos, incluyendo lambdas/consumidores auxiliares.

FollowNodeRouteCosHalf requiere evitar factoring a half*(cos+1), que altera orden/redondeo. Se propone pragma float_control(precise,on,push) solo para ese cuerpo, pop despues. Ese pragma puede cambiar disponibilidad de intrinseco o emitir CRT: comprobar objeto, no asumirlo. Si emite CRT, estudiar alternativa sin factoring antes de aprobar.

## Obstaculos por comprobar

Escalado del argumento en Markers debe permanecer x87 sin store double/float antes FSIN. No aceptar recomputacion redondeada del argumento. Variantes MulAdd deben emitir FMUL y despues FADD en orden, sin FMA/reasociacion ni stores intermedios. NegSinDiv exige FCHS antes FDIV, no despues. SubSin exige FSUBR z despues FSIN (no intercambiar orden/signos). CosHalf debe emitir FCOS,FMUL half,FADD half. Todos requieren mismo FLD float y ST0 final, sin temporal float32 o double ni prologo que modifique registros/flags adicionales.

PC24 por instrucciones y rango de exponentes x87 importan: expresion double almacenada o con PC64 no acredita igualdad universal. NaN, infinito, ceros firmados, divisor cero, subnormal, flags/FIP y angulos fuera de rango FSIN/FCOS requieren contratos y banco estricto; los bancos historicos de snapshots no los certifican automaticamente. Helpers naked originales no modifican registros enteros ni flagsCPU; evitar instrumentacion del compilador.

Conteos de pares son disponibilidad historica, no cobertura dinamica demostrada. Debe confirmar ramas consumidoras: aves proyectadas/generacion biome y WingStillness; markers tipos/oscilacion; slowdown/speedup ruta; rama resistencia swim. Copiar bancos a nuevas carpetas sin alterar indices, semillas, fixtures ni gates, exportando snapshots completos y sin passingNULL. No reducir gates si source/codegen difieren.

## Alcance de esta entrega

Archivos son propuesta y tabla de evidencia fuente; no build ni GTA ni Git ni produccion. No se inventan creditos de validacion. Los asm mas grandes, TimeCycle con stores de salida, sinMul simple restante Climb y cos simple FollowPoint quedan fuera de este lote compuesto.
