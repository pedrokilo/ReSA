<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Capa Rs del esqueleto de RenderWare (lista de 27)

Validadas el 04/10/2026 con **6.464 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `Rs` | `RsKeyFromScanCode`, `RsTimer`, `RsWindowSetText`, `RsPathGetSeparator`, `RsCameraBeginUpdate`, `RsAlwaysOnTop`, `RsRegisterImageLoader`, `RsSetDebug`, `RsMouseSetVisibility`, `RsMouseSetPos`, `RsSelectDevice`, `RsInputDeviceAttach`, `rsCommandLine`, `rsPreInitCommandLine`, `RsKeyboardEventHandler`, `RsMouseEventHandler`, `RsPadEventHandler`, `RsPathnameDestroy`, `RsPathnameCreate`, `RsRwTerminate`, `RsTerminate`, `RsInitialize`, `RsGrabScreen`, `RsErrorMessage`, `RsWarningMessage`, `RsEventHandler`, `RsRwInitialize` |

**Diferencias reales corregidas:**
- `RsInitialize`:
  - `maximumHeight` vale 480 (0x1E0), no `DEFAULT_SCREEN_HEIGHT` (448);
  - `appName` apunta a la cadena del EXE (0x86D388) en vez de a una copia de la DLL;
  - devuelve el `RwBool` de `psInitialize` (eax completo).
- `RsRwInitialize`: arena de recursos de 4 MB (0x400000) en `RwEngineInit` y `RwBool` como retorno; los llamadores del EXE prueban eax completo.
- `RsCameraBeginUpdate`: pasa la cámara recibida (antes `nullptr`) y devuelve `RwBool`.
- `RsErrorMessage` / `RsWarningMessage`: el `rsKEYUP` lleva un solo dword con `rsNULL` (0x420); antes un `RsKeyStatus` sin inicializar.
- `RsSetDebug` devuelve 1 en eax (antes `void`); `RsRegisterImageLoader`, `RsSelectDevice` y `RsAlwaysOnTop` devuelven `RwBool` (eax completo); `RsAlwaysOnTop` y `RsMouseSetVisibility` pasan el dword del argumento tal cual.
- `rsCommandLine` y `rsPreInitCommandLine` reciben el argumento en eax: envoltorios `naked` instalados con su nombre (`rsPreInitCommandLine` conserva edx como el EXE).

**Método:**
- Las funciones Ps, `AppEventHandler` (0x53EC10) y el motor RW (`RwEngineInit/Open/Start/Stop/Close/Term`, `RwTextureSetMipmapping/SetAutoMipmapping`, `psGetMemoryFunctions`) se sustituyen con `EntryPatch` solo durante la llamada, en el EXE y en el cuerpo de la DLL; los cuerpos se resuelven antes de escribir ningún parche y nunca coinciden con un sustituto ni con una función Rs probada.
- Sustitutos sin reservas de memoria: traza en un array fijo, convertida a json después de la llamada.
- Los manejadores de teclado, ratón y mando de `RsGlobal` apuntan a un manejador falso con traza; ni ventana, ni dispositivo, ni cámara, ni entrada, ni el motor RW reales se tocan.
- Se guardan y restauran en cada ejecución y al final `RsGlobal` (0x3C bytes) y `DefaultVM`; se compara `RsGlobal` completo, `DefaultVM`, el retorno, la traza y, en `rsPreInitCommandLine`, edx.
- Sin NOTSA ni `FIX_BUGS` en estas funciones.

Repetir: `python reversing/Rs27/build_probe.py` y `python reversing/run.py --case rs27`. Evidencia: [validation-summary.json](validation-summary.json).
