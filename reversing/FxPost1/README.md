<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxPost1: banco por pares de FxSystem_c y CPostEffects

Banco simple (flujo `Small2`/`PadMouse1`/`GfxWave2`, dentro del juego completo con partida cargada, enganchado en `CGame::Process`) para las funciones pendientes de `docs/hooks.csv` (validated=0) de dos clases:

- `FxSystem_c`: 31 funciones (`FxPart.hpp`).
- `CPostEffects`: 35 funciones (`PostPart.hpp`).

Sin compilar ni ejecutar todavia. Comando: `bash reversing/_handoff/bench.sh FxPost1 3`. Filtro opcional: `fxpost1-only.txt` en la carpeta del juego con `Clase::funcion`.
`build_probe.py` y `run_local.py` redirigen en memoria `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test`.

## Estructura

`LiveComparison.hpp` (tabla de funciones en comentario + bucle principal) incluye `FxPostCommon.hpp` (trazas sin reservas, `Buf` con guardas, `Iso` = EntryPatch en la direccion del EXE y en el simbolo de la DLL con todos los `Body()` calculados antes de crear ningun parche) y las dos partes.

## Corpus: 66 funciones, 15488 pares

Original = hooks de la clase desactivados (EXE puro); candidato = hooks activos (DLL). Se comprueba `JumpsToDll` tras `State()`. Cada caso se genera de forma determinista (semilla por indice y funcion) y se compara: valor de retorno, bytes del objeto, hashes de todos los objetos falsos, rangos de globales tocados, control x87 y traza de colaboradores.

| Parte | Funcion | Casos |
|---|---|---|
| Fx | AddOffsetPos 256, AttachToBone 256, Constructor 128, CopyParentMatrix 128, Destructor 256, DoFxAudio 512, EnablePrim 128, Exit 256, ForAllParticles 512, GetBoundingSphereLcl 128, GetBoundingSphereWld 512, GetCompositeMatrix 256, GetPlayStatus 64, Init 512, IsVisible 512, Kill 256, Pause 128, Play 512, PlayAndKill 256, ResetBoundingSphere 256, SetBoundingSphere 256, SetConstTime 256, SetLocalParticles 128, SetMatrix 128, SetMustCreatePrts 128, SetOffsetPos 256, SetRateMult 256, SetTimeMult 256, SetVelAdd 128, SetZTestEnable 128, Stop 256 | 8000 |
| Post | CCTV 256, Close 64, ColourFilter 256, DarknessFilter 256, DoScreenModeDependentInitializations 128, DrawQuad 512, FilterFX_* 128+128, Fog 512, Grain 192, HeatHazeFXInit 256, ImmediateModeFilterStuffInitialize 256, ImmediateModeRenderStates{ReStore 128, Set 64, Store 128}, InfraredVision 256, InfraredVision{RestoreLights 64, SetLightsDefault 64, StoreAndSetLightsHeat 512}, Initialise 64, IsVisionFXActive 64, NightVision 256, NightVisionSetLights 64, Render 1024, Script* 64+128+64+64+64+64, SetFilterMainColour 256, SetSpeedFXManualSpeedCurrentFrame 256, SetupBackBufferVertex 256, UnderWaterRipple 512, Update 128 | 7488 |

(La tabla exacta, de la que `run_local.py` extrae `EXPECTED`, esta en el comentario de `LiveComparison.hpp`.)

## Intercepcion

