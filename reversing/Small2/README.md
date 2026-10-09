<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Clases pequeñas del lote 4

Validadas el 02/10/2026 con **32.016 pares original/candidato dentro de GTA**, todos exactos: 74 funciones con `validated=1`.

| Clase | Funciones |
|---|---|
| `CMaths` | 3 |
| `CMatrixLink` | 2 |
| `CMatrixLinkList` | 10 |
| `CLodTimeModelInfo` | 2 |
| `CMissionTextOffsets` | 1 |
| `CAEAudioEntity` | 2 |
| `CAEAudioUtility` | 11 |
| `CAEAudioEnvironment` | 6 |
| `CAEExplosionAudioEntity` | 3 |
| `CAEDoorAudioEntity` | 6 |
| `CModelInfo` | 28 |

**Diferencias reales corregidas:**
- `CMaths`:
  - `InitMathsTables` no hacía nada; ahora rellena la tabla 0xBB3E00 con `fild i · 2π/256` y `fsin` del x87, como el original.
  - `GetSinFast`/`GetCosFast` usan el índice `trunc(rad · 40,743664) & 0xFF` (más 64 para el coseno) con la constante del EXE.
- `CMatrixLink`: el constructor ya no inicializa por valor `CMatrix` (`: CMatrix()` ponía a cero toda la matriz); el original solo pone a cero +0x40 y +0x44.
- `CMissionTextOffsets::Load`: el original devuelve en AL el byte bajo de `this` cuando hay entradas, y 0 si no.
- `CAEAudioEntity::Destructor`: restaura la vtable base del EXE (0x859988) y destruye `m_tempSound`.
- `CAEAudioUtility`:
  - `ResolveProbability`: `rand · (1/32767) < p`, cierto directamente si p ≥ 1.
  - `GetRandomNumberInRange<float>`: `rand · (1/32767) · (max - min) + min`.
  - `GetPiecewiseLinear` satura por arriba solo si x es estrictamente mayor.
  - `AudioLog10` con `fldlg2`/`fyl2x`.
  - `GetCurrentTimeInMS` en x87, con la frecuencia del EXE y el truco `fild`/`fchs` para el inicio de 64 bits (la DLL usaba `chrono`).
  - `ConvertFromBytesToMS` y `ConvertFromMSToBytes` con la aritmética sin signo y el `floor` del original.
  - `StaticInitialise`: base `(double)1.2f` (0x86A438), x87, y el log10 escrito siempre en `[1][1]` (error del original).
  - `GetBankAndSoundFromScriptSlotAudioEvent`: tabla de bancos del EXE y `floor((evento - 2000) · 0,005)` en float.
  - `FindVehicleOfPlayer`: usa `CWorld::PlayerInFocus` y siempre pide el peatón.
- `CAEAudioEnvironment`:
  - `GetDirectionalMikeAttenuation` normaliza una copia del vector y lee el corte (0,707) y la atenuación de variables del EXE. La DLL no normalizaba y usaba √½.
  - `GetDistanceAttenuation` usa `floor(d · 10)` sobre la tabla del EXE (la DLL dividía entre 0,1).
  - `GetReverbEnvironmentAndDepth`: una zona -1 cae a la zona interior 14 antes de "sin reverb".
  - `GetPositionRelativeToCamera` lee los desplazamientos de cámara de variables del EXE.
- `CAEExplosionAudioEntity::AddAudioEvent`:
  - distancia `sqrt(z² + y² + x²) · (1/12)`;
  - factor de variación 0x85F0F8, que no es exactamente 2^0,25;
  - capas frontales sin variación de frecuencia;
  - índice `m_Speed` de un byte con signo;
  - tabla de frecuencias del EXE.
- `CModelInfo::ReInit2dEffects` salta las entradas vacías (la DLL llamaba a `Init2dEffects` con `this` nulo).

**Método:**
- Se simulan y se trazan:
  - `rand` y `QueryPerformanceCounter`/`Frequency` (este último parcheado solo durante la llamada);
  - `operator new`/`delete`: arena fija, traza sin reservas;
  - `CFileMgr::Read`, `FindPlayerPed`/`FindPlayerVehicle`;
  - el banco de sonidos y el gestor de sonidos (peticiones con sus campos, consultas y cancelaciones);
  - las primitivas RW de liberación, `CMemoryMgr::Free`, `Init2dEffects`, `SetColModel` y `SetTexDictionary`.
- Los stores de model infos, la tabla de punteros, las zonas de audio, la tabla de senos, el estado temporal de audio, el modo de cámara y el reloj se guardan y se restauran en cada ejecución.
- En `ShutDown` los elementos de cada store usan una vtable falsa y los 2DFX llevan punteros falsos.
- El fichero `small2-only.txt` en la carpeta del juego limita el banco a una función.

Repetir: `python reversing/Small2/build_probe.py` y `python reversing/run.py --case small2`. Evidencia: [validation-summary.json](validation-summary.json).
