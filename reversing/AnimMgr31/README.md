<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de animaciones (lista de 31)

Validadas el 04/10/2026 con **11.072 pares original/candidato dentro de GTA**, todos exactos: 31 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAnimManager` | `Initialise`, `Shutdown`, `ReadAnimAssociationDefinitions`, `LoadAnimFiles`, `LoadAnimFile`, `GetAnimationBlock` (por nombre), `GetAnimationBlockIndex` (por nombre), `GetFirstAssocGroup`, `GetAnimation` (por hash y por nombre), `GetAnimGroupName`, `GetAnimBlockName`, `CreateAnimAssociation`, `GetAnimAssociation` (por id y por nombre), `AddAnimation` (por id y por jerarquía), `AddAnimationAndSync`, `AddAnimAssocDefinition`, `AddAnimToAssocDefinition`, `CreateAnimAssocGroups`, `RegisterAnimBlock`, `RemoveLastAnimFile`, `RemoveAnimBlock`, `AddAnimBlockRef`, `RemoveAnimBlockRefWithoutDelete`, `GetNumRefsToAnimBlock`, `UncompressAnimation`, `RemoveFromUncompressedCache`, `BlendAnimation` (por jerarquía y por id) |

**Diferencias reales corregidas:**
- `GetAnimationBlock` y `GetFirstAssocGroup` comparan con `_stricmp` (antes `ci_string_view`), con el contador de bloques con signo.
- `GetAnimation`, `RemoveLastAnimFile` y `RemoveAnimBlock`: el número de animaciones del bloque es con signo (antes sin signo: con un valor negativo recorrían 4 G entradas); los índices no se comprueban.
- `AddAnimAssocDefinition`:
  - salta las definiciones ya ocupadas y deja el contador en la primera libre;
  - `strcpy` en vez de `strcpy_s` (que en Debug rellena con 0xFE);
  - dos bloques con `operator new` escalar y solo el primer byte de cada nombre a cero (antes `new[]` y `memset` de todo);
  - marca como libre la definición siguiente si el contador queda por debajo de 145.
- `AddAnimToAssocDefinition`: `strcpy` sin límite ni assert sobre el número de animaciones.
- `ReadAnimAssociationDefinitions`: `sscanf` sin tamaños y sin `VERIFY` del número de campos.
- `RegisterAnimBlock`: `strncpy` de 16 (sin terminador con nombres largos), el grupo se busca con el nombre ya copiado y sin assert de referencias.
- `AddAnimation` (jerarquía): asigna los flags (antes OR).
- `UncompressAnimation`: sin asserts; con la caché llena libera el último de la lista de usados y anula el `m_Link` de su jerarquía sin mirarlo, y el segundo `Insert` no se comprueba.
- `RemoveFromUncompressedCache`: sin el assert sobre el dato del enlace.
- `BlendAnimation` (las dos):
  - un `m_BlendAmount` NaN también pone el delta a -1 (`test ah,41h`);
  - la comparación con el delta actual es estricta (`bd < delta`, antes `<=`);
  - el mínimo con -0,05 conserva un NaN (antes `std::min` lo cambiaba);
  - en la de jerarquía, sin assert de jerarquía nula y el segundo `UncompressAnimation` usa la jerarquía de la asociación;
  - en la de id, la asociación estática se pide tres veces, como el EXE.
- `CreateAnimAssociation` y `GetAnimAssociation`: índice crudo del array de grupos (el `span` abortaba en Debug fuera de rango).
- `LoadAnimFile` reescrita a partir del EXE (antes seguía la documentación de gtamods):
  - "ANP2"/"ANP3" por `strncmp`; cualquier otra cabecera se lee como ANPK;
  - en ANP2/ANP3 la compresión la decide el tipo de frame de la primera secuencia (también en ANP3), los tipos desconocidos no reservan frames y en ANP3 se marca la memoria externa;
  - en ANPK el bucle usa el número de animaciones del bloque, la lista de no comprimidas se recorre entera (sin `break`), los quaternions se guardan conjugados y los keyframes comprimidos se convierten con `_ftol2` (·4096, ·1024 y tiempo ·60 + 0,5);
  - las secuencias se reservan con `operator new` escalar (n·12 + 4) y el contador delante;
  - el número de animaciones solo crece si el último índice supera al actual.

**Método:**
- Las tablas globales del EXE (definiciones con sus 145 huecos, contadores, bloques, jerarquías 2440-2499, caché LRU y `ms_modelInfoPtrs[7]`) se guardan, se rellenan con fixtures durante el caso y se restauran tras cada ejecución; nunca se carga un IFP real ni se toca el almacén real.
- Grupos, asociaciones estáticas, asociaciones, clump (byte de tipo 2 y plugin en el offset real de `0xB5F878`), enlaces y jerarquías de la caché, el fichero IFP (ANP3, ANP2, ANPK y cabecera desconocida) y las líneas de `ANIMGRP.DAT` son búferes estáticos del banco: los punteros se comparan en crudo.
- Se simulan y trazan sin reservar memoria, solo durante la llamada (arena fija rellena con 0xCD):
  - `operator new`/`delete`, `CMemoryMgr::Malloc`/`MallocAlign`/`FreeAlign`;
  - `RwStreamOpen`/`Read`/`Close`, `CFileMgr::SetDir`/`OpenFile`/`CloseFile`, `CFileLoader::LoadLine`, `CStreaming::RemoveModel`;
  - `CAnimBlendHierarchy::Shutdown`/`CalcTotalTime`/`CalcTotalTimeCompressed`/`RemoveQuaternionFlips`/`RemoveUncompressedData`/`Uncompress`;
  - `CAnimBlendAssocGroup::CopyAnimation`/`CreateAssociations`/`DestroyAssociations` y `CAnimBlendAssociation::Start`/`SyncAnimation`/`ReferenceAnimBlock`;
  - `CAnimManager::UncompressAnimation` (salvo cuando es la probada), `CreateInstance` de un model info falso (vtable con trampas en el resto de huecos), `RpAnimBlendClumpInit`, `GetFirstAtomic`, `RpSkinGeometryGetSkin`, `RpClumpForAllAtomics` y `RpClumpDestroy`.
- El constructor de asociaciones, los de grupos y secuencias, `SetName`/`SetBoneTag`/`SetNumFrames` y `GetAnimation` de los grupos corren de verdad (ya validados).
- Se compara el retorno, las tablas globales, la caché, grupos, estáticas, asociaciones, clump, el hash de la arena (vtable y callback por defecto de la DLL leídos como los del EXE, `aStdAnimDescs` de la DLL como `0x8A7788`), la posición del fichero y la traza.
- Fuera de los fixtures: secciones o ficheros cortos (el EXE lee memoria de pila sin inicializar), nombres sin terminador en 24 bytes, contadores de secuencias o frames negativos y cabeceras de `ANIMGRP.DAT` con menos de 4 campos.

Repetir: `python reversing/AnimMgr31/build_probe.py` y `python reversing/run.py --case animmgr31`. Evidencia: [validation-summary.json](validation-summary.json).
