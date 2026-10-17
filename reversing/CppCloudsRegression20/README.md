<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Regresión C++ Clouds: 20 métodos

APROBADO: corrida 20261011-004915-062019, 8384 pares, cero diferencias, cero registros nulos y restauración verificada. Auditoría independiente en audit/audit_run.py y archivo en runs/20261011-004915-062019. Regresión de los métodos afectados por IA32/noPCH y los colaboradores separados de Clouds.cpp. No representa 20 conversiones nuevas.

Clon independiente de Clouds21: mismos 20 métodos, corpus, número de casos, fixtures, hooks, colaboradores simulados y comprobaciones. Total comprobado: **8384 pares**.

| Función | Dirección | Pares |
|---|---|---:|
| Init | 0x7138D0 | 64 |
| Update | 0x712FF0 | 1024 |
| Shutdown | 0x712FA0 | 64 |
| SetUpOneSkyPoly | 0x713060 | 512 |
| Render | 0x713950 | 1024 |
| RenderSkyPolys | 0x714650 | 512 |
| MovingFogInit | 0x713660 | 64 |
| MovingFog_Create | 0x713760 | 1024 |
| MovingFog_Delete | 0x713730 | 256 |
| MovingFog_Update | 0x716B10 | 512 |
| MovingFog_GetFXIntensity | 0x7136D0 | 64 |
| MovingFog_GetWind | 0x7136E0 | 128 |
| MovingFog_GetFirstFreeSlot | 0x713710 | 256 |
| MovingFogRender | 0x716C90 | 512 |
| VolumetricCloudsInit | 0x7131C0 | 64 |
| VolumetricClouds_Create | 0x715F40 | 1024 |
| VolumetricClouds_Delete | 0x7135F0 | 256 |
| VolumetricClouds_GetFirstFreeSlot | 0x7135C0 | 256 |
| VolumetricCloudsGetMaxDistance | 0x713630 | 256 |
| VolumetricCloudsRender | 0x716380 | 512 |

## Comprobaciones conservadas

Condición de igualdad idéntica a Clouds21: snapshot original==candidate y switches verdaderos en ambos lados. Conserva hashes/muestras de Clouds/VC/MF, slots, contadores, vértices/índices, globales, retorno y traza ordenada de colaboradores. Conserva RestoreAll por ejecución y restauración final de globales, hooks y locks. El analizador exige state_restored.

Solo cambian namespace/prefijo y serialización: original/candidate exportan snapshots completos para todos los pares, incluidos void y fallos. El banco original exportaba ret nulo para void pasantes y recortaba snapshots de fallos. Ninguna condición de aceptación cambia; no existen placeholders nulos.

No certifica todos los status/control x87, flags CPU ni todos los registros físicos. Las mediciones adicionales corresponden al banco CppCloudsMath3 de tres métodos y a la inspección del objeto. Pasar este banco demostraría regresión cero únicamente en las observaciones y corpus históricos.

## Fuentes y ejecución

Prefijo cppcloudsregression20; namespace CppCloudsRegression20Live. Ambos scripts redirigen GAME en memoria a E:/JUEGOS/GTA San Andreas - CVehicle Test; runtime_runner.py no cambia. No se toca la instalación personal.

Manifiesto: Clouds.cpp/.h, CloudsMathCpp.cpp/.h, CloudsAccessCpp.cpp/.h, CloudsWeatherCpp.h, Weather.h y headers de colaboradores, source/CMakeLists.txt y CMakeLists.txt, registro/implementación RH, ProbeSupport.hpp/ProbeRuntime.hpp, run_local.py y este README. Las opciones efectivas IA32/noPCH requieren comprobar el build real: el manifiesto no demuestra su aplicación.

Compilación y juego solo por coordinador y en serie. No usar cppcloudsregression20-only.txt para admitir regresión completa: el analizador exige los 20 métodos y 8384 pares.

## Verificación offline

Scripts analizados con ast.parse sin importarlos. Conteos extraídos:20 métodos/8384 casos. Comparación normalizada contra Clouds21: únicamente identificadores y serialización difieren; gates y corpus idénticos. No se copiaron resultados históricos ni artifacts ni aprobaciones.
