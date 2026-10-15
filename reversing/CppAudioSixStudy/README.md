<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Seis archivos de audio sin ensamblador (12/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `AEAudioEnvironment.cpp`, `AESmoothFadeThread.cpp`, `AEGlobalWeaponAudioEntity.cpp`, `AEVorbisDecoder.cpp`, `AEWaveDecoder.cpp` y `AEAmbienceTrackManager.cpp` (12 apariciones, 10 funciones fuente). Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CAEAudioEnvironment::GetPositionRelativeToCamera_Vector` (0x4D80B0) | float puro con el mismo orden de operaciones que el x87 a 24 bits (`sqrt(dx²+dy²+dz²) - mitad`; `((dy·m1 + dz·m2) + dx·m0)` negado, etc.) |
| `CAESmoothFadeThread::Service` (0x4EED10) | `DsVolumeToDb` = `IntTimesFloatCpp(dsVolume, 0.01f)`; el fundido = `SmoothFadeVolumeCpp` (`pow` del EXE 0x822130 con `ExeCompatiblePowCpp` y `log10`/fyl2x) |
| `CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire` (0x4DF210) | `TrigCompoundCosMulAdd` / `TrigCompoundSinMulAdd` |
| `CAEVorbisDecoder::SetCursor` y `SecondsToMs` | `UnsignedTimesFloatCpp(pos, 0.001f)` y `SecondsToMsCpp` |
| `CAEWaveDecoder::SetCursor` y `BytesToMs` | `WaveCursorBytesCpp` y `BytesToMsCpp` |
| `CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume` (0x4D6E60) | `AudioLog10Times20Cpp(tormenta) - 6.f` y `sqrt(d·d + dz·dz) · 0.5714285969734192f` en float |

Ayudantes nuevos en `X87Math`: `UnsignedTimesFloatCpp`, `SecondsToMsCpp`, `BytesToMsCpp`, `WaveCursorBytesCpp`, `SmoothFadeVolumeCpp` (y `UnsignedAsX87`, privado). Se recuperaron del stash `lote-28-archivos-sin-terminar` (solo los de estos seis archivos).

## Lo que reveló la validación

- **Core9, primera corrida: 206/1024 diferencias en `CAEWaveDecoder::SetCursor`.** Con `/fp:fast` el compilador reasociaba `(pos·0.001f)·tasa` en `(pos·tasa)·0.001f`; el desplazamiento difería en 32768 (un ulp a 24 bits en ~2^39). Corregido con `#pragma float_control(precise, on, push)` solo en `WaveCursorBytesCpp`; Core9 aprobó con 1024/1024.
- **AnimAudio6 fallaba también en HEAD** (`CAnimBlendClumpData::Destructor` 85/128 y `SetNumberOfBones` 174/256, evidencia en `CppAudioSixBaseline`): `CMemoryMgr::FreeAlign` es naked desde `d26e3a5d` y el banco solo interceptaba 0x72F4F0, no la entrada de la DLL. Se añadió `EntryPatch m5(&CMemoryMgr::FreeAlign, FakeFreeAlign)` en `reversing/AnimAudio6/LiveComparison.hpp` (sin quitar ninguna compuerta) y aprobó con 13568 pares.

## Bancos (todos con cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| Small2 | 32016 | GetPositionRelativeToCamera (1024 vec) |
| UnlockAudio / Audio14 / Audio18 / RadioVolumes355 | 9216 / 10240 / 9216 / 1024 | dependencias de GetPositionRelativeToCamera |
| Sys10 | 17088 | Service (1024) |
| AnimAudio6 | 13568 | ServiceAmbientGunFire (1024) |
| Audio13 | 11520 | Vorbis SetCursor (512), GetStreamLengthMs/PlayTimeMs |
| Core9 | 19968 | Wave SetCursor (1024), GetStreamLengthMs/PlayTimeMs |
| AudioCompletion | 10086 | UpdateAmbienceTrackAndVolume (7846; comprobado con `build.py` y `run_local.py` propios) |
| AmbienceTrack | 4416 | dependencia de la anterior |

Evidencia: `reversing/CppAudioSixUnified/runs/` (incluye las corridas fallidas de Core9 y AnimAudio6) y `reversing/CppAudioSixBaseline/runs/`. Parche completo de la fuente: `audio-six.patch`.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos. Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| AEAmbienceTrackManager.actual-after.obj | 28093fb04708df55ce45f48e0c0ddbb9ca7e829cc9316a91914dea574fa95f72 |
| AEAmbienceTrackManager.actual-before.obj | efe4bd0ae10d0f8a1b08157882c14e57ac12fa774b74143ed538ba35655b3b64 |
| AEAudioEnvironment.actual-after.obj | 04f5405f1b6dd02fb0a56273d0dc1eb3923c394dcb4233d7e3cbef34a776981b |
| AEAudioEnvironment.actual-before.obj | 3598748045b14465dedd183e3da4a232849264d2af45dc5db2778fcdb576be3e |
| AEGlobalWeaponAudioEntity.actual-after.obj | 81f7234e59d298e42a590037e90c9c6ea62ae607d9b4d853363ebc0bde9caf01 |
| AEGlobalWeaponAudioEntity.actual-before.obj | 32ce01cff130931f8d3501f2105da9d9397de8125848ee69b51b91074a155ed1 |
| AESmoothFadeThread.actual-after.obj | cbc6ead3fc0b2a2313881cf80613656063b825cf5a023a580ba0a01f69e55b94 |
| AESmoothFadeThread.actual-before.obj | c2764d059e917b2f65ffe7413f9ab3479974a79f156f58122b287237e45d8340 |
| AEVorbisDecoder.actual-after.obj | ef0a1fa0feed4fdbd020d4d1f8fa082b5ba1b3cfd7f6fbd184c2b66236735a7d |
| AEVorbisDecoder.actual-before.obj | c1167fd3ed8590269d8ba3a41318e7dfcbe05a496ae4da27ed8146506f261b1f |
| AEWaveDecoder.actual-after.obj | c3fc6beace613f9e1ec3de848310c351fd755dc937e7c6faf2052813d8d6ad8d |
| AEWaveDecoder.actual-before.obj | f15899ff5abeefc195bb0b240bc0a55050a86596e43ee6f5be956dc1fd4fec72 |
| X87Math.actual-after.obj | b08cb9c5d29c90478254d01490fb110de457f0abdec5e6f5046f309b3c7ffa59 |
| X87Math.actual-before.obj | 8590d8d99e4da699641567b30973e30ae208e169675609cdb7691e400b583869 |
