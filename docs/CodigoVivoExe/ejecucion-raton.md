<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ratón del EXE independiente (gta_sa_standalone.exe)

## Síntoma
En el EXE independiente el ratón no movía la cámara ni el cursor de CJ/menú. El log traía, al arrancar,
`WinInput.cpp:147: FALLÓ (hr=0x80070005, acceso denegado) en PSGLOBAL(diMouse)->Acquire()`.

## Causa real
Esa línea era un falso culpable. `diMouseInit` crea el dispositivo con `DISCL_FOREGROUND | DISCL_NONEXCLUSIVE` antes de que la
ventana sea la activa, y `Acquire` devuelve `E_ACCESSDENIED` (DIERR_OTHERAPPHASPRIO). `CPad::GetMouseState` vuelve a adquirir en el
primer fotograma con foco (`GetDeviceState` -> `DIERR_NOTACQUIRED` -> `Acquire` = 0), así que eso se recupera solo.

El fallo verdadero (medido por memoria con `reversing/FnFase8/probar_raton.py`, SendInput sobre la ventana al frente):
- `ForegroundApp` = 1, `diMouse` válido, `Acquire` = S_OK, `GetDeviceState` = S_OK, **pero siempre lX = lY = lZ = 0 y sin botones**.
  `CMouseControllerState` (Temp/New en 0xB73404/0xB73418) se queda a cero.
- SDL (que se engancha a la ventana nativa en `SDLWrapper::Initialize`) sí recibe `SDL_EVENT_MOUSE_MOTION`.
- DirectInput 8 recibe el ratón por la entrada en bruto (raw input). Windows admite un solo registro por proceso y tipo de
  dispositivo (usage 1/2): el de SDL3 sustituye al de DirectInput (hecho en `Acquire`) y DirectInput deja de recibir datos sin dar error.
- Aislado en `reversing/FnFase8/diprueba/dimouse.cpp` (ventana + DirectInput + SendInput, sin el juego): sin registro de raw input
  suma lX=200 lY=80 (correcto); registrando raw input ANTES del Acquire, correcto; registrando DESPUÉS del Acquire (lo que hace SDL), 0/0;
  `Unacquire`+`Acquire` lo recupera (200/80); destruir y volver a crear el dispositivo NO lo recupera.
- El ASI no lo sufre porque allí el EXE original crea y adquiere el dispositivo con el código del EXE y el orden con SDL no es el mismo
  (diMouseInit está bloqueado en el catálogo); además la prueba del ASI no cambia: el arreglo solo actúa si SDL ve movimiento y DirectInput no.

## Arreglo (C++ puro)
- `source/game_sa/Pad.cpp`: `RecoverMouseFromRawInputSteal` (solo con `NOTSA_USE_SDL3`), llamada desde `CPad::UpdateMouse` tras una lectura
  válida de DirectInput. Si DirectInput no da actividad y SDL cuenta >= 30 eventos de movimiento desde la última actividad de
  DirectInput (como mucho una vez cada 2 s), hace `Unacquire` + `Acquire`, lo que devuelve el registro de raw input a DirectInput.
  Con DirectInput sano (ASI, ratón con actividad) nunca actúa.
- `source/app/platform/SDLWrapper.cpp/.hpp`: contador `MouseMotionCount()` (eventos de movimiento que ve SDL).
- `source/app/platform/win/WinInput.cpp`: `diMouseInit` ya no registra `E_ACCESSDENIED` de `Acquire` como error (es lo normal al arrancar;
  se re-adquiere en el primer fotograma con foco). Se registran los demás fallos.

## Verificación
- Antes (binario independiente actual, mismo guion): tras 3 rondas de SendInput (movimiento, botón izquierdo, rueda), `Temp/New` siempre a cero.
- Después (binario con el arreglo, vía `compilar_over.cmd` + `enlazar.cmd`): `new.mov` = (16, 6) y (8, 3) por fotograma tras el movimiento,
  botón izquierdo = 128 (0x80) en `New`; el log muestra una sola línea de recuperación ("DirectInput no entrega datos pero SDL si") al empezar.
- Comando: `python reversing/FnFase8/probar_raton.py <etiqueta> [exe]` (toma `gta_lock`, lanza, pasa la ventana al frente, SendInput, lee la memoria,
  guarda `D:/Original/sim_out/raton_<etiqueta>.log` y captura, y mata solo su proceso).
- `python contrib/audit_hook_catalog.py --strict`: OK.