- **FxSystem_c**: objetos falsos con guardas de 16 bytes (sistema de 0x104 bytes, plantilla, primitivas con vtable trazada, particulas, matrices, esferas y arena fija para `CMemoryMgr::Malloc`). Dobles con EntryPatch (EXE + simbolo de la DLL): `CMemoryMgr::Malloc` (0x72F420) y `Free` (0x72F430) (arena fija rellena con 0xCD; los hilos ajenos reenvian al `malloc`/`free` del CRT del EXE 0x824257/0x82413F), `rand` (0x821B1E) y `CGeneral::GetRandomNumber`, `CAEFireAudioEntity::AddAudioEvent` (0x4DD3C0) y `Terminate` (0x4DCF40), `RwMatrixCreate` (0x7F2A50) y `RwMatrixDestroy` (0x7F2A20), `GetAnimHierarchyFromSkinClump` (0x734A40), `RpHAnimIDGetIndex` (0x7C51A0), `RpHAnimHierarchyGetMatrixArray` (0x7C5120). Los destructores virtuales y `Reset`/`Init`/`CreateInstance` de las primitivas respetan el `ret N` del EXE (regla 18). Se usan reales `RwMatrixMultiply`, `RwV3dTransformPoints`, `RwMatrixUpdate`, la pila de matrices de `g_fxMan` (contador y matrices guardados/restaurados), `FxFrustumInfo_c::IsCollision` (llama al EXE) y `CKeyGen`.
- **CPostEffects**: se sustituye `RwEngineInstance` (0xC97B24) por un dispositivo falso cuyas funciones `RenderStateSet/Get`, `Im2DRenderPrimitive` e `Im2DRenderIndexedPrimitive` trazan sus argumentos (vertices e indices con hash FNV); nada dibuja. Dobles (EXE + simbolo de la DLL cuando la DLL no envuelve al EXE): `RwCameraEndUpdate` (0x7EE180; el simbolo de la DLL **no** es un envoltorio), `RsCameraBeginUpdate` (0x619450), `RwRasterPushContext/PopContext/RenderFast/Create/Destroy/Lock/Unlock` (0x7FB060/0x7FB110/0x7FAF50/0x7FB230/0x7FB020/0x7FB2D0/0x7FAEC0), `rand`, `srand` (0x821B11, sin traza), las cuatro luces de las visiones (0x735F70/0x735F20/0x735E70/0x735EF0) y `FindPlayerSpeed` (0x56E090). Para `Render` ademas se doblan `Fog`, `ColourFilter`, `DarknessFilter`, `Radiosity`, `SpeedFX`, `NightVision`, `Grain`, `InfraredVision`, `HeatHazeFX`, `UnderWaterRipple`, `CCTV`, `FindPlayerPed/Vehicle`, `CPhysical::GetLightingFromCol` y `CCullZones::CamNoRain/PlayerNoRain`, y en esas ejecuciones solo se alterna el hook de `Render`.
- El estado de `CPostEffects` (banderas, contadores, vertices, `ms_imf`, `cc_vertices`, arrays del calor, tiempos, clima, ciclo de tiempo...) son rangos globales reales que se guardan al empezar, se restauran antes y despues de cada ejecucion y se verifican al final (`state_restored`).

## Ruido controlado y mascaras (documentado)

- Hilos ajenos: los dobles solo trazan/actuan en el hilo del banco (`Mine()`); `rand` en otro hilo devuelve 0 y `Malloc`/`Free` reenvian al CRT.
- `FxSystem_c::Constructor`/`Destructor`: el EXE solo escribe `FireAE+0x10` (sys+0x8C) y `FireAE+0x84` (sys+0x100) y el vptr; la DLL construye `m_FireAE` con `CAESound` por defecto (pone a cero el resto) y su vptr es el de la DLL. Se comparan los bytes `[0, 0x7C)` y esos dos dwords en el constructor, y `[0, 0x7C)` en el destructor. Los punteros de `m_Prims` que salen del monton real se enmascaran (se compara su contenido).
- `Grain`/`Initialise`/`HeatHazeFXInit`: el RNG se cuenta y se resume en un hash (65536 llamadas); `srand` del EXE es no-op sin traza (la DLL llama a su `srand` real; las semillas dependen de la hora y no se comparan). Del argumento de fuerza de `Grain` solo se compara el byte bajo (es el unico que llega al canal alfa de `DrawQuad`).
- `Render`: el color `RwRGBA` de `UnderWaterRipple` se compara sin su byte alto (el EXE lo deja sin escribir en la pila).
- Los fixtures no generan NaN de senalizacion y, en operaciones con dos operandos, solo uno puede ser NaN. Tampoco generan entradas que hagan al EXE leer locales sin inicializar ni bucles sin fin (`yoffset` > 0 en `UnderWaterRipple`, altura >= 448 en `CCTV`).

