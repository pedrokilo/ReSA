<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxEmitterRender355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `FxEmitterBP_c::RenderHeatHaze`, `FxEmitterBP_c::Render`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fx-emitter-render355/20261008-073656-465179/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# FxEmitterRender355

3072 pairs: RenderHeatHaze 1024 and Render 2048. Render and RenderHeatHaze APPROVED: 3072/3072 equal, zero differences, report 20261007-222532-935246. Earlier failures were corrected using the EXE brightness channel and smoke RNG multiplication order. Final source snapshot repetition of other emitter banks is coordinated by root.

Positive particles, local transformations, camera/flat/direction/velocity matrices, rotation, animated textures, color ranges/brightness, random sizes, trails and smoke. Every triangle captures position/UV/RGBA; complete matrices, emitters, particles and dependencies are compared. Graphics driver, matrix pool, RenderInfo and smoke isolated symmetrically; render callback, heat flag, smoke pointer, timestep and previous camera restored. No GPU pixels or real FxInfo module validation. Hooks false until root enables approved final functions.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261007-222532-935246`, 3072 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fx-emitter-render355/20261007-222532-935246/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261007-222532-935246`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fx-emitter-render355/20261007-222532-935246/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
