<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PadMouse1: banco por pares de CPad::UpdateMouse

Banco nuevo y simple (flujo viejo, copiado de `Small2`) para `CPad::UpdateMouse` (EXE 0x53F3C0, GTA SA 1.0 US). Sin compilar ni ejecutar todavía.

Uso: `bash reversing/_handoff/bench.sh PadMouse1`. Filtro opcional: `padmouse1-only.txt` en la carpeta del juego con `CPad::UpdateMouse`.

## Corpus: 9216 pares

6 rutas × 12 estados de `DIMOUSESTATE2` × 8 combinaciones (menú activo, invertir X, invertir Y) × 4 patrones de semilla × 2 (dispositivo previo o no) × 2 relojes.

- Rutas: sin foreground; init que deja el dispositivo nulo; lectura con `E_FAIL`; lectura con 0; HRESULT positivo (1); `E_ACCESSDENIED`.
- Estados: reposo, ±1 en X/Y, rueda arriba/abajo, botones con máscaras 0/128 y otros valores, extremos ±INT_MAX y 16777217. Sin NaN/Inf/INT_MIN (los flotantes salen de enteros).
- El exclusivo de `IsVideoModeExclusive` alterna según (estado + ruta).

## Qué se compara (original = hook desactivado, candidato = hook activo)

CPad falso de 308 bytes con guardas, token de dispositivo opaco, los tres estados del ratón (0xB73404/18/2C, 60 bytes) con sus guardas, dispositivo, `m_bMenuActive`, banderas de inversión, reloj, CPad globales (inmutables), control x87, validez del ABI y la traza de llamadas con argumentos (`foreground`, `exclusive`, `init(byte bajo)`, `read(status, estado, validez)`).

## Intercepción

Se parchean con `EntryPatch` la dirección del EXE y el símbolo de la DLL (thunk incremental y cuerpo):
- `diMouseInit` (0x7469A0, hook bloqueado): un único stub `cdecl(uint32)` en ambos; solo cuenta el byte bajo del argumento (en el EXE el byte alto es basura de pila).
- `GetMouseState` (0x746ED0): `ReadExe` cdecl(buffer) en el EXE y `ReadDll` fastcall(self,_,buffer) en la DLL; misma traza, el segundo comprueba que `self` es el CPad falso. Ambos verifican que el buffer esté en la pila del hilo.
- `IsForegroundApp` y `IsVideoModeExclusive`: mismos stubs en EXE y DLL.
- `CheckForInput` y `SetTouched` se ejecutan de verdad.

Al terminar se comprueba que todas las entradas parcheadas, los globales y el control FP han vuelto a sus bytes iniciales (`state_restored`).

## Ruido del banco corregido (ejecución 20261009-000614-797633)

La primera ejecución dio 579 diferencias en 1752 pares y una violación de acceso. Todo era ruido del banco; no hay diferencia real entre fuente y EXE:

- **Relleno de la estructura (576 pares, todas las rutas 3 y 4 con semillas 0-2)**: `CMouseControllerState` mide 0x14 y su byte 7 es relleno (tras `isMouseSecondXPressed`, antes del float en 8). El EXE (0x53F4F8/0x53F509) copia temp→nuevo→viejo con `rep movs dword` ×5 y por tanto propaga ese byte; el compilador copia miembro a miembro y no. Solo difieren los bytes 27 y 47 del bloque de 60 (relleno de nuevo y viejo). Ningún campo real difiere. Ahora se enmascaran a 0 los bytes 7, 27 y 47 (`MouseBytes()`).
- **Hilos ajenos (3 trazas sueltas + el fallo)**: `AEStreamThread` (y `AEAudioHardware`) llaman a `IsForegroundApp` (0x746060) desde otro hilo; el stub parcheado hacía `trace.push_back` sobre el `json` global concurrentemente (traza espuria y, en el par 1752, corrupción de la memoria del push_back: eip en el constructor de movimiento de nlohmann::json, dentro del stub `Foreground`). Ahora los stubs solo trazan/actúan en el hilo del banco (`owner`); en otros hilos `Foreground` devuelve el valor real de 0x8D621C y el resto no hace nada.
