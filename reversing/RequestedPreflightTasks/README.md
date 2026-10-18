<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lifetime y Expanded antes de gráficos

2026-10-08. Banco preparado para896pares básicos y192pares de ramas de destrucción, en procesos separados. Conserva intactos TaskLifetimeComparison.hpp, TaskLifetimeExpanded/Expanded.hpp y Frontend.hpp. No registra nuevas validaciones ni sustituye fallos anteriores.

El harness temporal entra en NOTSA_WinMain después de su guardia de cierre del registro y antes de SystemParametersInfo/RenderWare/input; devuelve0. Guarda todos los estados/entradas antes de desactivar los hooks salvo WinMain, usa el puente de marcadores de Frontend existente y restaura los estados originales. El runner mantiene WinMain activo, sin selector histórico ni argumento unhook, exige cierre natural0 y preserva logs/manifiesto.

CTimer inicial se trata como memoria del EXE mapeado, no como reloj del juego en funcionamiento. Se registran los12bytes de B7CB7C..B7CB87 y se exige que permanezcan iguales. No se acredita integración de timer, pools, atracción, entidades, animación ni gameplay. Los colaboradores aislados y child allocations del banco Expanded conservan sus contratos documentados; ver README original.

El analizador reutiliza exactamente los conteos y comprobaciones de ramas del banco congelado. El corpus896 básico tiene límites expresos en terminal vptr y colaboradores: coincidir en ese corpus no acredita una función completa. Expanded192 exige terminal vptr nativo, subtask borrado, trazas idénticas a expectativas y restauración de colaboradores. Ninguna disparidad se filtra.

Builder Debug `_CL_=/MP6`, exterior serial1; captura ASI/PDB y hashes de fuentes/instrumentos antes de finally. Restaura WinMain y binarios previos byte a byte. Root coordina ejecución; los agentes compilan en serie.
