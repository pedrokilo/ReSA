<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote de 355 identidades pendientes

Alcance solicitado: 34 clases, desde `CCarEnterExit` hasta `CCam`, conservado en `scope.json`. La reconstrucción parte del EXE 1.0 US: ensamblador completo y pseudocódigo de REA cuando estuvo disponible; 17 identidades conservan fallos documentados del proveedor, con comprobación de firmas, layout, colaboradores, condiciones y orden de las operaciones de coma flotante.

`resolutions.json` documenta una corrección del catálogo: `CPedDamageResponseCalculator::IsBleedingWeapon` en `0x4B5C2A` es un bloque interno de `ComputeDamageResponse`, no una función invocable. Se incorpora al cuerpo de su padre y no recibe un hook independiente. El objetivo ejecutable del lote es, por tanto, de 354 funciones válidas y una corrección de catálogo.

Los bancos hermanos `*355` y `PedDamagePending` conmutan entre EXE y DLL con fixtures reproducibles dentro del motor. Comparan resultados, estado y trazas; sus README describen colaboradores aislados y límites. Los hooks del alcance están activados. Se distinguen la reconstrucción, las aprobaciones históricas y el replay actual, que quedó parcial por orden del usuario. La compilación utiliza Debug y seis procesos; la ejecución es serial y restaura archivos, configuración, partidas y hooks, cerrando GTA al terminar.

`batch_probe.py` permite compilar un grupo de bancos congelados en una misma sonda. Los marcadores de ejecución activan sólo el banco solicitado y cada banco se ejecuta en un proceso nuevo del juego. Sus manifiestos registran todas las fuentes e instrumentos del grupo, incluido el driver generado; cambiar cualquiera de ellos obliga a repetir el grupo. `--no-install` permite comprobar la compilación y archivar sin instalar ni ejecutar.

`status.py` produce `progress.json`, distinguiendo bancos preparados, resultados históricos y resultados que aún coinciden con las fuentes e instrumentos actuales. Un banco preparado no constituye una validación. Una modificación posterior de una fuente compartida obliga a repetir los bancos afectados.

`register.py --strict` exige el manifiesto final con todos los headers y fuentes modificadas, el hash del EXE y de la ASI realmente ejecutados, y comprueba el alcance completo, la ausencia de cobertura duplicada, los informes aprobados y los hashes actuales de fuentes, instrumentos y binarios antes de escribir el registro. La aprobación final del lote se publica en `summary.json`; al detener el trabajo no existe un cierre completo aprobado; el estado parcial se conserva en `partial-summary.json`. Después del registro se sincronizan los Markdown, CSV, inventarios, mapas de memoria y contadores globales.

Las sondas e informes generados se conservan fuera del seguimiento de Git. Las carpetas de artefactos y las copias de sondas del juego pueden residir en la caché de la unidad C mediante junctions, conservando sus rutas originales. Esto no modifica la evidencia ni sustituye las comprobaciones de hashes.

## Estado al detener el trabajo (8 de octubre de 2026)

Se reconstruyeron las 354 funciones válidas de las 355 identidades solicitadas; la identidad restante es la corrección de catálogo descrita arriba. Las 354 cuentan con aprobación histórica en sus fixtures. El replay final se detuvo por instrucción del usuario: 74 de 177 bancos cumplieron la verificación de manifiestos completos para aquel cierre, con 301.568 pares del alcance. No se completó el replay final de los 177 bancos ni se afirma aprobación final global. La campaña completa prevista suma 639.008 pares del alcance y 1.024 suplementarios de InteriorGroup_c::DereferenceAnims (640.032 en total).

Persisten las tres desviaciones explícitas de resolutions.json: PlaneDamage explosive→smoke y dos rutas de CamControl que consumen datos de pila indeterminados en el EXE. Sus comportamientos C++ definidos quedan fuera de la equivalencia certificada. Las aprobaciones están limitadas a fixtures, colaboradores aislados y ramas documentadas; los límites de SEH de BikeControl permanecen en su README.

El cierre parcial verifica 117 de las 354 funciones con manifiestos actuales. Un banco tiene fallo pendiente: GangFollower355, destructor, 256/256 pares con diferencia de vptr en task[0]. Se conserva sin corregir por la instrucción de detener el trabajo. Los otros 102 bancos no completaron el replay final; sus aprobaciones anteriores son históricas. CPedGeometryAnalyser tiene 6/29 funciones con replay final actual; su clase no está totalmente validada en esta fase.

Solicitud posterior: la reconstrucción de `Interior_c` cambió el árbol de fuentes. El cierre parcial de este lote se conserva como evidencia histórica; sus pruebas no se repitieron en el trabajo de Interior. Consulte la vigencia recalculada en [ValidationInventory](../../docs/ValidationInventory.md).
