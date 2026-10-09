<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# AudioFinal1 (CAEAudioHardware::Initialise + CAEFrontendAudioEntity 3 funciones)

Banco simple (estilo Small2/PadMouse1), **compilado y ejecutado; aprobado en la corrida 20261009-010133-465573 (2304 pares, 0 diferencias) y registrado en `validation-summary.json` (historico, `dependency_review=null`).** La primera corrida (20261009-005347-353159) fallo con las 341 diferencias de ruido descritas abajo. Reejecucion: `bash reversing/_handoff/bench.sh AudioFinal1 3`.

| Funcion | EXE | Casos |
|---|---|---|
| `CAEAudioHardware::Initialise` | 0x4D9930 | 1536 |
| `CAEFrontendAudioEntity::Initialise` | 0x5B9AB0 | 512 |
| `CAEFrontendAudioEntity::IsLoadingTuneActive` | 0x4DD470 | 128 |
| `CAEFrontendAudioEntity::Reset` | 0x4DD440 | 128 |

## Como se prueba Initialise sin tocar el audio real
- Objeto `CAEAudioHardware` FALSO de 0x1100 bytes (relleno pseudoaleatorio por caso). `AEAudioHardware` no se toca.
- `DirectSoundCreate` (IAT del EXE 0x85801C) y `DirectSoundCreate8` (import de la DLL) se parchean en dsound.dll solo durante
  la llamada y devuelven un dispositivo COM falso (GetCaps, SetCooperativeLevel, GetSpeakerConfig; HRESULT y capacidades
  guionizados, incluido `dwFreeHw3DAllBuffers` de 0 a 0xFFFFFFFF alrededor del umbral 0x18/0x40).
- `operator new` (0x82119A, al que reenvia el de la DLL) se parchea solo durante la llamada y reparte bloques de una arena
  rellena con 0xCD (los demas hilos reenvian al original por un trampolin): los constructores REALES de los cargadores y de
  los canales (EXE frente a DLL) se comparan byte a byte.
- Simulados con traza (EXE y DLL): InitDirectSoundListener, CAEMP3BankLoader/TrackLoader::Initialise,
  CAEStreamingChannel::Initialise, CAEAudioChannel::SetVolume, CAEUserRadioTrackManager::{ScanUserTracks, Initialise},
  CAESmoothFadeThread::{Initialise, Start}, CAEStreamThread::{Initialise, Start}. No se crea ningun hilo.
- El hilo de streaming real se aparca en IsForegroundApp y los falsos ignoran hilos ajenos (`Mine()`).
- Se compara: retorno (AL), objeto completo (hash), bloques de la arena (offset, tamano, hash; el dword 0 de los canales,
  el puntero a vtable, se enmascara), `AESmoothFadeThread.m_nNumAvailableBuffers` (0xB610E8), estado de escaneo (0xB6B988) y la traza.
- Frontend: objeto falso de 0x1EC bytes; simulados LoadSoundBank/LoadSound, GetRandomNumberInRange<int> (valores 0..3 y
  125..300, -1: cubre `movsx` del byte), AreSoundsOfThisEventPlayingForThisEntity y CancelSoundsOfThisEventPlayingForThisEntity.

## Fuera del banco
- Camino `m_nUserTracksScanState == IN_PROGRESS` (bucle `Sleep(100)` hasta que un hilo de escaneo real lo cambie): sin ese
  hilo el bucle no termina. Los demas estados (OFF, COMPLETE, ERROR, otros) si se prueban.

## Correcciones de fuente
- `CAEFrontendAudioEntity::Initialise`: faltaban escrituras (0x80, 0x82=0x1C, 0x7E=-1, 0x84..0x94) y el id de `LoadSound` usa el byte con signo (movsx).
- `CAEAudioHardware::Initialise`: `new CAEMP3BankLoader` ponia a cero el cargador (inicializadores de miembro de CAEBankLoader); el EXE solo escribe 0x14 y 0x66A (0x4DFB10).

## Ruido documentado (ronda 1: 341/1536 pares de CAEAudioHardware::Initialise)
Las 341 diferencias (100 % de los casos con `CAEMP3BankLoader::Initialise` o `CAEMP3TrackLoader::Initialise` simulados a false)
eran solo `arena_used`, `blocks` y `recs`: la DLL ejecuta `NOTSA_LOG_ERR("Failed initializing MP3 module!")` (spdlog, desviacion NOTSA)
y esa llamada reserva 10+ bloques extra (8/32/48/71/106 bytes...) con operator new; el EXE (0x4D99BA `je 0x4D99CB`) sale sin mas.
El objeto (`hw_fnv`), el retorno y los demas campos eran iguales. El banco congela la traza (recs, bloques, arena_used) en el
instante en que falla el cargador (`Freeze()`), de modo que las reservas del log NOTSA no cuentan; no se enmascara nada mas.
