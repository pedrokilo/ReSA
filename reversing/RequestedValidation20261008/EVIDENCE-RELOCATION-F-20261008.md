<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reubicación de evidencias a F: (2026-10-08)

Las cinco carpetas de artefactos se movieron a `F:\ReSA-evidence-archive-20261008`.
Los registros históricos conservan sus rutas y hashes originales; este documento sólo
registra la nueva ubicación y no modifica evidencias ni documentos congelados.

Junctions `reversing/<banco>/artifacts` (reparados; antes apuntaban a
`C:\Users\pedro\Tools\ReSA-evidence-archive-20261008`, que ya no existe):

| Banco | Destino actual | Archivos verificados (SHA-256 vs `*-file-hashes.json`) |
|---|---|---|
| RequestedPreflightAdditionalLifetimes | `F:\ReSA-evidence-archive-20261008\RequestedPreflightAdditionalLifetimes-artifacts` | 34/34 |
| RequestedMenuBatch | `...\RequestedMenuBatch-artifacts` | 12/12 |
| RequestedPreflightTaskCtorDtor | `...\RequestedPreflightTaskCtorDtor-artifacts` | 21/21 |
| RequestedGeometryPlanesV2 | `...\RequestedGeometryPlanesV2-artifacts` | 6/6 |
| RequestedAnimProjectilePair | `...\RequestedAnimProjectilePair-artifacts` | 72/72 |

Los manifiestos `*-file-hashes.json` están en la raíz de `F:\ReSA-evidence-archive-20261008`.
