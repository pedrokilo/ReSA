<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEAmbienceTrackManager (resto)

Validadas el 02/10/2026 con **4.416 pares original/candidato dentro de GTA**, todos exactos: `Reset`, `IsAmbienceTrackActive`, `IsAmbienceRadioActive`, `PlaySpecialMissionAmbienceTrack`, `StopSpecialMissionAmbienceTrack` y `Service`. Con `UpdateAmbienceTrackAndVolume` (validada antes en [AudioCompletion](../AudioCompletion/README.md)), la clase queda 7/7.

**Diferencia real corregida:** `CAEAudioUtility::GetRandomNumberInRange<int>` (0x4D9C10) es `min + trunc(rand/32768 · (max − min + 1))`. La plantilla de la DLL interpolaba en `float` y podía dar otro desplazamiento de arranque de pista. Afecta a todo el audio que la usa.

**Método:** gestor propio. `AudioEngine`/`AEAudioHardware` (`PlayTrack`, `SetChannelVolume`, `StopTrack`, `StopSound`), `UpdateAmbienceTrackAndVolume`, `StartTrackPlayback`, `CheckForPause` y el RNG base están controlados. El hook de `CAEAudioUtility::GetRandomNumberInRange<int>` se desactiva en la fase original para comparar la fórmula del EXE. Pausas, menú y tiempo se fijan por caso.

Repetir: `python reversing/AmbienceTrack/build_probe.py` y `python reversing/run.py --case ambience`. Evidencia: [validation-summary.json](validation-summary.json).
