<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio, pipeline de edificios y decisiones (lista de 7)

Validadas el 02/10/2026 con **11.776 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEFireAudioEntity` | `Initialise`, `StaticInitialise`, `Terminate`, `AddAudioEvent`, `PlayFireSounds`, `PlayWaterSounds`, `UpdateParameters` |
| `CAETwinLoopSoundEntity` | `Initialise`, `UpdateTwinLoopSound`, `UpdateParameters`, `SwapSounds`, `StopSoundAndForget`, `DoSoundsSwitchThisFrame`, `PlayTwinLoopSound` |
| `CAEWaterCannonAudioEntity` | Constructor, `Initialise`, `Terminate`, `SetSplashInfo`, `ClearSplashInfo`, `Service`, `UpdateParameters` |
| `CAEMP3TrackLoader` | Constructor, Destructor, `Initialise`, `LoadStreamPackTable`, `LoadTrackLookupTable`, `GetTrackInfo`, `IsCurrentAudioStreamAvailable` |
| `CCustomBuildingPipeline` | `CreatePipe`, `DestroyPipe`, `CustomPipeAtomicSetup`, `CreateCustomObjPipe`, `CustomPipeMaterialSetup`, `CustomPipeRenderCB`, `CustomPipeReinstanceCB` |
| `CDecisionMakerTypesFileLoader` | `ReStart`, `GetPedDMName`, `GetGrpDMName`, `UnloadDecisionMaker`, `LoadDefaultDecisionMaker`, `LoadDecisionMaker` (enum y puntero) |

**Diferencias reales corregidas:**
- `CAEFireAudioEntity`:
  - `StaticInitialise` carga el banco de explosiones (0x34) en el slot 4; la DLL cargaba el de lluvia.
  - `PlayFireSounds` trata el índice de frecuencia como byte con signo (`movsx` + `idiv`).
  - Las voces sin variable de cliente propia quedan con -1, el valor de `CAESound::Initialise` (la DLL ponía 0).
  - `UpdateParameters`: NaN cuenta como volumen alcanzado y también detiene la voz (`test ah,5` / `test ah,41h`).
- `CAETwinLoopSoundEntity`:
  - `Initialise` escribe solo sus campos; la DLL asignaba un temporal completo (base, `m_tempSound` y la hora de intercambio).
  - `PlayTwinLoopSound` detiene las voces previas sin anular los punteros ni tocar `m_IsInUse`, y la variable de cliente queda en -1.
- `CAEWaterCannonAudioEntity`:
  - el constructor solo escribe la vtable, `m_tempSound.m_PhysicalEntity` y `m_bInitialized`;
  - las voces nuevas se preparan en `m_tempSound` (no en una `CAESound` local), con rolloff, velocidad y flags fijados tras `Initialise`.
- `CAEMP3TrackLoader`:
  - `LoadStreamPackTable`/`LoadTrackLookupTable` usan la API Win32 del original: si el fichero falta devuelven false sin crearlo ni tocar la tabla; los paquetes se cuentan con los bytes leídos y los tracks con el tamaño del fichero;
  - `GetTrackInfo` acepta `trackId == m_nTrackCount` (`ja`), reserva `tTrackInfo` sin inicializar y antes que el stream, y construye la ruta con `operator new` y `lstrcpyA`/`lstrcatA`;
  - `IsCurrentAudioStreamAvailable` no añade una barra extra tras la ruta del DVD ni marca `m_bInitialised`;
  - `Initialise` resuelve la ruta del DVD por sí mismo y comprueba el `FILE*` con signo.
- `CCustomBuildingPipeline`:
  - `CreateCustomObjPipe` pide la definición del nodo una sola vez, justo tras crear la pipeline;
  - `CustomPipeRenderCB`: factor de textura opaco con el brillo en RGB (`brillo · 1/255 · 254`), escala en la matriz global 0xC02C70, alfa por vértice con la función interna 0x7FE0A0 y número de mallas con signo;
  - `CustomPipeReinstanceCB` devuelve TRUE si no hay callback.
- `CDecisionMakerTypesFileLoader`:
  - `ReStart` descarga los decision makers de misión (10..19), no los del juego;
  - `LoadDefaultDecisionMaker` carga primero los índices de eventos y cada decision maker por defecto en uno local reiniciado que luego se copia entero;
  - `LoadDecisionMaker` lee con `CFileMgr::ReadLine` y el formato con comas del EXE en una sola llamada, y copia una decisión local completa.

**Método:**
- Se simulan y se trazan:
  - el banco de sonidos y el gestor (peticiones con todos sus campos y su origen, cancelaciones), `CAESound::SetPosition`, `StopSound` y `StopSoundAndForget`;
  - `FxSystem_c::GetCompositeMatrix`/`GetPlayStatus`, `rand`;
  - `CFileMgr` (directorio, apertura, líneas, cierre), `getDvdGamePath`, `CMemoryMgr`, `operator new`/`delete` (arena fija, solo durante la llamada) y `CAEDataStream::Initialise`/`FillBuffer`;
  - `CreateFileA`, `GetFileSize`, `ReadFile` y `CloseHandle` por IAT, solo en el hilo principal;
  - las primitivas `RxPipeline`/`RpGeometry`/`RpMatFX`, el estado D3D9 de RW y un dispositivo D3D9 falso con las cachés de shaders, declaración e índices;
  - el alta, la baja y la carga de índices de `CDecisionMakerTypes`.
- Entidades, materiales, atómicos, entradas de recursos, cañón de agua y decision makers son falsos; los punteros a voces apuntan a voces falsas.
- Se guardan y restauran el índice de frecuencia del fuego, el reloj de pausa, `ObjPipeline`, la matriz 0xC02C70, las cachés D3D9 y los decision makers por defecto de la instancia.
- En `Service`, `m_tempSound` se compara por los campos de la petición: su contenido completo depende de si `CAESound::Initialise` está enganchado.
- El fichero `audio7-only.txt` en la carpeta del juego limita el banco a una función.

Repetir: `python reversing/Audio7/build_probe.py` y `python reversing/run.py --case audio7`. Evidencia: [validation-summary.json](validation-summary.json).
