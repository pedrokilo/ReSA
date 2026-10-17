<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de conducción y vuelo

Corrida `20261010-205837-320565`. Lectura íntegra del gzip archivado y comprobación nueva de resultados, sin ejecutar el analizador del banco, compilar o iniciar GTA.

## Conteos y gates

4096 comparaciones con pares únicos exactamente 0..4095; dos funciones con 2048 casos únicos exactamente 0..2047 y dieciséis perfiles de 128 variantes cada una. Cero original/candidate nulos, cero desigualdades de objetos completos y cero equal/accepted falsos. Ambos lados cumplen fixture_unchanged, floating_state_restored, globals_restored, helper_route_ok, nonvolatile_preserved, route_ok y stack_restored. Dos function_summary declaran 2048 casos y cero fallos; un único cierre declara 4096 pares y restauración correcta.

Cada salida contiene los **18 bloques crudos** previstos, contador final, argumento físico y EAX/ECX/EDX. Se comprobaron nuevamente los efectos del contador con redondeo float por operación, truncación y wrap, y se verificó que counter_after corresponde al dword real dentro de state[0] en B794EC/B794F0. No se eliminó ningún campo antes de comparar los lados.

## Cobertura efectiva serializada

| Ruta derivada de input y efecto | Driving | Flying |
| --- | ---: | ---: |
| grant | 1024 | 512 |
| fast | 496 | 76 |
| slow | 288 | 84 |
| unchanged | 240 | 96 |
| subtype_skip | — | 1024 |
| contact_skip | — | 256 |

Las velocidades inmediatamente inferiores, iguales y superiores a 0.2/0.8 en Driving y 0.5/1.3 en Flying están presentes dentro de las rutas de movimiento activas. Flying activo incluye up.z -1/0/1. La cobertura procede de inputs serializados y efectos de contador comprobados independientemente: no es tracing dinámico de cada salto del EXE ni todo el producto cartesiano.

## Colaboradores y restauración

El comienzo declara nueve colaboradores con route_ok=true. FindPlayerInfo53FB70 y ftol821B40 no están registrados y conservan el EXE; los otros siete están reconstruidos, registrados y ON. Increment55C180 y Display55B980 ejecutan cuerpos DLL compartidos. La lectura de HelperRoutes y su huella corroboran checks de modos/flags, bytes de entrada/biblioteca/cuerpo y resolución E9/módulo. La aprobación no significa independencia transitiva de texto/HUD/CRT.

Los snapshots abarcan estadísticas B78200..B79517, ayuda completa, gString/gString2, PlayerInfo, flags enumerados, fallback, timestep y buffers sintéticos completos de vehículo/matriz con canarios. Los 4096 pares verifican su restauración junto con x87 de 108 bytes y MXCSR; el final incluye los dos hooks restaurados con sus modos/código iniciales. Las propiedades cubren esos bloques/rutas, no todos los globales del juego. No se prepara dinero ni se reivindica cobertura financiera; el PlayerInfo real completo se observa/restaura junto al resto.

## Huellas y reparaciones anteriores

SHA-256 recalculado del registro descomprimido: `d218e38f10bf24eba551286ac0f40a891d0706725a7b61c01ee80a8331b09725`. ASI disponible: `9650b4515c596a00f7aaaf51b4566232173d15a79de5ef947e62cb3236563f84`. Coinciden con aprobación, manifest y ejecutado declarado. Las 41 fuentes coinciden entre manifest, aprobación y archivos actuales al revisar; los cuatro instrumentos coinciden con sus huellas archivadas.

Antes de esta prueba la coordinación corrigió exclusivamente la ruta del manifest a source/game_sa/Core/Matrix.h. La propuesta escrita con CRLF se aplicó con ignore-space-change contra Stats.cpp LF tras comprobar contexto: no cambió las instrucciones. Estas reparaciones están documentadas en README y pertenecen a preparación/compilación, no a una diferencia runtime de la fuente.

El resumen declara saves_unchanged/configs_restored y conserva tres huellas iniciales de partidas y dos de configuración; no archiva sus huellas posteriores individuales. Estas propiedades se apoyan en el runner, sin volver a medir archivos vivos de otras corridas. Tampoco se recalculó el EXE vivo ni un PDB ausente. La revisión no encontró diferencias ocultas, resultados nulos ni gates falsos.
