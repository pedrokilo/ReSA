<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CClothes, CScripted2dEffects y AttractorSprint

> Informe de esta entrega, conservado como evidencia histórica. El estado instalado más reciente y las entregas posteriores se documentan en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Los hashes y pruebas de este documento corresponden a su propia entrega.

**8.448 pares exactos aprobados** en la ejecución final del 23 de septiembre de 2026. Una primera ejecución, con hooks inicialmente deshabilitados, pasó otros 8.448 pares del mismo banco; no son cobertura adicional.

| Clase | Función | Original | Pares |
| --- | --- | --- | ---: |
| CClothes | GetTextureDependency | 0x5A7EA0 | 4.096 |
| CScripted2dEffects | Init | 0x6FA6F0 | 256 |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | Clone | 0x4B8870 | 2.048 |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | GetTaskType | 0x4B88D0 | 2.048 |

## Correcciones

- **Ropa:** el switch existente coincide; se habilitó después de comparar.
- **2DFX:** el helper original 0x6F9EB0 reinicia siete palabras y un booleano por pareja, conservando el segundo PartnerTask y el relleno. La asignación anterior sobrescribía estos datos. El candidato reproduce esas escrituras sin delegar al helper.
- **Sprint:** el original clona construyendo una tarea nueva con movimiento 7, padre y subtarea nulos; la copia anterior terminaba con el movimiento predeterminado de la base. Ahora construye una tarea nueva. El tipo es 262.

## Cobertura

Se ejecuta el original con el hook desactivado y el candidato con el mismo hook activado. Contadores confirman cero entradas al candidato durante el original y una durante su comparación.

Ropa cubre -1024..1023 y 2.048 valores de 32 bits dispersos, incluidos todos los valores del enum. Init compara byte por byte el intervalo 0xC39ED0..0xC3E010 (fin exclusivo): tablas, huecos, efectos sin modificar y 16 bytes de guarda por extremo, con 256 patrones iniciales, incluidos cero y 255. Cada invocación restaura inmediatamente el intervalo real.

Clone y GetTaskType usan 2.048 patrones de objeto de entrada con guardas y comprueban que no cambian. Clone usa el pool real, comprueba una asignación, libera la tarea y verifica el número de plazas usadas. Se comparan los 12 bytes posteriores al vptr y el tipo virtual; el vptr de DLL y ejecutable se excluye porque sus direcciones difieren. Se desactiva temporalmente el hook del constructor de la base para que el original no llame al candidato compartido. Se restaura su estado al terminar.

La ejecución final arrancó con los cuatro hooks habilitados y desbloqueados; CClothes recibió 18 llamadas e Init 3 antes de sus respectivos bloques. El campo `startup_calls` del log es el contador al comenzar cada bloque: el valor 4.096 de GetTaskType incluye las llamadas de las comparaciones Clone anteriores y **no** son llamadas de arranque.

## Ejecución y evidencia

```powershell
python reversing/run.py --case clothes-scripted-sprint
```

Integrado en --all y en el único PROBAR.cmd. [Runner](run_local.py), [sonda](LiveComparison.hpp), [constructor de sonda](build_probe.py), [resumen con hashes](validation-summary.json).

El constructor de sonda guarda y restaura byte por byte los fuentes en finally. Los artefactos locales están en artifacts/. El runner espera el hash aprobado en _baseline/gta_reversed-clothes-scripted-sprint-probe.asi, cierra su propio proceso y verifica restauración de ASI/PDB y hashes de partidas.

Evidencia final: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-clothes-scripted-sprint\20260923-001346-817023`. Logs completos, resumen y resultado conservados allí.

## Límites

Se validaron las cuatro funciones pendientes, no todos los métodos de estas clases. No se probó cambio completo de ropa, interacción prolongada con atractores, tareas hijas reales, agotamiento del pool ni representación audiovisual. El reinicio ya permite arrancar con su hook activo, pero no identifica todas las causas del comentario histórico de crash. La comparación replica incluso campos que el original deja intactos.

Se mantienen excluidos Audio y Global/SurfaceInfos_c al arrancar. No se repitieron las sondas de otras clases. Este banco finito y la compilación normal no garantizan estabilidad general; la campaña integral queda para cuando estén completas todas las clases.

Al cerrar esta entrega se instaló este ASI normal solo en la copia de pruebas: `d62c1071aadb1b459964a46efc4bfc3b9c6b3ea094521374408c14de7e9da034`. Respaldo _baseline/before-clothes-scripted-sprint-20260923. Partidas sin cambios, instalación personal intacta. Sin commit/push.
