<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Guardia de restauracion de cabezas: propuesta adicional

No aplicada ni compilada ni ejecutada. scope_restore_after_isolation.patch se genera contra el banco con world_heads_isolation.patch incorporado en memoria; no aplicar contra banco sin aislamiento. Ningun instrumento actual cambia.

El guard toma referencias a regions/saved ya creadas y rellenas al comienzo de Update. Se construye dentro de cada run, despues de CleanRefs/restoreGlobals y antes de memset de regular/repeat y de inyectar nueve celdas. Su vida cubre todo el fixture, target y construccion del snapshot retornado. Al salir normalmente o por excepcion C++ ejecuta solo el mismo bucle memcpy de restoreGlobals, marcado noexcept, sin nuevas asignaciones, logs, PumpMessages ni llamadas GTA. Las referencias apuntan a vectores exteriores que siguen vivos durante el destructor.

La restauracion normal ya presente permanece intacta; duplicar memcpy es idempotente. El guard no registra trazas, no usa RNG y no modifica gates ni limites de fixtures. No elimina diferencias. Comparaciones/Write exteriores reciben ambos lados con las cabezas restauradas.

Cubre unwind C++ que ejecute destructores. No promete restauracion tras terminate, cierre forzado del proceso o SEH que no produzca unwind segun las opciones reales de compilacion. No certifica cleanup de referencias, pools o hooks tras excepcion: este guard restaura regiones globales guardadas, especialmente cabezas. Si se maneja error por catch, hacerlo despues de salir del run y restaurar heads antes de emitir Write/Pump. Conservar requisito final state_restored.

Los limites son 120x120 regular (115200 bytes) y 16x16 repeat (3072), con CSector8/CRepeat12. Snapshot de arrays completo y sin recorrer/destruir nodos reales. Rechazo de GetBoundCentre externo del parche previo continua activo; este guard no lo altera.

Revision offline: contextos unicos, aislamiento aplicado solo a string local, destructor de copias sin alloc/log/calls de juego y construct antes de vaciado. No build/GTA/Git/produccion.
