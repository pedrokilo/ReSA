<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Flujo de datos y audio de armas (lista de 20)

Validadas el 03/10/2026 con **10.176 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEDataStream` | Constructor, Destructor, `FillBuffer`, `GetCurrentPosition`, `Initialise`, `Seek` (OG) |
| `CAEWeaponAudioEntity` | `Initialise`, `Terminate`, `Reset`, `StopFlameThrowerIdleGasLoop`, `PlayGoggleSound`, `WeaponReload`, `PlayFlameThrowerIdleGasLoop`, `ReportChainsawEvent`, `ReportStealthKill`, `PlayGunSounds`, `PlayFlameThrowerSounds`, `PlayWeaponLoopSound`, `PlayCameraSound`, `PlayMiniGunFireSounds`, `PlayMiniGunStopSound`, `PlayChainsawStopSound`, `UpdateParameters`, `WeaponFire` |

**Diferencias reales corregidas:**
- `CAEWeaponAudioEntity`:
  - todos los sonidos se construyen en `m_tempSound` de la entidad (`Initialise`, `RegisterWithPhysicalEntity`, `m_Event`/`m_ClientVariable` y `RequestNewSound`), como el EXE; la DLL usaba un `CAESound` local, así que `m_tempSound` y su referencia a la entidad física no quedaban como en el original;
  - la vtable propia es 0x862E68 con una entrada (`RH_ScopedVirtualClass` usaba 0x86C2AC, la de `CAEPedWeaponAudioEntity`, con dos);
  - `PlayGunSounds`: el índice de frecuencia de avión es un byte con módulo con signo y la tabla se lee del EXE (0x8CC29C, también el índice −1); una distancia NaN interpola (`test ah,1`); los dos `20 · log10` usan `fldlg2`/`fyl2x` en precisión extendida (antes `log10` de la CRT en double);
  - `UpdateParameters`: el giro de la minigun se para con cualquier estado distinto de girando/disparando; la cola de la minigun se para también con volumen NaN; `m_ClientVariable` del cuchillo se convierte con `_ftol2` (64 bits, parte baja) y la suma es de 32 bits;
  - `PlayMiniGunFireSounds` llama a `PlayGunSounds` antes de marcar el disparo activo; `StopFlameThrowerIdleGasLoop` para el sonido antes de anular el puntero.
- `CAEDataStream`: las seis funciones ya coincidían con el EXE (sin cambios).
- Constructor (0x5DE990) y Destructor (0x507560) de `CAEWeaponAudioEntity` siguen bloqueados: 0x5DE990 es el constructor de `CAEPedWeaponAudioEntity` y 0x507560 el de `CAEWeaponAudioEntity` (no hay destructor propio); ninguna dirección tiene llamadores en el EXE y los envoltorios de la DLL no hacen lo mismo.

**Método:**
- El hilo de streaming de audio se aparca en `IsForegroundApp` durante todo el banco, como en FixAudio y Audio13.
- `CAEDataStream` usa objetos falsos con `HANDLE` propios sobre `%TEMP%/gta_audio20/stream.bin` (nunca ficheros de audio del juego) y un fichero inexistente; la vptr se compara por tipo y el `HANDLE` se enmascara (se comprueba la posición del fichero y si el destructor lo cierra). `operator delete` (0x8214BD) se sustituye solo durante el destructor.
- `CAEWeaponAudioEntity` usa una entidad de audio falsa (vptr 0x862E68, `m_tempSound` incluido), una entidad física falsa (peatón, coche o avión, con o sin matriz) y sonidos falsos; `UpdateParameters` se llama por el hueco 0 de la vtable 0x862E68.
- Se simulan y registran en el EXE y en la DLL:
  - `RequestNewSound` (copia el sonido pedido y devuelve una ranura falsa o nulo), `CancelSoundsOwnedByAudioEntity`, `AreSoundsOfThisEventPlayingForThisEntity`, `AreSoundsPlayingInBankSlot` y `CancelSoundsInBankSlot`;
  - `IsSoundBankLoaded`, `LoadSoundBank` e `IsLoadingTuneActive`;
  - `ResolveProbability`, `GetRandomNumberInRange<float>`, `GetPositionRelativeToCamera` y `GetDistanceAttenuation`;
  - `CEntity::RegisterReference`/`CleanUpOldReference` (el EXE las llama en línea desde `CAESound::Initialise`).
- `CTimer::m_snTimeInMilliseconds` y `CGame::currArea` se fijan por caso y se restauran; se vigilan las vtables 0x862E68, 0x86C2AC y 0x85F0BC, la tabla 0x8CC298 y las entradas parcheadas.
- Fuera de los fixtures:
  - valores NaN en distancias, volúmenes, velocidades y `m_ClientVariable`;
  - entidades nulas en las funciones que el EXE desreferencia sin comprobar.

Repetir: `python reversing/Audio20/build_probe.py` y `python reversing/run.py --case audio20`. Evidencia: [validation-summary.json](validation-summary.json).
