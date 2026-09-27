# Registro de validación por función

La lista separa **haber pasado una prueba** de **tener confirmada su vigencia para el código actual**. `hooks.csv` incorpora ahora `validated`: `1` significa evidencia aprobada vinculada y `0` significa que todavía no existe esa vinculación. La vigencia actual sigue siendo una dimensión separada.

Datos: [validation-registry.json](../validation-registry.json). Auditor de solo lectura: [validation_registry.py](../../contrib/validation_registry.py). El [generador del panel](../../contrib/hooks2md.py) lo consulta automáticamente; no ejecuta GTA ni necesita los binarios locales de las sondas.

## Primeras funciones con vigencia acreditada

| Función | Evidencia vinculada | Fuentes registradas coincidentes | Vigencia completa |
| --- | --- | --- | --- |
| CTaskSimpleRunNamedAnim::Constructor-Anim | 8.192 pares del constructor, repetidos con fuentes actuales | Sí | Acreditada para el alcance del banco |
| IKChain_c::SetupBones | 131 pares únicos, repetidos con fuentes actuales | Sí | Acreditada para el alcance del banco |
| RpAnimBlendClumpUpdateAnimations | 652 pares, repetidos con fuentes actuales | Sí | Acreditada para el alcance del banco |

Hay **34 funciones con `validated=1`** y **0 con vigencia confirmada para el árbol modificado**. Las tres acreditaciones anteriores quedan pendientes por el cambio de huella global de source durante CustomBuilding; no significa que hayan fallado sus bancos. La barra es **0/8.080 · 0,00%**. CustomBuilding sigue aplazada y todavía no se incorpora al registro.

El [inventario de todas las funciones reconstruidas](../../docs/ValidationInventory.md) clasifica las 7.545 entradas: 0 vigentes, 34 históricas por revisar y 7.511 sin evidencia vinculada todavía. Incluye las clases y la siguiente tanda de funciones concretas. No significa que las 7.511 nunca se probaran: faltan bancos por incorporar. Las 535 entradas sin reconstruir se contabilizan aparte.

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
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
python contrib/validation_inventory.py
python -m unittest discover -s contrib/tests -p test_validation_registry.py
```

La primera orden muestra las comprobaciones; la segunda sincroniza `validated` en `hooks.csv`; la tercera actualiza el panel y la cuarta la lista completa de trabajo. El siguiente paso es revisar la vigencia de las 30 entradas históricas e incorporar los bancos restantes. No exige repetir automáticamente toda la suite. No se modifica `validated` manualmente en el CSV: lo mantiene `sync_validated_flags.py`. Los 86 bloqueos aplazados y las reglas de protección permanecen vigentes.

Para repetir la comprobación actual del constructor, `python reversing/ValidationRegistry/revalidate_namedanim.py` recompila el banco existente, conserva la sonda anterior, ejecuta la copia de pruebas y verifica la restauración. Sus resultados nuevos requieren revisión antes de actualizar el registro; no acredita funciones automáticamente. Reutiliza el banco y runner existentes, sin añadir otro CMD.

El mismo ejecutor acepta `--case ik-data` para SetupBones; también verifica que los tres archivos de datos permanecen intactos y utiliza el banco IK existente. No vuelve a reparar datos ni añade otro caso al catálogo.

También acepta `--case rpanimblend`: utiliza el nuevo build_probe.py y el analizador histórico con sus exclusiones Audio/SurfaceInfos, sin cambiar el hash fijo del runner archivado. `import_historical.py` importa de forma idempotente la tanda revisada de 30 funciones; exige identidad única y resumen aprobado, no concede vigencia ni infiere funciones por el total de pares. Conserva direcciones originales del informe cuando el CSV registra cero para una virtual.

### GetTaskType de GangLeader (2026-09-24)

Vinculada su [evidencia de 24 pares](../GangLeader/validation-summary.json), sin nueva acreditaci?n vigente. Se probaron entrada directa y rutas virtuales dentro del men? del motor, con contadores y conmutaci?n. La entrega usa fuentes aisladas para excluir los trabajos aplazados; no acredita toda la clase ni sus dependencias. Totales actuales: 34 evidencias hist?ricas, 0 vigentes, 7.511 reconstruidas sin evidencia vinculada.
