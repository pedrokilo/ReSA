<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Secuencias de animación y pipeline día/noche de edificios (lista de 14)

Validadas el 02/10/2026 con **6.784 pares original/candidato dentro de GTA**, todos exactos: 28 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAnimBlendSequence` | Constructor, Destructor, `SetName`, `SetBoneTag`, `GetDataSize`, `SetNumFrames`, `Uncompress`, `CompressKeyframes`, `MoveMemory`, `Print`, `RemoveQuaternionFlips`, `RemoveUncompressedData`, `GetUKeyFrame`, `GetCKeyFrame` |
| `CCustomBuildingDNPipeline` | `ExtraVertColourPluginAttach`, callbacks del plugin (constructor, destructor, lectura, escritura, tamaño), `PreRenderUpdate` (atomic y clump), `NVCPipelineProcess`, `CreatePipe`, `DestroyPipe`, `CreateCustomObjPipe`, `CustomPipeAtomicSetup`, `SetFxEnvTexture` |

**Diferencias reales corregidas:**
- `CAnimBlendSequence`:
  - `Uncompress` reservaba el tamaño comprimido (16/10 bytes por frame) y escribía 32/20: desbordamiento del bloque;
  - `Uncompress` multiplica el tiempo por 0x3C888889 (1/60) en vez de dividir entre 60;
  - `CompressKeyframes` trunca con `_ftol2` (64 bits) y sin asserts de estado;
  - contadores de frames con signo (`movsx`) en `GetDataSize`, `Uncompress`, `CompressKeyframes` y `RemoveQuaternionFlips`;
  - `SetNumFrames` añade los flags con OR (no borra traslación ni compresión) y calcula el tamaño con los argumentos;
  - `RemoveQuaternionFlips`: producto escalar x87 en el orden del EXE y paso según la traslación aunque esté comprimida;
  - el constructor pone a cero la palabra de flags completa y deja sin escribir los bytes 2-3.
  - `SetBoneTag` compara el valor de 32 bits con -1: `eBoneTag` es `int16` y la comparación por enum descartaba cualquier etiqueta con la palabra baja 0xFFFF.
- `CCustomBuildingDNPipeline`:
  - `CreateCustomObjPipe` guarda 0x53F20098 en `pluginId` y `pluginData` (la DLL ponía 0);
  - `SetFxEnvTexture` usa el `CPool::New` del EXE (0x5D9110: empieza en `m_LastFreeSlot + 1`, sin relleno 0xCD) y copia los 12 bytes con el relleno;
  - `PreRenderUpdate`: umbrales `!(d > c)` (NaN sale como un valor pequeño);
  - callbacks del plugin: el constructor solo escribe con offset positivo, el destructor solo libera punteros no nulos, el tamaño devuelve 0 sin geometría y la escritura reproduce el EXE (primer color como puntero);
  - `ExtraVertColourPluginAttach`: offset a -1 antes de registrar y fallo del stream con cualquier negativo;
  - `NVCPipelineProcess` lee colores y prelit antes de bloquear; `DestroyPipe` anula el puntero después de destruir.

**Método:**
- Secuencias, geometrías, atomics, clumps, pipelines, nodos y el pool de env map son búferes falsos; el estado global (offset del plugin, balance, `ObjPipeline`, contadores, datos falsos de env map, puntero del pool) se guarda y se restaura.
- `CMemoryMgr` (EXE y DLL) usa una arena propia; flujos RW, registro de plugins, lock/unlock, materiales, `SetPipelineID` y primitivas Rx/D3D9 simulados y trazados. Nunca se toca el dispositivo ni una pipeline real.
- Los átomos de un clump ocupan franjas separadas del búfer para que ninguno pise la geometría de otro.
- Punteros a funciones de la DLL equivalentes a una del EXE se normalizan a la dirección del EXE.
- Se mantienen dos desviaciones de la DLL y se dejan fuera de los fixtures: el bugfix activo de `SetName` (borra el bit de bone tag) y el NOTSA de `SetNumFrames` (`m_bUsingExternalMemory = frameData != nullptr`).
- No cubierto: contadores negativos con reserva propia (el EXE pide bloques enormes) y lecturas de stream con 0 vértices y colores nocturnos (`VERIFY` de la DLL).

Repetir: `python reversing/Render14/build_probe.py` y `python reversing/run.py --case render14`. Evidencia: [validation-summary.json](validation-summary.json).
