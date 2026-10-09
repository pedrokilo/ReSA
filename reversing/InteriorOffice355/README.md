<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorOffice355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::UpdateOfficePeds`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-office355/20261008-020212-902386/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorOffice355

InteriorGroup_c::UpdateOfficePeds — 594E90, 2048 pares pendientes; combo REA+ASM en ../World355Evidence/InteriorGroup_c_UpdateOfficePeds.txt.

Cuatro transiciones de visibilidad; eliminación de peds válidos/inválidos, matriz existente/asignada, franjas horarias completas/parciales/fuera, 0–36 puestos, límite 16, creación nula, asignación de tarea nula y selectores aleatorios. Constructor real de tarea; stubs simétricos de población, streaming, reloj, RNG, inteligencia, matrices y asignación de task. Se comparan buffers completos por FNV-1a64 y trazas exactas; entrada de modelo global restaurada. Sin eventos/render/archivos reales.

python reversing/InteriorOffice355/build_probe.py
python reversing/InteriorOffice355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-020212-902386`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-office355/20261008-020212-902386/summary.json>).

Última aprobación histórica: `20261008-020212-902386`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-office355/20261008-020212-902386/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_UpdateOfficePeds.txt](../World355Evidence/InteriorGroup_c_UpdateOfficePeds.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
