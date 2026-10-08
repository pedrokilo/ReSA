<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Corpus ampliado de FindCoverPointsBehindBox

2026-10-08. Preparado y congelado antes de compilar; este archivo no acredita replay.

4096 pares con cajas no degeneradas, transformaciones rÃ­gidas rotadas e inclinadas, capacidad inicial 0/22/23/24, objetivo en cuadrantes y alturas Â±7 con sus floats adyacentes, Â±0. Cortes: distancia exacta a una esquina, float inmediatamente inferior/superior y100. La distancia de frontera se calcula con operaciones x87 del EXE; la esquina se transforma por la rutina original0x59C890. No cubre pointers invÃ¡lidos, Ã­ndices fuera de capacidad, NaNs ni infinitos.

Se comparan todos los bytes de CPointList, cantidad y conmutaciÃ³n efectiva. Se exigen ramas de altura dentro/fuera y salidas con/sin puntos aÃ±adidos. Se restauran ambos cuerpos del hook y se verifican todos los flags/candados. Los colaboradores se sustituyeron dentro de la reconstrucciÃ³n por auxiliares locales con operaciones x87 revisadas contra0x59C890/0x59C910; el banco sÃ³lo usa0x59C890 para definir una frontera independiente.

El banco previo de256pares y su replay fallido permanecen en FormationCover y RequestedMenuBatch/runs/formation. La revisiÃ³n REA nueva confirma cuerpo835bytes0x699FF0..0x69A332: `ev_4432c08bc39cc17ed2196400075d5bdfda08981fe1591476bac857d4416f7bea`.

La compilaciÃ³n y replay son coordinados por root. El aislamiento del arranque WinInput que root usa para llegar al menÃº debe registrarse en el manifiesto; no interpretar el replay como validaciÃ³n de WinInput ni del arranque del juego.

## Approved menu replay, 2026-10-08

FindCoverPointsBehindBox passed 4096 pairs covering rigid rotations/inclinations, output capacity and height/distance boundaries. [Frozen enriched summary](../../RequestedMenuBatch/runs/formation-expanded/20261008-113522-938438/enriched-summary.json). This is historical fixture accreditation with dependency_review null, not exhaustive class or subsystem equivalence. The earlier failed attempt is preserved.
