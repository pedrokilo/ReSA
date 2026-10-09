<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FindCoverPointsBehindBox: banco preparado

2026-10-08. No compilado ni ejecutado. Identidad `CFormation::FindCoverPointsBehindBox`, `0x00699FF0`.

256 pares con rectángulos asimétricos, cuadrantes del objetivo, capacidad de salida 0/23/24, altura 7 exacto y distancia de corte 0/100. Compara toda la estructura CPointList y verifica bytes exactos de ambas entradas durante conmutación. Restauración efectiva de ambas entradas y flags/candados de todos los hooks.

El banco escribe un registro antes de cada llamada para conservar la identidad del caso si el candidato accede fuera de corners por `(i-2)%4`. No corregir antes de preservar el primer fallo. Después de corregir, ampliar con distancias de corte exactas, flotantes adyacentes a altura7, rotaciones y datos degenerados; el corpus actual es de reproducción y no una cobertura final suficiente.

Colaboradores Matrix/Vector/PointList no están sustituidos en este banco: cualquier replay requiere revisar sus fuentes y añadirlas al manifiesto de dependencias. No afirmar independencia integral de esos colaboradores. REA nuevo y ejecución son coordinados por root.
