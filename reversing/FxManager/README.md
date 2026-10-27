<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxManager_c — 25/25 funciones registradas

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

El 2026-09-22 se validó y habilitó `DestroyAllFxSystems` (0x4A98B0), la última entrada pendiente. También se corrigió el mismo defecto en `Update` (0x4A9A80), que ya figuraba como reconstruida. Las otras 23 entradas no se vuelven a certificar con este banco.

## Defecto y reconstrucción

Los dos bucles consultaban `m_pNext` después de destruir el sistema actual: un acceso a memoria liberada. El original de DestroyAllFxSystems guarda el siguiente nodo en ESI en 0x4A98C0 antes de llamar a DestroyFxSystem. Update lo guarda en EDI en 0x4A9ABB, antes incluso de llamar a FxSystem_c::Update. Ambos recorridos C++ conservan ahora ese orden.

Ejecutable analizado y copia de pruebas: SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Desensamblados locales: `artifacts/DestroyAllFxSystems.asm` y `artifacts/Update.asm`.

## Validación dentro del motor

**720 comparaciones aprobadas; 1.980 entradas al código candidato y 6.192 sistemas seleccionados destruidos por cada versión.** Las fases originales registraron cero entradas a los candidatos.

- **180 casos de DestroyAllFxSystems:** listas de 0, 1, 2, 8 y 32 sistemas; blueprint sin primitivas, un blueprint cargado y mezcla de dos; sin partículas, una por sistema/primitiva o dos con partículas de un sistema ajeno; matrices padre propias o ausentes. Cada caso incluye una segunda llamada sobre la lista vacía.
- **540 casos del recorrido de Update:** los mismos tamaños, tres distribuciones de estados FX_3/FX_KILLED, tres densidades de partículas, matrices propias o ausentes y pasos de tiempo 0, 0.016 y 0.2. Se ejecutan tres actualizaciones: eliminación inmediata o transición a FX_3, eliminación diferida y repetición con lista vacía. Incluye cálculo de frustum con cámara real y recorrido de un blueprint sin primitivas.
- La mitad de cada familia limpia el enlace del nodo ya retirado antes de liberar el objeto, mediante instrumentación temporal de DestroyFxSystem. Es una condición de estrés controlada y explícita; la otra mitad conserva el cuerpo de destrucción habitual.

Se comparan orden de destrucción, estados de los supervivientes, cabeza/cola/contador y enlaces de listas de partículas, orden de devolución, propiedad, conservación exacta de todas las partículas y contenido de cada partícula fuera de sus enlaces. Las partículas del sistema ajeno deben permanecer activas. Update compara además los bytes del frustum y las tres etapas del ciclo.

Desde el 04/10/2026 `DestroyFxSystem` devuelve las partículas, como el EXE, a la lista libre del gestor global (0xA9AE9C): el banco intercambia la lista libre del fixture con la de `g_fxMan` solo durante cada llamada (también en la limpieza) y la devuelve después, así el gestor real no se modifica.

Los sistemas, instancias de primitivas, matrices propias y partículas de cada fase tienen almacenamiento independiente. Los blueprints cargados comparten datos de definición de solo lectura durante las llamadas; sus listas mutables se intercambian temporalmente por las de cada fixture y se restauran antes de la otra fase. No se actualizan ni destruyen los efectos activos del jugador. Se verifican al final los bytes del manager global, los sistemas vivos y las primitivas/partículas conservadas de los blueprints utilizados. Las partidas originales y de prueba conservaron sus hashes.

Registro final: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fxmanager/20260922-194518-413631/comparisons.jsonl`.
SHA-256: `e1522295f076aedef9534234a89f560298c0ef1ff0fa2b82e0f1e73f55bf3ac7`.
Resumen: [validation-summary.json](validation-summary.json).

El ensayo anterior de 180 casos en `20260922-194123-298309` aprobó únicamente DestroyAllFxSystems. El registro final anterior es el que respalda ambas correcciones.

## Repetir sin agente

`PROBAR.cmd --case fxmanager`, o `python reversing/run.py --case fxmanager` para evitar la pausa interactiva del CMD. Reutiliza el runner compartido: verificación de hashes, bloqueo de concurrencia, respaldo/restauración del ASI, límite de 180 segundos y comprobación de partidas. Solo cierra el proceso que inicia.

La sonda archivada está en `_baseline/gta_reversed-fxmanager-probe.asi` dentro del juego de pruebas; hash fijado en `run_local.py`. `build_probe.py` conserva la instrumentación reproducible de `Probe.hpp` y `LiveComparison.hpp` y restaura las fuentes incluso si falla el build. El script genera la sonda en `artifacts`; no la instala. Una nueva compilación puede cambiar el hash: revisar el resultado y actualizar archivo/hash antes de repetir. Después de generar una sonda, recompilar el ASI normal antes de instalarlo.

La compilación normal Debug x86 terminó sin instrumentación y se instaló solo en la copia de pruebas. SHA-256: `6999d5daf6a7e4a9051ed773dda66dcd0d66e8572b1211bcbf9059b58c8b5176`. Respaldo del ASI/PDB anteriores: `D:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline/before-fxmanager-20260922`.

## Límites

Se compara cada función objetivo original con su reconstrucción, manteniendo dependencias compartidas: DestroyFxSystem y CalcFrustumInfo conservan sus hooks existentes. FxSystem_c::Update permanece original; sus casos ejercen las salidas FX_KILLED/FX_3, no toda la simulación o emisión de partículas. Los sistemas de prueba no reproducen sonidos activos ni renderizado de efectos. No se mide una sesión prolongada ni se certifican todas las funciones de FxManager, la ausencia general de fugas o el comportamiento de Exit/UnloadFxProject.

Los hooks Audio y Global/SurfaceInfos_c se excluyen según la configuración previa. No hubo revisión visual. La campaña integral de estabilidad sigue reservada para cuando todas las clases estén completas, según MEJORAS_DEL_PROCESO.md. No se modificó la instalación personal. Sin commit ni push.
