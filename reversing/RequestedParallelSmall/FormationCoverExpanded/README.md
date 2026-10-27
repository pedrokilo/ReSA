<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Corpus ampliado de FindCoverPointsBehindBox

2026-10-08. Preparado y congelado antes de compilar; este archivo no acredita replay.

4096 pares con cajas no degeneradas, transformaciones rÃ­gidas rotadas e inclinadas, capacidad inicial 0/22/23/24, objetivo en cuadrantes y alturas Â±7 con sus floats adyacentes, Â±0. Cortes: distancia exacta a una esquina, float inmediatamente inferior/superior y100. La distancia de frontera se calcula con operaciones x87 del EXE; la esquina se transforma por la rutina original0x59C890. No cubre pointers invÃ¡lidos, Ã­ndices fuera de capacidad, NaNs ni infinitos.

Se comparan todos los bytes de CPointList, cantidad y conmutaciÃ³n efectiva. Se exigen ramas de altura dentro/fuera y salidas con/sin puntos aÃ±adidos. Se restauran ambos cuerpos del hook y se verifican todos los flags/candados. Los colaboradores se sustituyeron dentro de la reconstrucciÃ³n por auxiliares locales con operaciones x87 revisadas contra0x59C890/0x59C910; el banco sÃ³lo usa0x59C890 para definir una frontera independiente.

El banco previo de256pares y su replay fallido permanecen en FormationCover y RequestedMenuBatch/runs/formation. La revisiÃ³n REA nueva confirma cuerpo835bytes0x699FF0..0x69A332: `ev_4432c08bc39cc17ed2196400075d5bdfda08981fe1591476bac857d4416f7bea`.

La compilaciÃ³n y replay son coordinados por root. El aislamiento del arranque WinInput que root usa para llegar al menÃº debe registrarse en el manifiesto; no interpretar el replay como validaciÃ³n de WinInput ni del arranque del juego.

## Approved menu replay, 2026-10-08

FindCoverPointsBehindBox passed 4096 pairs covering rigid rotations/inclinations, output capacity and height/distance boundaries. [Frozen enriched summary](../../RequestedMenuBatch/runs/formation-expanded/20261008-113522-938438/enriched-summary.json). This is historical fixture accreditation with dependency_review null, not exhaustive class or subsystem equivalence. The earlier failed attempt is preserved.
