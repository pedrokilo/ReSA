<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
