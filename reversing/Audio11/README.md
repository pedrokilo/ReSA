<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Voz sin peatón, decodificador WMA, ropa y aceleración de colisiones (lista de 11)

Validadas el 02/10/2026 con **13.632 pares original/candidato dentro de GTA**, todos exactos: 43 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEPedlessSpeechAudioEntity` | Constructor, `Initialise`, `StopCurrentSpeech`, `AddSayEvent`; virtuales `UpdateParameters`, `Terminate`, `PlayLoadedSound`, `GetAllocatedVoice`, `WillPedChatAboutTopic`, `GetPedType`, `IsPedFemaleForAudio` |
| `CAEWMADecoder` | Constructor, Destructor, `InitLibrary`, `SelectStreamIndex`; virtuales `Initialise`, `FillBuffer`, `GetStreamLengthMs`, `GetStreamPlayTimeMs`, `SetCursor`, `GetSampleRate`, `GetStreamID` |
| `CClothes` | `Init`, `LoadClothesFile`, `ConstructPedModel`, `RequestMotionGroupAnims`, `RebuildPlayer`, `RebuildPlayerIfNeeded`, `RebuildCutscenePlayer`, `GetDependentTexture`, `GetPlayerMotionGroupToLoad`, `GetDefaultPlayerMotionGroup` |
| `CColAccel` | `isCacheLoading`, `startCache`, `endCache`, `addCacheCol`, `cacheLoadCol`, `addColDef`, `getColDef`, `setIplDef`, `getIplDef`, `cacheIPLSection`, `addIPLEntity` |

**Diferencias reales corregidas:**
- `CAEPedlessSpeechAudioEntity` (ya no usa las plantillas compartidas con `CAEPedSpeechAudioEntity`):
  - el constructor y `Initialise` solo escriben los campos del EXE (la asignación de un objeto nuevo pisaba `m_tempSound`, la voz, el hueco y la referencia sin `CleanUpOldReference`);
  - `StopCurrentSpeech` deja de hablar y libera el hueco;
  - `AddSayEvent`: contexto con signo de 16 bits, tipo GFD y voz 10 antes de seguir, `(entidad - cámara)² > 3600` (a 60 m exactos sigue), primero el hueco (0..4) y después el sonido, y la referencia a la entidad solo cuando la frase sale;
  - `UpdateParameters` coloca el sonido solo si no es frontal **y** es de voz, con la posición de la entidad asociada;
  - `PlayLoadedSound` prepara `m_tempSound` (no una `CAESound` local), la registra con la entidad y suelta la referencia antes de pedir el sonido; comprueba el tipo jugador al fallar;
  - `Terminate` solo reinicia los campos del EXE.
- `CAEWMADecoder`:
  - las virtuales se instalan en la vtable 0x862D90 (`RH_ScopedVirtualClass` + `NOTSA_EXPORT_VTABLE`): el hook plano saltaba al thunk virtual;
  - el destructor no pone `m_bInitialized` a falso ni llama a `CoUninitialize`;
  - `Initialise` guarda el resultado en `m_bInitialized` en todas las salidas, libera las propiedades aunque `GetOutputProps` falle, usa `operator new`/`delete` escalares y pregunta la duración por la vtable;
  - `FillBuffer` no comprueba `GetBufferAndLength`, no libera las muestras de otro stream y guarda el sobrante desde el inicio del buffer;
  - `GetStreamPlayTimeMs` multiplica por 0.0001 (0x858FC4) en x87; `SetCursor` multiplica en 32 bits;
  - `InitLibrary` compara el módulo con -1, no lo anula al fallar y no llama a `FreeLibrary` si falla `WMCreateSyncReader`;
  - `SelectStreamIndex` devuelve el error de `GetStreamCount` y el último `HRESULT`, y no libera el stream de audio ni el que falla.
- `CClothes`:
  - `Init` deja las claves de `PlayerClothes` a -1;
  - `LoadClothesFile` reinicia las reglas, compara las etiquetas sin distinguir mayúsculas y lee siempre dos argumentos;
  - `GetPlayerMotionGroupToLoad` repite las lecturas de estadísticas y con NaN devuelve el grupo del jugador;
  - `GetDefaultPlayerMotionGroup` indexa los grupos sin comprobar el rango;
  - `GetTextureDependency` (ya validada) se engancha con un envoltorio que conserva `ecx` y `edx`, que usan `Init` y `CPedClothesDesc::SetTextureAndModel` del EXE.
- `CColAccel`:
  - `startCache` reserva sin cabecera y solo construye los rectángulos;
  - `endCache` libera las listas solo si existen;
  - `cacheLoadCol` usa el modelo empaquetado de 32 bits;
  - `setIplDef`/`getIplDef` copian los 0x34 bytes con relleno;
  - `addIPLEntity` guarda entidad y LOD en el orden del EXE, cruza los bits de distancia lejana y colisión propia, y marca colisión solo si difiere de la del LOD **y** existe.

**Método:**
- Objetos falsos para la voz (vtable del EXE), el decodificador WMA, el jugador, la ropa, la info de modelo, el pool de TXD y los grupos de animación.
- COM falso para Windows Media (lector, perfil, flujos, propiedades, cabecera y muestras) con vtables trazadas.
- `LoadLibraryA`, `GetProcAddress`, `FreeLibrary` y `CoInitialize` por IAT solo en el hilo principal y solo durante la llamada.
- Se simulan `CAESound`, `CAESoundManager::RequestNewSound`, las referencias de entidad, las consultas de `CAEPedSpeechAudioEntity`, `CStats::GetStatValue`, `CStreaming`, `CTimer`, `CClothesBuilder`, `CFileMgr`, `CFileLoader::LoadLine`, `CMemoryMgr`, `operator new`/`delete` y las funciones de modelo y colisión.
- Se guardan y restauran los huecos de voz, las banderas de voz, la cámara, el estado del juego, `WMCreateSyncReader` y su módulo, las reglas de ropa y `PlayerClothes`, los globales de `CColAccel`, las entradas 19990..19997 de `ms_modelInfoPtrs`, el pool de TXD y los grupos de animación.
- Fuera de los fixtures:
  - etiquetas de ropa desconocidas (el EXE reutiliza un valor sin inicializar);
  - punteros nulos en los arrays de `CColAccel` salvo en `endCache`;
  - modelos empaquetados con fin distinto de cero en `cacheLoadCol` (el EXE leería fuera del array);
  - el contador y el tamaño sin inicializar que el EXE pasa a `GetAttributeIndices` y `GetMediaType`.

Repetir: `python reversing/Audio11/build_probe.py` y `python reversing/run.py --case audio11`. Evidencia: [validation-summary.json](validation-summary.json).

## Actualización 11/10/2026

`AddSayEvent` fallaba 4/1024 porque la fuente leía `s_pVehicleAudioSettingsForRadio` (0xB6B99C) donde el EXE lee `s_pPlayerAttachedForRadio` (0xB6B98C); corregido en seis sitios de voz. Con la corrección, el banco aprueba con 13632 pares (corrida 20261011-231624-593138). Detalle en `reversing/CppAudioX87Study/README.md`; evidencia en `reversing/CppAudioSpeechFix`.
