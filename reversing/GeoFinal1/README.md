<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GeoFinal1: banco por pares de 8 funciones de 4 clases

Banco simple (flujo `Small2`/`PadMouse1`, dentro del juego completo, enganchado en `CGame::Process`). Sin compilar ni ejecutar todavía.

Uso: `bash reversing/_handoff/bench.sh GeoFinal1 3`. Filtro opcional: `geofinal1-only.txt` en la carpeta del juego con `Clase::funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (como PadMouse1).

## Corpus: 4608 pares

| Función | EXE | Pares |
|---|---|---|
| `CCollision::SphereVsEntity` | 0x41A5A0 | 1024 |
| `CPlayerPed::SetupPlayerPed` | 0x60D790 | 128 |
| `CPedGeometryAnalyser::CanPedTargetPed` | 0x5F1C40 | 1024 |
| `CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped` | 0x5F3B70 | 512 |
| `CRealTimeShadowManager::DoShadowThisFrame` | 0x706BA0 | 512 |
| `CRealTimeShadowManager::Exit` | 0x706A60 | 256 |
| `CRealTimeShadowManager::Init` | 0x7067C0 | 128 |
| `CRealTimeShadowManager::Update` | 0x706AB0 | 1024 |

Con esto `CRealTimeShadowManager` queda completa (GetRealTimeShadow, ReInit y ReturnRealTimeShadow ya estaban validadas) y también `CPedGeometryAnalyser` (las demás ya figuraban validadas en `docs/hooks.csv`).

## Qué se compara (original = hook desactivado, candidato = hook activo)

Todo sobre objetos falsos con guardas de 16 bytes a cada lado (se comparan enteros). Se compara el valor devuelto (solo AL), la traza ordenada de colaboradores con sus argumentos crudos, el estado x87 (palabra de control; precisión de 24 bits durante la llamada) y todos los bytes de los fixtures. Se comprueba además que el salto del hook (`JumpsToDll`) coincide con el estado, siempre después de `State(candidato)`.

Los únicos globales reales tocados son `CWorld::Players` (0x320 bytes), `PlayerInFocus` y los seis arrays de sombras del `CTimeCycle` (+ índice): se guardan al empezar, se restauran tras cada ejecución y se vuelven a comprobar al final (`state_restored`).

### SphereVsEntity
El EXE no lee su parámetro `sphere`: prueba una `CColSphere` local sin inicializar, es decir 16 bytes de pila en `[S-0x68, S-0x59]` (S = ESP al entrar). El banco llama con un puente `naked` (`CallSve`) que escribe una semilla de 16 bytes justo ahí (con la esfera "real" de decoy llena de NaN, que debe ignorarse). La fuente ahora captura esos mismos 16 bytes en un puente `naked` antes de cualquier push y salta al cuerpo C++. Fixtures: entidad (usa colisión, `IsExtraEntityToIgnore`), matriz presente o creada por `AllocateMatrix`+`UpdateMatrix`, modelo con y sin datos, 0..3 esferas/cajas, 0..4 triángulos con y sin grupos de caras, contadores negativos (0xFFFF, 0x8000, 0xFFFFFFFF), semilla finita/NaN/infinita. `TestSphereBox`, `TestSphereTriangle`, `CalculateTrianglePlanes` están simulados y devuelven resultados guionizados (la traza incluye los 16 bytes de la esfera, la caja/triángulo/plano exactos y el orden de las llamadas).

### SetupPlayerPed
Se simulan `CPed::operator new` (0x5E4720), el constructor de `CPlayerPed` (0x60D5B0; el de la DLL se localiza como la primera llamada tras el `operator new` dentro de la función, copiado de Misc8), `CPlaceable::SetOrientation` y `CWorld::Add`. Ped falso de 0x7A4 bytes; ids 0 y 1 con `PlayerInFocus` 0/1.

### CanPedTargetPed y ComputeClosestSurfacePoint-ped
Peds falsos con y sin matriz, inteligencia falsa (la tarea secundaria DUCK se lee de verdad). `CanPedTargetPoint`, `ComputeEntityBoundingBoxCornersUncached` y `ComputeClosestSurfacePoint` (rectángulo) se simulan y trazan (vector, bool de dirección, puntero a las esquinas, `point` y la posición del ped).

### CRealTimeShadowManager
Gestor falso (0x54), 16 sombras falsas (0x4C), 16 propietarios falsos (0x140) y una arena para `Init`. `operator new/delete` del EXE (0x82119A/0x8214BD) solo se sustituyen durante `Init` y `Exit`. Simulados: `CRealTimeShadow::{Create, Destroy, Update}`, `CShadowCamera::{Create, Destroy, MakeGradientRaster}`, `ReInit`, `GetRealTimeShadow`, `CShadows::StoreRealTimeShadow`, `Fx_c::GetFxQuality` (cuenta las llamadas). Intensidades de prueba con bit 7 (128, 129, 200, 253, 255) para ejercitar el `movsx`.

## Ruido controlado
- Hilos ajenos: el registro de trazas solo acepta el hilo del banco (`owner`/`Mine()`); los stubs en otros hilos no registran nada.
- Floats: sin NaN de señalización; magnitudes ≤ 3e14 (el x87 conserva el exponente extendido en sumas de cuadrados, SSE no).
- El valor devuelto se enmascara a AL; los bytes altos de EAX son basura de pila en el EXE.
- Solo se prueban `playerId` 0 y 1 (con -1 el EXE indexa `Players[-1]`, memoria ajena).

## Funciones corregidas en la fuente al preparar el banco
Ver el informe del lote: `SphereVsEntity` (implementada, antes `NOTSA_UNREACHABLE`), `SetupPlayerPed` (indexa `Players[playerId]` sin pasar por `FindPlayerInfo`), `CanPedTargetPed` (z = (z+0.75f)-1.0f), `CRealTimeShadowManager::Exit` (borra cada sombra no nula, sin mirar propietarios), `Update` (intensidad con signo) y `DoShadowThisFrame` (dos llamadas a `GetFxQuality`).
