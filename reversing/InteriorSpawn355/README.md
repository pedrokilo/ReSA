<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorSpawn355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::SetupHousePeds`, `InteriorGroup_c::SetupShopPeds`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-spawn355/20261008-020232-371962/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorSpawn355

InteriorGroup_c: SetupHousePeds (5965E0), SetupShopPeds (594C10). 2048 pares cada función, 4096 pendientes de ejecución. Combo REA+ASM en ../World355Evidence/InteriorGroup_c_*.txt.

Las firmas pasan a void según los RET originales. Casa: 1–4 actores, RNG, zona/grupo/ocupación válidos e inválidos, conservación de peds por flags, streaming, cerebro, matriz existente/asignada. Tienda: número de interiores firmado negativo/cero/positivo, factores originales negativos -3/-7, encargado y clientes, centros de tiles y altura +1. Ambas: creación/tarea nula, constructors reales, asignación primaria y trazas exactas; bytes completos por FNV-1a64. Mocks simétricos sin eventos/render/archivos reales. Se restauran modelo, zona y flags BB3D9C. La llamada de cerebro 46CED0 usa el colaborador EXE, porque el método declarado carece de definición DLL; el banco lo sustituye simétricamente.

python reversing/InteriorSpawn355/build_probe.py
python reversing/InteriorSpawn355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-020232-371962`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-spawn355/20261008-020232-371962/summary.json>).

Última aprobación histórica: `20261008-020232-371962`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-spawn355/20261008-020232-371962/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_SetupHousePeds.txt](../World355Evidence/InteriorGroup_c_SetupHousePeds.txt), [InteriorGroup_c_SetupShopPeds.txt](../World355Evidence/InteriorGroup_c_SetupShopPeds.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
