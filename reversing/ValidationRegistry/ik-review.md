<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Vigencia de IKChain_c::SetupBones

Revisión del 23 de septiembre de 2026. Entrada `IKChain_c,SetupBones,0x00617CA0,S`. [Nueva evidencia](ik-validation.json) y [alcance histórico](../IKDataWarnings/README.md).

## Dependencias y alcance revisados

| Ruta | Función en el banco |
| --- | --- |
| IKChain.cpp/.h | Obtiene matriz del pivote, recorre del efector al pivote excluido, crea el array y enlaza los padres internos |
| CEntity::GetBoneMatrix, CPed::GetAnimHierarchy y RenderWare | Acceso al esqueleto real, matriz del pivote e índices de frames; RpHAnimIDGetIndex delega en 0x7C51A0 del EXE |
| BoneNodeManager y BoneInfo | Tabla de padres/límites del host y reserva de 128 nodos; búsqueda de etiquetas, retirada y devolución de nodos |
| BoneNode::Init → InitLimits → GetIdFromBoneTag | Copia orientación/posición del keyframe, limpia hijos, fija límites y velocidad; operaciones de CVector |
| BoneNode::AddChild y TList_c | Padre y lista de hijos; listas del gestor, RemoveHead/AddItem/RemoveAll |
| IKChain::Exit, new[]/delete[] | Liberación con la implementación correspondiente, devolución de nodos y restauración del gestor entre casos |

Se revisaron cuerpos, helpers inline, estructuras y el banco. Se conserva el pivote excluido deliberadamente: no se busca dentro de la cadena como padre de su raíz. Las rutas de diagnóstico para etiquetas inválidas y los fallos de asignación no están cubiertos. No se ejecuta el solver completo ni se acredita Update, MoveBonesToTarget o toda la clase.

La prueba emplea el esqueleto del jugador en lectura, cadenas separadas ejecutadas secuencialmente y restauración del gestor entre original/candidato. Comprueba orden, etiquetas, padres, orientación, límites y velocidad. No compara padding, direcciones, matrices aún sin calcular ni todos los campos internos; esta acreditación conserva ese alcance.

## Nueva prueba y restauración

Se recompiló la sonda desde el árbol actual y se ejecutó en la copia CVehicle Test sin exclusiones de hooks. Ejecución `resultados-ik-data/20260923-164625-027968`: **131 pares únicos coincidentes**, contadores original/candidato 0/1 y caso efector 5/pivote 4 incluido. Devolución de nodos, jugador, gestor, frames y keyframes comprobados. No se suman repeticiones como cobertura única adicional.

El mismo banco volvió a comparar **9.384 bytes de tablas de clima**, grupos activos de coches y valores explícitos de skimmer. Son comprobaciones complementarias con los datos previamente reparados, no otras 9.384 funciones validadas. Esta tarea no modifica timecyc.dat, vehicles.ide ni cargrp.dat; se verificaron sus hashes antes/después junto a ASI/PDB, INI y partidas.

Sonda ASI `511a491a0856c02e88124f1bc49995ca73be6be2b119587253a320385fdcf44c`. Original Compact `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Hash del log, datos, configuración y manifiesto en el resumen enlazado. Artefactos voluminosos archivados en `artifacts/20260923-164601`; se restauraron también las sondas históricas predeterminadas.

Entorno: Debug x86, Visual Studio 18 2026, MSVC 14.51.36231, Win32/v145. Se conservó la caché de CMake y su hash. Se capturaron antes de compilar las huellas de source/cmake/conan, configuración versionada y banco, verificadas después de restaurar. Cualquier cambio, añadido o eliminación en esos árboles retira conservadoramente la acreditación al regenerar. El entorno externo y los datos están acotados a los documentados; el contador del repositorio no vigila instalaciones externas ni certifica otro entorno.

## Decisión

**Vigencia acreditada para SetupBones dentro del alcance de estos 131 pares.** Es la segunda entrada acreditada del registro. Conserva dependencias del host y llamadas al original; no es un subsistema independiente. La métrica global de independencia permanece sin medir. No se modifican flags, código del motor ni los 86 bloqueos aplazados, y no se afirma estabilidad general.
