<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEAmbienceTrackManager (resto)

Validadas el 02/10/2026 con **4.416 pares original/candidato dentro de GTA**, todos exactos: `Reset`, `IsAmbienceTrackActive`, `IsAmbienceRadioActive`, `PlaySpecialMissionAmbienceTrack`, `StopSpecialMissionAmbienceTrack` y `Service`. Con `UpdateAmbienceTrackAndVolume` (validada antes en [AudioCompletion](../AudioCompletion/README.md)), la clase queda 7/7.

**Diferencia real corregida:** `CAEAudioUtility::GetRandomNumberInRange<int>` (0x4D9C10) es `min + trunc(rand/32768 · (max − min + 1))`. La plantilla de la DLL interpolaba en `float` y podía dar otro desplazamiento de arranque de pista. Afecta a todo el audio que la usa.

**Método:** gestor propio. `AudioEngine`/`AEAudioHardware` (`PlayTrack`, `SetChannelVolume`, `StopTrack`, `StopSound`), `UpdateAmbienceTrackAndVolume`, `StartTrackPlayback`, `CheckForPause` y el RNG base están controlados. El hook de `CAEAudioUtility::GetRandomNumberInRange<int>` se desactiva en la fase original para comparar la fórmula del EXE. Pausas, menú y tiempo se fijan por caso.

Repetir: `python reversing/AmbienceTrack/build_probe.py` y `python reversing/run.py --case ambience`. Evidencia: [validation-summary.json](validation-summary.json).