## Fuera del banco (regla 17)

- Rama de guardado de foto de `Render` (0x705301..0x7053E3, `s_WaitForOneFrame` con `CWeapon::ms_bTakePhoto` vigente): E/S real de fichero (`fopen` 0x8232D8 de `Gallery\galleryN.jpg` en Mis Documentos, `JPegCompressScreenToFile` 0x5D0820, `CFileMgr::SetDirMyDocuments` 0x538860 / `SetDir` 0x5387D0). Los fixtures evitan esa rama (`s_WaitForOneFrame` se pone a falso cuando entraria); las ramas sin E/S (borrado por `m_bIsSaveDone`, armado de `s_WaitForOneFrame` y `s_SavePhotoToGallery`) si se comparan.
- Fallo de `RwRasterCreate` en `SetupBackBufferVertex` (0x7044B5..0x7044CF: el EXE llama a `printf` 0x821982 y a `RwRasterDestroy(nullptr)`; la DLL registra con spdlog): ruido NOTSA (regla 26), no se genera.
- `FxSystem_c::Update`, `AddParticle`, `GetBoundingBox` y `UpdateBoundingBoxCB`, y `CPostEffects::HeatHazeFX`, `Radiosity`, `SpeedFX`, `DrawQuadSet*`, `RasterCreatePostEffects`: no pertenecen a la lista pendiente (ya validadas o sin hook en el CSV).

## Correcciones de fuente que cubre el banco

Ver el informe de la revision (cada una con direccion e instruccion). FxSystem_c: `Init`, `Play`, `ForAllParticles`, `SetBoundingSphere`, `ResetBoundingSphere`, `AttachToBone` y los bucles con contador de byte con signo (`Play`, `Stop`, `Init`, `Exit`, `ForAllParticles`). CPostEffects: `Initialise`, `SetupBackBufferVertex`, `ImmediateModeFilterStuffInitialize`, `HeatHazeFXInit`, `NightVision`, `InfraredVision`, `InfraredVisionStoreAndSetLightsForHeatObjects`, `Fog`, `CCTV`, `Grain`, `UnderWaterRipple` y `Render` (varios puntos).

## Supuestos a revisar al compilar

- `offsetof`/disposicion: se comprueba en `begin.layout` (RwGlobals: 0x20/0x24/0x30/0x34 y zBufferNear 0x18; RwCamera: frameBuffer 0x60, nearPlane 0x80). Si `known` sale falso, no se ejecuta ningun caso.
- Los campos crudos del CPed falso (0x530 estado, 0x750 muerte), del vehiculo falso (0x594 tipo, 0x38C banderas, 0x49C pedal, 0x8A4 temperatura, 0x14 matriz, 0x44 velocidad) y de la entidad falsa (0x18 objeto RW) son los que usa el EXE; si la DLL los ubica distinto la diferencia aparecera como fallo de `InfraredVisionStoreAndSetLightsForHeatObjects`/`Render`/`AttachToBone` (seria un fallo real de la disposicion de clases).
- `CEntity::m_pRwObject` y `CMemoryMgr::Malloc` (sobrecarga de dos argumentos) deben ser accesibles/unicos; ajustar el `static_cast` de `Fx::Sites()` si la firma cambia.

## Aprobación (09/10/2026)

Corrida `20261009-161301-487563`: passed=true, 15488 pares, 66 funciones con 0 fallos (31 de FxSystem_c y 35 de CPostEffects), evento complete (state_restored), begin con known=true, layout y 44 sitios correctos, saves y configuraciones intactos. Registrada con `register_fxpost1.py` (evidencia en `runs/`, `validation-summary.json`). Con esto FxSystem_c y CPostEffects quedan COMPLETAS. **Alcance limitado de `CPostEffects::Render`**: la rama de guardado de foto (0x705301..0x7053E3, E/S real de fichero) no está cubierta y los fixtures la evitan; las ramas sin E/S sí se comparan. Límites: dispositivo RW falso, colaboradores doblados (sin crédito), RNG resumido en hash en Grain/Initialise/HeatHazeFXInit, Constructor/Destructor de FxSystem_c con máscaras.
