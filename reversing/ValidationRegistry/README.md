<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Registro de validación por función

La lista separa **haber pasado una prueba** de **tener confirmada su vigencia para el código actual**. `hooks.csv` incorpora ahora `validated`: `1` significa evidencia aprobada vinculada y `0` significa que todavía no existe esa vinculación. La vigencia actual sigue siendo una dimensión separada.

Datos: [validation-registry.json](../validation-registry.json). Auditor de solo lectura: [validation_registry.py](../../contrib/validation_registry.py). El [generador del panel](../../contrib/hooks2md.py) lo consulta automáticamente; no ejecuta GTA ni necesita los binarios locales de las sondas.

## Primeras acreditaciones: resultado histórico

La tabla conserva lo acreditado en aquellas revalidaciones. El estado actual se calcula debajo y en el inventario; las coincidencias históricas no se trasladan automáticamente a las fuentes de hoy.

| Función | Evidencia vinculada | Fuentes coincidentes en aquella prueba | Acreditación en aquella versión |
| --- | --- | --- | --- |
| CTaskSimpleRunNamedAnim::Constructor-Anim | 8.192 pares del constructor | Sí | Acreditada para el alcance del banco |
| IKChain_c::SetupBones | 131 pares únicos | Sí | Acreditada para el alcance del banco |
| RpAnimBlendClumpUpdateAnimations | 652 pares | Sí | Acreditada para el alcance del banco |

Hay **8.729 funciones con `validated=1`** y **0 con vigencia confirmada para el árbol modificado**. La barra de vigencia es **0/8.729 · 0,00%**.

**476 funciones conservan coincidencia de todas sus fuentes registradas.** La coincidencia de fuentes y la evidencia aprobada se consultan por función en el inventario. La revisión de dependencias es una condición adicional para acreditar vigencia; no se deduce de haber pasado un banco.

El [inventario de todas las funciones reconstruidas](../../docs/ValidationInventory.md) clasifica las 8.729 entradas: 0 vigentes, 8.729 con evidencia histórica por revisar y 0 sin evidencia vinculada. Las 0 entradas pendientes de reconstrucción se contabilizan aparte. Sin evidencia vinculada no significa nunca probada ni fallida.

### Integridad del universo catalogado

El registro de validación no decide qué funciones pertenecen al catálogo. Esa coherencia se comprueba por separado con:

```powershell
python contrib/audit_hook_catalog.py --strict
```

En el corte actual el auditor devuelve **OK**: no hay hooks activos ni registros indirectos fuera de `hooks.csv`, ni entradas del CSV sin scope fuente. Los owners inactivos se consideran información de código disponible, no funciones activas del denominador.

## Cómo funciona

- La identidad usa clase, nombre de entrada, dirección y tipo del CSV. Las sobrecargas tienen entradas distintas. Identidades ambiguas o duplicadas detienen la generación.
- Cada entrada enlaza el resumen aprobado y fija su SHA-256. Un resumen ausente, cambiado o no aprobado deja de aportar al contador. El resumen conserva los hashes de logs y las rutas locales; este auditor no vuelve a verificar logs ni reproduce pruebas.
- Los hashes de fuentes proceden del banco, no se inventan tomando el código actual. Las tres revalidaciones fijaron fuentes e inputs antes de compilar y verificaron su restauración después. Las 30 entradas históricas importadas aún no tienen vinculación de fuentes: no se les atribuye vigencia por coincidir el nombre de la función.
- La coincidencia de esos archivos no certifica el resto de dependencias. `dependency_review=null` mantiene la vigencia pendiente. Para acreditarla se necesita un informe revisado con hash y un mapa de archivos relevantes con hashes: headers, implementación de dependencias, configuración, compilación y demás entradas que afecten al alcance. El autor del informe debe justificar su integridad; el auditor comprueba integridad de archivos, no entiende automáticamente el C++.
- Cambios en evidencia, fuentes o archivos de la revisión retiran automáticamente la acreditación vigente al regenerar. No actualizar los hashes al código nuevo para hacer subir la barra: revisar el cambio y revalidar cuando corresponda.
- Los hashes son de bytes completos, conservadores: incluso un cambio de finales de línea puede exigir revisión. No se marca fallo del juego por ello.
- La revisión del constructor incluye huellas de árboles completos (`source`, `cmake`, `conan`): añadir o borrar archivos también retira la acreditación. El entorno externo queda acotado al compilador, bibliotecas y configuración probados, sin certificar otros entornos.
- Una entrada cuenta una vez, no una vez por par. El alcance de cada prueba sigue siendo limitado. Validación no significa independencia del original; esa auditoría continúa sin medir.

