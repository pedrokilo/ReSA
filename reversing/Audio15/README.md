# Radio de usuario y grupos de animaciones (lista de 15)

Validadas el 02/10/2026 con **11.008 pares original/candidato dentro de GTA**, todos exactos: 30 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEUserRadioTrackManager` | `Initialise`, `Shutdown`, `ReadUserTracks`, `GetTrackPath`, `LoadUserTrack`, `ResolveShortcut`, `GetAudioFileType`, `IsShortcut`, `SelectUserTrackIndex`, `SetUserTrackIndex`, `GetUserTrackPlayMode`, `DeleteUserTracksInfo`, `ScanUserTracks`, `WriteUserTracksFile`, `WriteUserTracksThread` |
| `CAnimBlendAssocGroup` | Constructor, Destructor, `CreateAssociations` (1, 2 y 3), `DestroyAssociations`, `InitEmptyAssociations`, `GetAnimation` (nombre e id), `CopyAnimation` (nombre e id), `GetAnimationId`, `GetNumAnimations`, `GetAnimBlock`, `IsCreated` |

**Diferencias reales corregidas:**
- `CAEUserRadioTrackManager`:
  - la tabla de extensiones es la del EXE (0x8CBB28, 8 entradas, faltaba `.mov`); `Initialise` reescribe ahí el tipo de `.mp3` y solo escribe los decoders 0..4 (antes ponía a cero los 8);
  - `Shutdown` no pone el puntero a nulo y `ReadUserTracks` no libera la copia anterior (las dos cosas quedan tras el arreglo reversible `GenericUB`); `ReadUserTracks` exige un handle positivo y reserva con `Malloc(size, 0)`;
  - `GetTrackPath` solo falla con un handle negativo (`GenericCrashing` mantiene la comprobación de nulo);
  - `LoadUserTrack`: el tipo 0 (desconocido) libera el stream en vez de crear un decoder Vorbis;
  - `SelectUserTrackIndex`: con una sola pista pide igualmente un número aleatorio (salía 1 sin llamar a la RNG) y el índice secuencial va con división sin signo;
  - `DeleteUserTracksInfo` borra con `DeleteFileA` (antes `std::filesystem::remove`);
  - `WriteUserTracksFile`, `WriteUserTracksThread` y `ResolveShortcut` transcritos del EXE: `FindFirstFileA`/`FindNextFileA` con rutas ANSI, longitudes de 16 bits, `push_back` del vector VC6 (0x4F4620), `.lnk` solo en el primer nivel, `IShellLinkA` con `SLGP_SHORTPATH`, el hilo sin relectura del índice, sin comprobar `sa-utrax.dat`, con el registro duplicado cuando hay una sola pista y saliendo sin `CoUninitialize` si falla `sa-ufiles.dat`. La versión UTF-8 con relectura inmediata queda tras el nuevo arreglo reversible `CAEUserRadioTrackManager_UnicodeUserTracks` (activo por defecto).
- `CAnimBlendAssocGroup`:
  - el destructor (0x4CE1D0) no recibe argumentos (`ret` sin pila; la envoltura hacía `ret 4`);
  - `DestroyAssociations` solo toca `m_NumAnims` si hay array y, con contador 0, libera el bloque con `operator delete` sin pasar por la vtable;
  - `IsCreated` mira `m_Anims`, no `m_NumAnims`;
  - `InitEmptyAssociations` recorre de delante hacia atrás; los bucles son con signo y `GetAnimation(id)` no comprueba el rango;
  - `CreateAssociations`: `m_NumAnims` arranca en 0 y cuenta cada vuelta; en la variante de buffers paralelos gana la última coincidencia de nombre; sin registros spdlog.

**Método:**
- El gestor de la radio es falso, con su array de pistas; `CFileMgr` se simula en el EXE y en la DLL con handles falsos (nunca se abre ni se borra nada).
- Árbol de carpetas virtual: `FindFirstFileA`/`FindNextFileA`/`FindClose`/`GetFileAttributesA` son función pura de la ruta y la semilla del caso. `IShellLinkA`/`IPersistFile` son falsos. `CreateThread` no crea hilos y `TerminateThread`/`DeleteFileA`/`CoInitialize`/`CoUninitialize` solo registran. Todo por las IAT del EXE y de la DLL, solo alrededor de cada llamada y solo en el hilo principal; `DeleteUserTracksInfo` no se llama si sus huecos no están sustituidos.
- Los constructores reales de `CAEDataStream` y de los decoders corren en el arena con sus hooks forzados activos en las dos pasadas; vtables y callbacks normalizados.
- Los grupos usan asociaciones estáticas, jerarquías, bloque y model infos falsos; se simulan `GetAnimationBlock`, `GetAnimation`, `GetModelInfo*`, `CreateInstance`, `Init`, las funciones RW del skin y del clump, `UncompressAnimation` y `AddAnimBlockRef`. Los hashes de `ms_aAnimations` del bloque y `ms_lastPositionSearched` se fijan por caso y se restauran.
- El hilo de streaming de audio se aparca en `IsForegroundApp`; `GenericCrashing`, `GenericUB` y `CAEUserRadioTrackManager_UnicodeUserTracks` se apagan durante el banco y se restauran.
- operator new/delete y `CMemoryMgr::Malloc/Free` solo durante la llamada, con registros sin reservas.
- Fuera de los fixtures:
  - el decoder MediaFoundation (tipo 4, sustituye a QuickTime: NOTSA);
  - las variantes UTF-8 (arreglo reversible) y los arreglos `GenericCrashing`/`GenericUB`;
  - `operator new` nulo y tamaños de `new[]` que desbordan (el EXE no comprueba);
  - `GetAnimation` sin array con `m_NumAnims` distinto de cero (el EXE lee de nulo).

Repetir: `python reversing/Audio15/build_probe.py` y `python reversing/run.py --case audio15`. Evidencia: [validation-summary.json](validation-summary.json).
