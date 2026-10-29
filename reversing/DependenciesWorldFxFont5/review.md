<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de la corrida aprobada

Se leyó y descomprimió directamente `runs/20261010-200311-662835/comparisons.jsonl.gz`, sin reutilizar las cifras del resumen como resultado de la cuenta. También se revisaron summary, approval-audit, manifiesto, ASI, instrumentación, desensamblado y la corrida previa 20261010-200111-971826 de la copia de pruebas. No se compiló ni ejecutó el juego.

**Resultado confirmado:** cuatro funciones, 15.360 pares únicos; cero objetos original/candidate nulos, cero desigualdades entre sus objetos completos, cero registros rechazados y cero gates fallidos en ambos lados. Los casos por función son únicos y completos: FX 1024, Font 6144, Camera 4096 y Radar 4096. Los cuatro function_summary tienen failures=0; cámara y radar acreditan tanto resultado vacío/rechazado como positivo. Hay un solo complete con pairs=15360 y state_restored=true, y ningún evento first_chance.

Se verificaron individualmente `route_ok`, `helper_route_ok`, `library_route_ok`, `stack_restored`, `registers_preserved`, `fp_preserved`, `readonly_ok`, `guards_ok` y `return_domain_ok`. Begin exige hooks registrados, layout de cámara correcto y bibliotecas RW reales. La instrumentación compara estado restaurado al finalizar y aplica la restauración antes de publicar cada resultado. Los colaboradores de Font y Radar conmutan junto con cada lado: OFF original/ON candidato. La restauración cubre los seis hooks, cámara, near/far, textos/contador, métricas y g_fx como readonly, estado x87 y MXCSR dentro del alcance que describe el banco.

## Hashes y restauración externa

SHA-256 del registro descomprimido: `1f39571e6dc281aaa5cbd78c53e6a1944a9e476f6fe6f9370b57417feb5c5ccf`, igual a summary y approval-audit. Gzip tiene mtime=0. SHA-256 de summary: `e67c7a8918ef19ec79a76b3adc9ebba7066c79d23f5d7b0638f2cd85dbf67f5d`.

Se calculó directamente el hash del ASI presente en artifacts: `54000fbfa714b9b2396dd81f6aabf6ca695aee20f66c4bde15456a3e21b659dc`, igual a manifiesto, summary y approval-audit. Se calculó el EXE de la copia de prueba: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`, igual a la identidad del EXE original requerida.

Los hashes de las cuatro piezas de instrumentación coinciden con approval-audit y manifiesto. Los trece hashes de fuentes probadas del manifiesto coinciden entre sí con approval-audit. Doce archivos actuales aún coinciden; `source/InjectHooksMain.cpp` ya cambió por integración posterior, de modo que su aprobación corresponde al hash histórico `1bcf20e722e114bd25680050a86cf0d927edd8e74e860844719024339636a291`, y no se extiende automáticamente al archivo actual. No se comprobó un PDB histórico: su hash está declarado, pero ese binario no está en artifacts. Esta limitación no afecta a la identidad del ASI comprobada.

Además de revisar saves_unchanged/configs_restored=true y su cálculo en runtime_runner, se recalcularon los hashes actuales de las tres partidas enumeradas y de los dos INI de la copia de pruebas. Todos coinciden con los hashes previos registrados en summary. No se leyó ni modificó la instalación personal de E:\JUEGOS\GTA San Andreas.

## Los 40 rechazos anteriores de Radar

Se leyó la corrida anterior completa: 15.360 pares, cero diferencias entre objetos original/candidate y exactamente **40 rechazos**, todos Radar, retorno=10 y únicamente `return_domain_ok=false`. Los otros ocho gates estaban verdaderos. Esto confirma ruido/error del dominio del banco, no una diferencia real de fuente.

En el EXE, 0x585233 ejecuta `inc eax`, 0x5852D4 `inc esi` y 0x585307 `inc dword ptr [esp+10h]`. El algoritmo incorpora intersecciones/esquinas sin eliminar todas las repeticiones de borde. Por tanto, el supuesto de máximo ocho no describe sus entradas de salida. Los buffers del banco tienen capacidad de dieciséis y guardas comprobadas.

Se cotejaron **los 4096 casos de Radar**, ambos lados, entre corridas: excluyendo sólo `return_domain_ok`, ningún otro campo cambió. La repetición aprobada contiene los mismos cuarenta retornos 10 y guardas correctas; el histograma completo de retornos es 0:1387, 1:28, 2:110, 3:207, 4:1474, 5:366, 6:348, 7:135, 8:1, 10:40. La modificación del gate a 0..16 no ocultó cambios de fuente ni de salida y corrigió el supuesto equivocado del banco.

La aprobación se sostiene para los fixtures y colaboradores observados. No acredita integración universal de estas clases ni independencia transitiva de bibliotecas del EXE.