## Uso y siguiente paso

```powershell
python contrib/validation_registry.py
python contrib/sync_validated_flags.py
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
python contrib/validation_inventory.py
python -m unittest discover -s contrib/tests -p test_validation_registry.py
```

La primera orden muestra las comprobaciones; la segunda sincroniza `validated` en `hooks.csv`; la tercera actualiza el panel y la cuarta la lista completa de trabajo; la quinta ejecuta las pruebas del registro. El siguiente paso es revisar dependencias para acreditar vigencia e incorporar los bancos restantes. No exige repetir automáticamente toda la suite. No se modifica `validated` manualmente en el CSV: lo mantiene `sync_validated_flags.py`. Los 85 bloqueos aplazados y las reglas de protección permanecen vigentes.

Para repetir la comprobación actual del constructor, `python reversing/ValidationRegistry/revalidate_namedanim.py` recompila el banco existente, conserva la sonda anterior, ejecuta la copia de pruebas y verifica la restauración. Sus resultados nuevos requieren revisión antes de actualizar el registro; no acredita funciones automáticamente. Reutiliza el banco y runner existentes, sin añadir otro CMD.

El mismo ejecutor acepta `--case ik-data` para SetupBones; también verifica que los tres archivos de datos permanecen intactos y utiliza el banco IK existente. No vuelve a reparar datos ni añade otro caso al catálogo.

También acepta `--case rpanimblend`: utiliza el nuevo build_probe.py y el analizador histórico con sus exclusiones Audio/SurfaceInfos, sin cambiar el hash fijo del runner archivado. `import_historical.py` importa de forma idempotente la tanda revisada de 30 funciones; exige identidad única y resumen aprobado, no concede vigencia ni infiere funciones por el total de pares. Conserva direcciones originales del informe cuando el CSV registra cero para una virtual.

### GetTaskType de GangLeader (2026-09-24)

Vinculada su [evidencia de 24 pares](../GangLeader/validation-summary.json), sin nueva acreditación vigente. Se probaron entrada directa y rutas virtuales dentro del menú del motor, con contadores y conmutación. La entrega usa fuentes aisladas para excluir los trabajos aplazados; no acredita toda la clase ni sus dependencias. Totales de aquella entrega: 34 evidencias históricas, 0 vigentes, 7.511 reconstruidas sin evidencia vinculada.

### CConversationForPed (2026-09-30)

Vinculadas las dos funciones a su [comparación de 4.702 pares](../ConversationForPed/validation-summary.json): `Update`, 2.288; `IsPlayerInPositionForConversation`, 2.414. Original y candidato coinciden dentro del alcance probado, con cierre natural y restauración. Se registran los hashes de las fuentes probadas; la revisión global de dependencias continúa pendiente. Son 37 evidencias aprobadas, 0 acreditaciones vigentes y 7.511 reconstruidas sin evidencia vinculada. [Alcance y procedimiento](../ConversationForPed/README.md).

### CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume (2026-10-01)

Vinculada a su [comparación de 7.846 pares](../AudioCompletion/validation-summary.json), con `passed_path = covered_functions.UpdateAmbienceTrackAndVolume`. El banco ejecuta original y candidato por separado con proveedores de zonas, radio, clima, cámara y atenuación controlados, y compara el estado del gestor (44 bytes) y la traza de llamadas. Los hashes de `AEAmbienceTrackManager.cpp/.h` coinciden con el manifiesto de compilación `20261001-212155-352577`. `dependency_review` es nulo, así que la vigencia queda pendiente. La sonda se detuvo después en `AddAudioEvent`, una función distinta que no queda validada. [Detalle y corrección aplicada](../AudioCompletion/README.md).

### Cinco pendientes completadas (2026-10-07)

Vinculadas cinco funciones nuevas a 8.192 pares exactos: ProcessFOVZoom de CIdleCam (1.024), AbortPedTasks de CPedAttractor (1.024), CreateRoadBlockBetween2Points de CRoadBlocks (1.024), ProcessPed de CTaskSimpleFightingControl (1.024) y PlayerControlFighter de CTaskSimplePlayerOnFoot (4.096). Fuentes y manifiestos de las sondas finales registrados. Dependencias controladas, estado/INI/partidas restaurados; la revisión global de dependencias y regresión de juego no se acreditan con estos bancos.
