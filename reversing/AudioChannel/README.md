<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEAudioChannel

Validadas el 02/10/2026 con **5.184 pares original/candidato dentro de GTA**, todos exactos: las 12 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `Lost`: dos llamadas a `Restore()` por vuelta, como el original (`UpdateStatus` lo hereda).
- `SetVolume`: la diferencia de volumen se trunca a entero antes de compararla con 60.
- `SetFrequencyScalingFactor`: la frecuencia escalada se trunca a 16 bits.

- `SetReverbAndDepth` y `SetNotInRoom`: la DLL tenía el EAX desactivado (`#if 0`). Ahora llaman a `IKsPropertySet::Set` como el original, con los GUID de propiedades y la tabla de entornos del propio EXE (0x85EFDC, 0x85EF9C, 0x8AC198). El índice del entorno es `int8` con signo, la sala es `100 * depth` y en la calle vale 0 (dentro, -10000).

**Método:** buffers DirectSound falsos (vtables COM con un stub por método que registra los argumentos y devuelve valores controlados), un `IKsPropertySet` falso que registra cada propiedad y su valor, y `AESmoothFadeThread` controlado. El IID de `QueryInterface` se compara por contenido.

Repetir: `python reversing/AudioChannel/build_probe.py` y `python reversing/run.py --case channel`. Evidencia: [validation-summary.json](validation-summary.json).
