<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tarea de policía de a pie (CTaskComplexBeCop)

Banco preparado el 05/10/2026 (sin ejecutar todavía): **1.536 pares original/candidato** previstos para 3 funciones del catálogo más el constructor.

| Función | Dirección | Casos |
|---|---|---|
| `Clone` (virtual, slot 1 de 0x85A428) | 0x499440 | 512 |
| Constructor (no catalogado, sin gancho) | 0x4993D0 | 512 |
| `Destructor` | 0x4994E0 | 256 |
| `GetTaskType` (virtual, slot 4) | 0x499430 | 256 |

**Diferencias corregidas en `Clone` y el constructor:**
- `Clone` clonaba la subtarea sin comprobar nulo; el EXE solo la clona si no es nula (llamada virtual `[vtable+4]`) y lo hace **antes** de reservar la tarea (`CTask::operator new`, 0x50 bytes).
- `m_nDir` se pasaba por un `bool`; el EXE lo lee con `movzx` y lo pasa como byte. El constructor ahora recibe `uint8`.
- El gancho de `Clone` ya no lleva `.reversed = false`.

**Método:** objeto en un buffer con vptr del EXE y campos aleatorios (`m_nDir` 0..255, estado de movimiento con valores extremos), subtareas falsas con vtable trazada (el `Clone` falso anota los slots ocupados del pool, así se comprueba el orden clonar/reservar), tareas nuevas del pool serializadas y borradas con traza, pool restaurado y slots libres con 0xCD en cada ejecución. El constructor se compara sobre 0x80 bytes con 0xCD (EXE frente a `new` de colocación del DLL), sin vptr y con el tipo. En el original se desactivan también los ganchos auxiliares `CTaskComplexWander::Constructor`, `CTask::operator new/delete` y `CTaskPool::New`.

Repetir: `python reversing/BeCop60/build_probe.py` y `python reversing/run.py --case becop60`.
