<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Asociaciones de animación (lista de 20)

Validadas el 03/10/2026 con **10.368 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAnimBlendAssociation` | Constructores (por defecto, clump + jerarquía, copia de asociación, asociación estática), `Init` (las 3 sobrecargas), `AllocateAnimBlendNodeArray`, `FreeAnimBlendNodeArray`, `ReferenceAnimBlock`, `SetBlend`, `SetBlendTo`, `SetCurrentTime`, `SetDeleteCallback`, `SetFinishCallback`, `Start`, `SyncAnimation`, `UpdateBlend`, `UpdateTime`, `UpdateTimeStep` |

**Diferencias reales corregidas:**
- El constructor por defecto ya no escribe `m_pCallbackFunc` ni `m_pCallbackData` (NOTSA quitado).
- `AllocateAnimBlendNodeArray`:
  - pide el tamaño redondeado a múltiplo de 64 (`((n·24 - 1) >> 6 sin signo + 1) << 6`), como el EXE;
  - el bucle usa el contador con signo (con `n <= 0` no inicializa nada; antes recorría un `span` gigante).
- `Init` (las 3) y `SetCurrentTime`: el número de nodos y de secuencias se trata como `int16` (`movsx`/`jle`).
- `Init` (clump + jerarquía): busca el frame (bone o hash) antes de mirar el número de keyframes, y solo enlaza secuencias con más de 0 keyframes (con signo).
- `Init` (copia): los nodos se copian en crudo (`rep movs`).
- `UpdateBlend`:
  - tras el callback relee su tipo y deja un callback por defecto de **borrado** si sigue siendo de borrado (antes siempre de fin);
  - al pasar de 1 solo recorta un delta positivo (un NaN se conserva; `std::min` lo cambiaba por 0).
- `UpdateTime`: con un tiempo NaN tras avanzar devuelve `true` sin tocar nada (`fcomp` + `test ah,1`).

**Método:**
- Asociaciones, asociación estática, jerarquías, secuencias, clump (byte de tipo 2 y puntero del plugin en el offset real de `0xB5F878`), `CAnimBlendClumpData`, frame datas, nodos y arena son búferes estáticos del banco: los punteros se comparan en crudo.
- Se simulan y trazan sin reservar memoria, solo durante la llamada y solo en el hilo principal:
  - `CMemoryMgr::MallocAlign`/`FreeAlign` (0x72F4C0/0x72F4F0; arena fija rellena con 0xCD);
  - `CAnimManager::AddAnimBlockRef` y `UncompressAnimation` (no se toca el almacén de bloques ni la caché);
  - `CAnimBlendNode::FindKeyFrame` y `SetupKeyFrameCompressed` (ya validadas);
  - `RpAnimBlendClumpFindBone` y `RpAnimBlendClumpFindFrameFromHashKey` (respuesta fija por argumento: nulo, dentro o fuera de rango);
  - el callback (puede cambiar el tipo de callback) y el destructor virtual (vtable falsa: el `delete this` se traza sin liberar).
- Los constructores corren sobre búferes rellenos con 0xCD o aleatorios y se comparan enteros; el vptr se compara como tipo y el callback por defecto de la DLL se lee como el del EXE (0x4D1390).
- Se compara el retorno, los bytes de las dos asociaciones, el hash de la arena y de los nodos y la traza.
- Fuera de los fixtures:
  - número de frames del clump negativo en 16 bits con secuencias enlazadas (reserva nula y escritura en `nullptr`, también en el EXE);
  - animaciones cíclicas con duración 0 y tiempo >= 0 en `SetCurrentTime`/`Start`, y `SyncAnimation` con duración 0 en la otra asociación (el bucle no termina, también en el EXE);
  - el destructor (0x4CECF0) no está en la lista.

Repetir: `python reversing/Anim20/build_probe.py` y `python reversing/run.py --case anim20`. Evidencia: [validation-summary.json](validation-summary.json).
