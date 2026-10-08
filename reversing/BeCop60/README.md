<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
