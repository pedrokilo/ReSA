<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Game::Idle y FrontendIdle como el EXE

Validadas el 04/10/2026 con **2.048 pares original/candidato dentro de GTA**, todos exactos: 2 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `Game` (`app_game.cpp`) | `Idle` (0x53E920), `FrontendIdle` (0x53E770) |

**Diferencias reales corregidas:**
- `Idle`:
  - faltaba el limitador de fotogramas del EXE: dos lecturas descartadas de `GetCurrentTimeInCycles`/`GetCyclesPerMillisecond` y espera activa mientras `ciclos / ciclos_ms - 0xB72CA8 < 14` (sin signo); después guarda el nuevo tiempo en 0xB72CA8;
  - un solo `if/else`: `GetScreenFadeStatus` se llamaba dos veces (la segunda tras `RsCameraBeginUpdate`) y ahora una sola, y solo si el menú no está activo;
  - el centrado del ratón es `RsMouseSetPos` en línea con (ancho · 0.5, alto · 0.5) en float (0x858B8C), no `CMenuManager::CentreMousePointer` (mitades enteras, comprobación de 0 y llamada distinta); con SDL3 no se centraba en absoluto;
  - `RenderMenus` y las dos comprobaciones de `CCredits::Render` van en línea (antes llamaban a las funciones de la DLL).
- `FrontendIdle`: se quitó la salida NOTSA de FastLoader (`ShouldLoadSaveGame`) que se saltaba `RwCameraClear`/`RsCameraBeginUpdate` y todo el dibujo.

**04/10/2026: menú ImGui retirado.** Por decisión del usuario (el juego debe comportarse como `gta-sa.exe` 1.0 US), `Idle` y `FrontendIdle` ya no llaman a `UIRenderer::DrawLoop` (menú F7, módulos de depuración, interruptores de hooks), ni `Idle` comprueba si el menú tiene la entrada. La traza del candidato debe coincidir exactamente con la del EXE, sin llamadas extra. Los hooks siguen con `locked = true`, aunque ya no hay menú desde el que conmutarlos. Los 2.048 pares de arriba se obtuvieron con la versión anterior, la que tenía `DrawLoop`: **hay que repetir el banco**.

**Adiciones NOTSA conservadas** (no cambian el orden de llamadas ni el estado observable del EXE):
- En `Idle`, `RsMouseSetPos` no se llama con la aplicación en segundo plano (se lee el dword `ForegroundApp` 0x8D621C sin llamar a nada). El EXE nunca llama a `Idle` en segundo plano (su `WinMain` se detiene); la DLL sí (`NO_FOREGROUND_PAUSE`). Con SDL3 también se centra, como el EXE (la versión anterior lo omitía con `NOTSA_USE_SDL3`, que está activado en esta compilación).
- `ZoneScoped` (Tracy) solo perfila.

**Método:**
- Se llama a la dirección del EXE con los hooks de `Idle`/`FrontendIdle` desbloqueados y desactivados (original) o activados (candidato); el cambio se comprueba después de `State()` y se restauran estado y bloqueo al final.
- Las 44 funciones llamadas (dirección del EXE y cuerpo de la DLL, resuelto antes de escribir ningún parche; se rechazan direcciones repetidas o de thunks) se sustituyen por thunks trazados genéricos, incluidos `CGame::Process` (no hay reentrada en el banco), `Render2dStuff`, `RenderScene`, `RenderEffects`, `DoRWStuffStartOfFrame_Horizon`, `RwCameraBeginUpdate/EndUpdate/Clear`, pads, audio, menú y temporizadores. También `CentreMousePointer`, `RenderMenus`, `CCredits::Render` e `IsForegroundApp`, que no deben aparecer.
- Solo se trazan las llamadas del hilo principal: otros hilos que toquen un stub durante la llamada (el hilo de streaming de audio llama a `IsForegroundApp`) reciben el valor real de `ForegroundApp` o un retorno neutro, sin trazar ni consumir fixtures.
- Registro fijo sin reservas (el json se arma después de la llamada): nombre, `ECX`, argumentos (bool a 8 bits, colores del cielo a 16 bits, `RwV2d` por valor, floats por bits); `CalculateAspectRatio` devuelve el float en ST0; `RwRenderStateSet` se traza cambiando `fpRenderStateSet`.
- Fixtures: ciclos crecientes con paso y ciclos/ms aleatorios (de 0 a ~56 vueltas del limitador, incluida la vuelta de la resta sin signo), estado del fundido, retornos de `BeginUpdate`/`DoRWStuff`, menú, `quit`, créditos, relámpago, colores del cielo, tipo de desenfoque y reducción de pantalla (0, negativa, NaN), FOV, aspecto, resolución, `param` nulo o no.
- `ms_weaponPedsForPC` (0x53E8D0 en el EXE, en línea en la DLL) corre de verdad sobre una lista falsa colocada en 0xC88224 con eslabones propios; se compara por hash junto con la cámara falsa (`fogPlane`) y 17 rangos globales, que se restauran en cada ejecución.
- Fuera del banco: `ForegroundApp` (0x8D621C) = 0, un estado que el EXE nunca alcanza en `Idle`. `ForegroundApp` vale 1 durante la llamada (se restaura justo después).
- Desde el 04/10/2026 no hay stub de `UIRenderer::DrawLoop` ni comprobación del menú ImGui: cualquier llamada que no haga el EXE aparece como diferencia en la traza.

Repetir: `python reversing/GameIdle/build_probe.py` y `python reversing/run.py --case gameidle`. Evidencia: [validation-summary.json](validation-summary.json).
