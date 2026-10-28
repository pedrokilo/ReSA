<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría COFF de propuesta TrigSimple: solo lectura

compare_coff.py compara objetos i386 COFF estándar o bigobj; no compila, aplica patches ni ejecuta juego/Git. Pendiente ejecución con objetos antiguos y nuevos obtenidos por coordinador. El script solo tiene verificación sintáctica; no está certificado contra todos los formatos MSVC.

## Comparación diseñada

Un spec JSON debe listar consumidores con old_object/new_object y símbolo COFF mangled exacto (obtener del objeto, no inventarlo): Birds::Render, BrightLights::Render, PlaneTrails::Update, JetPack::ProcessPed y Swim::ProcessControlInput. helpers lista pares old/new de símbolos exactos y canonical que designa sin/cos/sinMul/cosMul. No usar reglas por prefijo ni sustituir todos los calls.

Para cada función se comparan todos los bytes y relocations de su rango. El fin es la siguiente función del mismo section o el fin del section; padding incluido. Se exige mismo tamaño, posiciones/tipos/addends de relocations y destinos exactos, excepto aliases autorizados. Un alias solo puede estar en call E8 con relocation IMAGE_REL_I386_REL32. Se normalizan los cuatro bytes del campo relocatable para comparar código, pero su addend se compara explícitamente. No se acepta diferencia de instruciones, stack, constantes, registros ni branching. Si una llamada interna resuelta no posee relocation, sus bytes permanecen intactos: el script rechaza diferencias, no inventa tolerancia.

Conservador: renombre de símbolos locales/section/constants puede causar rechazo aunque fuese benigno; corresponde investigar y mantener evidencia, no omitir relocations. Requiere símbolos de función definidos/type function y rechaza formatos/tipos de relocation no soportados. No prueba SEH/unwind/debug sections ni semántica del helper: inspeccionarlos separadamente si cambia algo fuera del cuerpo. Relocations a secciones/datos deben revisarse también para confirmar contenido/identidad del destino; nombre igual por sí solo no prueba bytes de datos iguales.

Objetos caller deben compilarse con opciones idénticas; solo TU TrigSimpleCpp es nuevo con opciones propuestas. Probar helper generado FLD DWORD;FSIN/FCOS;FMUL DWORD cuando toca;RET sin stores intermedios/CRT/SSE/prólogo instrumentado. La equivalencia de caller no concede por sí sola fidelidad x87 universal.

## Aplicabilidad del parche

patch-context-audit.json contiene búsquedas exactas de cada contexto antiguo de hunk contra fuente actual (sin ejecutar Git). CMake debe tener una sola coincidencia; los dos nuevos archivos deben estar ausentes. Auditoría puntual: cambios posteriores de fuente exigen repetirla. No se aplicó producción.

## Fixtures: cobertura estructural, no validación dinámica

BirdsRender banco índice5/512: fixtures generan modos1/2/0/3, created0/1, tamaños0.4..2.6 y ángulos-4..4; screenMask permite proyectar algunos pájaros. Ambas llamadas sinMul/cosMul están juntas tras CalcScreenCoors exitoso. Eso habilita ambas primitivas pero no acredita que se hayan observado dinámicamente: medir llamadas/código generado o revisar trazas/cobertura por caso antes de afirmar cobertura.

BrightRender índice2/512: mkLight genera colores13/14/15 (small) y distancias varias, incluidos0..45/45..60/60..90/45 exacto/negativas. Los escenarios big restringen smallOnly. En rama small cada uno de8vértices llama sinMul y cosMul; comprobar que entradas sobreviven filtros de distancia/alpha y aparecen trazas/geometry en corrida real. No se afirma que todos los512 entren.

PlaneUpdate índice11/1024: fuente llama sinMul/cosMul en los tres aArray Update, con constantes2590/2200,2000/-2600 y2100/1300; corpus cambia timeMs entre rango2millones y uint32 completo/hora0..23. Invocar consumidor habilita las seis llamadas propias incondicionales, pero no se ha ejecutado ni contado en el juego. Ambas primitivas están en el código propuesto sin ampliar corpus.

Los dos bancos nuevos preservan tabla/índices/semillas/fixtures/gates. Sus snapshots/trazas no certifican flags CPU, todos raw regs ni estado/status x87 adicionales; no se inventa aprobación.

Resultado puntual: los5consumidores tienen una coincidencia por hunk; CMake antiguo tiene0 y no aplica. Usar cmake-current-proposal.patch como alternativa generada contra el CMake actual; eliminar el hunk CMake antiguo al armar propuesta combinada. No aplicar ambos. Nuevos archivosTrigSimpleCpp ausentes.

Endurecimiento de relocations: destinos nohelper comparan value/type/storage y, para datos definidos, hash de bytes completos del section referenciado. Cambios de colocación de datos pueden provocar rechazo conservador. Revisar relocations internas de datos y metadatos SEH aparte: un hash de bytes sin interpretación no sustituye esa revisión.
