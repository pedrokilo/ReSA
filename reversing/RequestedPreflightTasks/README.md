<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lifetime y Expanded antes de gráficos

2026-10-08. Banco preparado para896pares básicos y192pares de ramas de destrucción, en procesos separados. Conserva intactos TaskLifetimeComparison.hpp, TaskLifetimeExpanded/Expanded.hpp y Frontend.hpp. No registra nuevas validaciones ni sustituye fallos anteriores.

El harness temporal entra en NOTSA_WinMain después de su guardia de cierre del registro y antes de SystemParametersInfo/RenderWare/input; devuelve0. Guarda todos los estados/entradas antes de desactivar los hooks salvo WinMain, usa el puente de marcadores de Frontend existente y restaura los estados originales. El runner mantiene WinMain activo, sin selector histórico ni argumento unhook, exige cierre natural0 y preserva logs/manifiesto.

CTimer inicial se trata como memoria del EXE mapeado, no como reloj del juego en funcionamiento. Se registran los12bytes de B7CB7C..B7CB87 y se exige que permanezcan iguales. No se acredita integración de timer, pools, atracción, entidades, animación ni gameplay. Los colaboradores aislados y child allocations del banco Expanded conservan sus contratos documentados; ver README original.

El analizador reutiliza exactamente los conteos y comprobaciones de ramas del banco congelado. El corpus896 básico tiene límites expresos en terminal vptr y colaboradores: coincidir en ese corpus no acredita una función completa. Expanded192 exige terminal vptr nativo, subtask borrado, trazas idénticas a expectativas y restauración de colaboradores. Ninguna disparidad se filtra.

Builder Debug `_CL_=/MP6`, exterior serial1; captura ASI/PDB y hashes de fuentes/instrumentos antes de finally. Restaura WinMain y binarios previos byte a byte. Root coordina ejecución; los agentes compilan en serie.
