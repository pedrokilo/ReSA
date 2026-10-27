<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tres helpers de CClouds propuestos en C++

Estado: APROBADO. La corrida 20261011-004646-091089 pasó 3072 pares sobre tres consumidores, cero diferencias, cero resultados nulos y restauración verificada. Auditor independiente en audit/audit_run.py y evidencia archivada en runs/20261011-004646-091089. La regresión CppCloudsRegression20 pasó 8384 pares sobre 20 métodos. Registro separado en CppOnlyMigration/approvals.json; no añade entradas del EXE ni funciones a la cohorte.

Intento rechazado 20261011-002232-430207: Update sin diferencias, RenderSkyPolys con 713 y MovingFogRender con 890. Sólo difería SW x87 (C0/C3); valores, memoria y trazas coincidían. Partidas y configuración restauradas.

La inspección inicial de CloudsMathCpp.obj está en codegen-proof.json; la final está en codegen-proof-final.json: FSIN/FCOS y FMUL dword, sin almacenamiento intermedio ni llamadas trigonométricas al CRT. Las comparaciones SSE se sustituyeron por generación x87 en Clouds.cpp con /arch:IA32 y sin PCH. Inicialmente fallaron C4651 y LNK2005; las dependencias explícitas y una unidad separada de acceso resolvieron el enlazado. Condiciones equivalentes y helpers de comparación con precisión local fijan el orden de operandos del EXE. No se relajó ninguna comparación del banco.

`source-proposal.patch` extrae únicamente CloudsSinX87, CloudsCosX87 y CloudsSinMulX87 a la TU CloudsMathCpp.cpp, con declaraciones externas CloudsMathCpp.h. En la propuesta inicial los consumidores permanecían intactos; la versión aprobada corrige además sus comparaciones x87. Los restantes helpers asm siguen pendientes. La TU usa /O2 /Oi /arch:IA32 /fp:fast /Zi sin PCH ni unity; BasicRuntimeChecks=Default evita la incompatibilidad /O2 /RTC1. Todos los atributos noinline se aplican a funciones C++ normales, ninguna naked. No se llama a cuerpos originales del juego ni se emiten opcodes.

La evidencia host `../CppOnlyMigration/compiler-math-study.cpp` demuestra la ruta de generación para sin(double) y sqrt(double). La propuesta inicial de cos(double) y el producto requería inspección propia del objeto, ya realizada en codegen-proof-final.json; no se supone automáticamente que std::sin reproduzca FSIN.

## Comprobación obligatoria del código generado

Antes de ejecutar el banco, inspeccionar CloudsMathCpp.obj y la imagen final: Sin debe hacer FLD angle → FSIN → RET; Cos FLD angle → FCOS → RET; SinMul FLD angle → FSIN → FMUL k → RET, dejando ST0 extendido y sin FSTP/FLD intermedio. No deben aparecer calls al CRT sin/cos, SSE para el producto ni una sustitución de FSIN/FCOS por aproximaciones. Un operando k materializado en double sólo puede admitirse después de demostrar su representación exacta y estado numérico; el objetivo preferido es FMUL dword como el EXE. Sin esta prueba, la propuesta no está lista para validación.

## Evidencia original y consumidores

Los tres dis de consumidores y el dis de CPlaceable::GetRight conservan direcciones e instrucciones del EXE canónico, obtenidas con dumpbin como reversing/_handoff/dis.sh:

| Consumidor | Entrada | Sitios x87 relevantes | Pares |
|---|---|---|---:|
| CClouds::Update | 0x712FF0 | FSIN 0x713004, seguido de multiplicación por viento | 1024 |
| CClouds::RenderSkyPolys | 0x714650 | FSIN 0x714681; FCOS 0x71468F | 1024 |
| CClouds::MovingFogRender | 0x716C90 | CALL CPlaceable::GetRight 0x716D8F→0x41CC70; trigonometría de su rama sin matriz | 1024 |

El banco deriva de Clouds21 y conserva sus fixtures, semillas, trazas y hashes de memoria. Ejecuta los tres consumidores completos, 3072 pares; los veinte hooks de CClouds cambian OFF/ON juntos para conservar el comportamiento de colaboradores internos. Los colaboradores externos de render/RNG y los actores son fixtures del banco anterior, por tanto esta prueba tiene alcance instrumentado y no acredita ejecución transitiva real ni otros cuerpos.

Cada target se invoca inmediatamente con un gate de ESP y EBX/ESI/EDI/EBP. Se comprueba la ruta EXE OFF contra el cuerpo fuente ON y el módulo de cada lado. Payload completo para todos los casos, incluso diferencias: nunca se registra original/candidate nulo ni sólo el retorno void. Comparación conserva trace, memoria, retornos y estado x87 numérico íntegro: CW, SW completo incluyendo C1/TOP, tag, registros vivos de 80 bits y MXCSR. Los 108 bytes físicos se guardan raw, junto a FIP/FDP/selectores/opcode e instrucciones runtime. Los offsets 12..27 describen direcciones/instrucciones reubicadas y no son resultados numéricos comparables; el analizador comprueba su coherencia con la evidencia raw. No se borran diferencias numéricas ni flags. Los registros vacíos conservan su residuo raw, pero no son operandos vivos.

Antes de cada lado se restaura FP108+MXCSR del juego; cada llamada usa PC24/nearest. Se exige CW/TOP/tag y controles MXCSR conservados; se restaura físicamente FP108+MXCSR antes/después de PumpMessages. La restauración final exige globals completos del banco, código de todos los colaboradores parcheados y targets, estados Enabled/Locked de los hooks y FP108+MXCSR.

## Ejecución a cargo del coordinador

Aplicar el parche sólo después de revisión y reconfigurar CMake para descubrir la nueva TU. Inspeccionar primero el objeto. Ejecutar build_probe.py/run_local.py mediante el flujo serial existente cuando no haya otra compilación o GTA. Ambos redirigen runtime_runner.GAME en memoria exclusivamente a E:/JUEGOS/GTA San Andreas - CVehicle Test, antes de importar probe_build/probe_case. No se edita runtime_runner.py ni la instalación personal.

El manifiesto incluye ambas TU y headers, configuración CMake, InjectHooksMain, reversiblehooks, ProbeSupport/ProbeRuntime y run_local. Los archivos de producción están aplicados y contrastados con el manifiesto de la corrida aprobada. El analizador exige tres funciones, 3072 pares únicos, 1024 casos únicos por método, payloads completos, cero diferencias, todos los gates y estado final restaurado. La aceptación requiere además revisar hash del EXE/ASI, partidas/configuración y evidencia de código generado; la preparación offline no sustituye esos pasos.

La unidad IA32 produjo FCOMP/FUCOMPP/FCOMPP seguidos de FNSTSW en ambos consumidores, inspeccionados con ../CppOnlyMigration/inspect_coff.py. El objeto provisional se conserva en artifacts y la evidencia final en codegen-proof-final.json. Los objetos por sí solos no constituyen validación dinámica.
