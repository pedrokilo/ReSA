<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# CIdleCam y CPedAttractor: funciones completadas

Reconstrucción estática con `reversing/_handoff/combo.sh`, pseudocódigo Ghidra 12.1.4 vía REA y ensamblador completo del EXE 1.0 US (sha256 72ae59e4…). **Validado el 07/10/2026: 2.048/2.048 pares exactos**, 1.024 por función; estado, configuración y partidas restaurados. [Evidencia](validation-summary.json).

| Función | Dirección | Pares exactos |
|---|---|---|
| CIdleCam::ProcessFOVZoom | 0x517BF0 | 1024 |
| CPedAttractor::AbortPedTasks | 0x5EAF60 | 1024 |

ProcessFOVZoom reconstruye cuatro estados de zoom, la decisión de línea de visión, el tratamiento de peatones tipos 5/22, zoom forzado, reloj sin signo y la interpolación con FSIN. Ensamblador 0x517BF0–0x517E8B: distancia sumada z²+y²+x²; flags de visión 1,0,0,1,0,0,1; resta/división/multiplicaciones antes de FSIN y orden exacto después.

AbortPedTasks corregido: 0x5EAF9B limpia Ped; 0x5EAFA7 llama al destructor virtual con flag 1; solo 0x5EAFAD anula Task. La implementación anterior anulaba Task antes del destructor. El bucle captura el tamaño, relee _First en cada iteración y conserva MatchID/UsedTask y tamaño/capacidad.

Banco en motor: objetos propios sin constructores/destructores de juego; funciones de posición y línea de visión simuladas en ambos lados; destructor virtual de tareas simulado y trazado incluyendo el estado del par durante la llamada. Compara bytes completos de objetos y trazas. Reloj, entidad ignorada, parches y hooks restaurados. Incluye PumpMessages y filtro por clase/función. No usa pools ni entidades reales para las llamadas probadas.

Límites: dependencias simuladas, sin regresión de cámara en juego real; no reconstruye ni revalida los otros miembros de estas clases. Fixtures finitos, sin NaN/infinito ni duración cero, sin destructores que cambien la colección. Compilación con `/MP6` y validación dinámica aprobadas sobre las fuentes finales.

<!-- BEGIN PENDING BATCH REGRESSION -->

Regresión del 07/10/2026: **APROBADA**. 2.048 pares exactos. [Resumen actualizado](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-idle-attractor-pending\20261007-201404-402476/summary.json>).
<!-- END PENDING BATCH REGRESSION -->
