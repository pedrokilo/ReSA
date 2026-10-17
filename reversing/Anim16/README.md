<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Jerarquías y nodos de animación (lista de 16)

Validadas el 02/10/2026 con **14.080 pares original/candidato dentro de GTA**, todos exactos: 32 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAnimBlendHierarchy` | Constructor, Destructor, `Shutdown`, `RemoveAnimSequences`, `AllocSequenceBlock`, `FindSequence`, `SetName`, `CalcTotalTime`, `CalcTotalTimeCompressed`, `RemoveQuaternionFlips`, `GetSequenceBlock`, `Uncompress`, `CompressKeyframes`, `RemoveUncompressedData`, `MoveMemory`, `Print` |
| `CAnimBlendNode` | `Init`, `CalcTheta`, `CalcDeltas`, `CalcDeltasCompressed`, `FindKeyFrame`, `NextKeyFrame`, `NextKeyFrameCompressed`, `NextKeyFrameNoCalc`, `SetupKeyFrameCompressed`, `UpdateTime`, `GetCurrentTranslation`, `GetCurrentTranslationCompressed`, `GetEndTranslation`, `GetEndTranslationCompressed`, `Update`, `UpdateCompressed` |

**Diferencias reales corregidas:**
- `CAnimBlendHierarchy`:
  - el constructor no escribe `m_hashKey`;
  - `CalcTotalTimeCompressed` pasa el tiempo con la constante del EXE (`0x3C888889`, ·1/60) y resta los deltas en enteros de 16 bits (antes pasaba por float y redondeaba con +0,5);
  - `MoveMemory` estaba sin implementar: mueve el bloque compartido una vez y desplaza todas las secuencias, o mueve cada secuencia, y devuelve si algo cambió;
  - `RemoveUncompressedData` llama a `CAnimBlendSequence::RemoveUncompressedData`;
  - `FindSequence` calcula el hash antes de mirar el contador y compara la palabra completa (sin assert de bone tag);
  - contadores con signo (`movsx`), sin asserts y acceso crudo a los keyframes (el paso depende solo del bit de traslación).
- `CAnimBlendNode`:
  - `CalcTheta` es una función cdecl de 4 argumentos (quaternion anterior, actual, `&theta`, `&1/sin`), no un miembro con el coseno: el hook tenía la firma equivocada. Ahora en x87: producto en el orden del EXE, acos del CRT y `1/fsin` del resultado sin redondear;
  - `CalcDeltasCompressed` invierte el keyframe **actual** (no el anterior) y pasa a `CalcTheta` el quaternion ya invertido (antes, el coseno negativo);
  - `FindKeyFrame` relee el keyframe en cada vuelta (comparaba siempre con el keyframe 1);
  - `Init` no toca `m_Theta` ni `m_InvSinTheta`;
  - `NextKeyFrameNoCalc` estaba sin implementar;
  - la traslación se interpola como `(actual - anterior)·t + anterior`, no como `to·t + from·(1 - t)`;
  - los tiempos comprimidos se multiplican por `0x3C888889` (antes se dividían por 60);
  - el anterior se calcula como `actual - 1` en 16 bits y, si queda negativo, se le suma el número de frames;
  - `Update` no exige traslación ni rotación (se quita el assert).

**Método:**
- Jerarquías, secuencias (con la cookie de `new[]`), keyframes, nodos y asociaciones son búferes estáticos del banco: los punteros se comparan en crudo.
- Las secuencias usan el código real de `CAnimBlendSequence` y `CQuaternion::Slerp` llama al del EXE.
- Se simulan y trazan sin reservar memoria, solo durante la llamada y solo en el hilo principal: `CMemoryMgr::Malloc` (arena fija rellena con 0xCD), `Free`, `MoveMemory` (respuesta fija por puntero), `operator delete`/`delete[]`, `CAnimManager::RemoveFromUncompressedCache` y `UncompressAnimation`.
- Se compara el retorno, los bytes de la jerarquía, de las secuencias y del nodo, el hash de los keyframes y de la arena y la traza.
- Fuera de los fixtures:
  - secuencias sin frames en `CalcTotalTime*` y `Uncompress` (FIX_BUGS de la DLL: el EXE lee el keyframe -1);
  - contadores negativos (el EXE lee fuera del bloque);
  - cosenos < -1 en `CalcTheta` (el acos del CRT pasa por su rutina de error);
  - animaciones cíclicas con todos los tiempos a 0 (el bucle no termina, tampoco en el EXE).

Repetir: `python reversing/Anim16/build_probe.py` y `python reversing/run.py --case anim16`. Evidencia: [validation-summary.json](validation-summary.json).
