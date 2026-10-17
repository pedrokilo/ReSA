<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría de bancos y replay, 2026-10-08

Esta auditoría es estática: no compila ni ejecuta GTA. No representa validación funcional nueva.

## Hallazgos bloqueantes

1. `reversing/Pending355/scope.json` y `status.py` cubren el lote anterior de 355 identidades (34 clases), no el alcance completo solicitado. `status.py` sólo descubre carpetas terminadas en 355 y PedDamagePending. Se necesita inventario del alcance actual por identidad completa (class, fn_name, address, type), sin incluir silenciosamente GangFollower.
2. 119 de los 179 headers que mencionan identidades de Pending355 contienen `state_restored,true` literal. Es búsqueda textual, no una afirmación de que todos esos bancos pertenezcan al nuevo alcance. Ejemplos comprobados: CamAttached355, CamControl355, CameraProcess355, StorageCore355, StorageIO355, Sunbathe355. `probe_case.analyzer` confía en ese booleano; no demuestra la restauración.
3. Incluso varios bancos con expresión de restauración no verifican todo lo que restauran. `TrainQueries355/LiveComparison.hpp` restaura oldCounts/oldNodes, pero su `restored` sólo compara estaciones, total y flags de hooks. `BikeCore355` compara timestep y flags, pero no bytes del código de los EntryPatch, aunque éstos salen de ámbito antes del resultado.
4. `CamAttached355` emite complete mientras sus EntryPatch Water siguen vivos; los destructores restauran después del evento. El momento exacto de aceptación precede la restauración real de esos parches.
5. `Pending355/batch_probe.py:78` fija CL=/MP6; no fija _CL_=/MP6. `probe_build.py` tampoco limita procesos por sí mismo. La instrucción del usuario requiere límite efectivo de seis; flags posteriores del proyecto pueden ganar a CL.
6. `register.py --strict` exige headers completos y CPP modificados respecto a HEAD. No captura el cierre de todos los CPP colaboradores: un CPP modificado y luego comprometido puede desaparecer de esa comprobación, y un colaborador nunca incluido en SOURCES puede cambiar sin invalidar el manifiesto.
7. `probe_summary.py` escribe `dependency_review: None`. `contrib/validation_registry.py` exige evidencia y hashes de dependencias para `current`; el replay y registro existentes por sí solos conservan aprobación histórica, no acreditan vigencia del registro.
8. `probe_summary.py` asigna EXE_HASH constante al resumen. Sin pasar por register --strict, esa constante no demuestra identidad del EXE realmente ejecutado. `runtime_runner.run_case` sí comprueba hash del EXE y ASI instalada y lo conserva: debe mantenerse como fuente de identidad.

## Mecanismo propuesto de restauración

Un helper RestorationAudit debe empezar antes de la primera mutación, registrar rangos globales explícitos (dirección, longitud, bytes), todos los hooks participantes (Hooked, Locked, bytes reales de entrada o slot virtual) y todos los destinos EntryPatch (bytes originales). Tras cada banco, los EntryPatch deben salir de ámbito, después se restauran globales/hooks, y por último se compara byte por byte todo el inventario con los snapshots. La evidencia debe contener rangos/destinos, resultado de cada comparación y fallos; complete exige Verify=true. No basta comparar flags Hooked/Locked, ni emitir complete antes de destructores. Los rangos no inventariados deben declararse como límite, no asumirse restaurados.

## Replay concreto

1. Inventariar sólo identidades pendientes del alcance actual desde hooks.csv y registro; asociar banco y funciones/colaboradores. Revisar GangFollower exclusivamente si lo llama un fixture incluido.
2. Reparar los bancos seleccionados con snapshots reales, ABI y ramas documentadas. Conservar informes históricos y fallos. Usar llamados con firmas ABI acreditadas; bool se compara según AL, float ST0, enteros/punteros EAX y uint64 EDX:EAX cuando aplique, agregados según su convención y salida real.
3. Congelar fuentes/instrumentos. Capturar todos los archivos source (o cierre probado de dependencias), drivers, helpers, builders/runners/analyzers, CMake y configuración; conservar commit y árbol congelado. Implementar dependency_review verificable para el registro.
4. Compilar los grupos Debug en serie con _CL_=/MP6 y comprobar límite efectivo. Cada grupo registra todos sus instrumentos/dependencias; cambiar alguno renueva todos los bancos de ese grupo.
5. Ejecutar serialmente cada banco en proceso nuevo; verificar EXE, ASI instalada, catálogo runtime, comparaciones, cobertura positiva/negativa/límites y restoration audit. El runner existente conserva fallos, restaura ASI/PDB/INI con hashes y verifica partidas: mantener esos gates.
6. Fallo de arranque sin comparación se conserva como intento fallido y se diagnostica; repetir mismo binario antes de atribuir diferencia funcional.
7. Registrar únicamente evidencia con identidad completa, hashes realmente ejecutados y dependency_review vigente. Recalcular Markdown, CSV, inventarios y mapas. Compilar DLL normal sin sonda y comprobar GTA cerrado.

## Límites de esta auditoría

No se efectuó análisis estático nuevo del EXE ni replay. Las 119 coincidencias sólo cuantifican el defecto textual sobre el alcance histórico; el inventario actual determina los bancos a corregir. Las desviaciones históricas de resolutions.json (PlaneDamage y CamControl) siguen siendo exclusiones explícitas y no equivalencia global. `runtime_runner` restaura archivos externos con verificaciones reales, pero no sustituye snapshots de memoria/hooks/parches dentro del proceso.
