<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigCompoundBirds: banco pendiente de ejecucion

Preparado offline, sin compilar, ejecutar ni aprobar. Derivado de Misc6; conserva la tabla completa, indices originales de funcion y caso, semillas, fixtures, rutas y comprobaciones. El filtro selecciona consumidores sin compactar la tabla. La comparacion normalizada del instrumento contra el original confirma que solo cambian identificadores, filtro y serializacion.

Pares previstos: CreateNumberOfBirds: 1024; Render: 512. Total: 1536. No son resultados nuevos ni creditos de validacion.

Las filas exportan snapshots completos de original y candidate, tambien en fallos y funciones void. Ninguno de esos campos es nulo; un ret void nulo dentro del snapshot sigue siendo un dato legitimo. audit_nested.py exige objetos completos, claves ret/trace/switches, rutas verdaderas en ambos lados, trazas de tipo array e igualdad de todos los campos. Conserva el analizador comun: cobertura y reparto exactos, pares unicos, accepted/equal verdaderos y state_restored.

El manifiesto incluye fuentes historicas, TrigCompound3Cpp.cpp/.h, CMake, inyeccion de hooks, soporte de runtime y scripts de auditoria. GAME se redirige en memoria a la instalacion de prueba. La fuente propuesta aun no esta aplicada. Solo el coordinador puede aplicar, compilar o ejecutar en serie, tras guardar los objetos antiguos.

Los snapshots historicos no certifican todo el status x87, flags CPU ni registros fisicos. Se requiere inspeccionar los helpers compilados y comparar los objetos de consumidores antiguos/nuevos fuera de los destinos de llamadas autorizados. No se introducen tolerancias ni se debilitan comprobaciones.

Hash hpp origen:8146649abb23b8f0ac41674a8383c26a7966151216d92fd35610147daad37961.
