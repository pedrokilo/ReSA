<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Mundo, render y plugins (lista de 52)

Validadas el 02/10/2026 con **18.384 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 14 clases con `validated=1`. Los 3 `WindowedModeHooks` quedan fuera: son reemplazos NOTSA dentro de RenderWare D3D9 (modo ventana) sin equivalente fiel en el original.

| Clase | Funciones |
|---|---|
| `CAnimatedBuilding`, `CHandObject` | `PreRender`, `ProcessCollision`/`Render`, `ProcessControl` |
| `CBouncingPanel` | `ProcessPanel`, `ResetPanel`, `SetPanel` |
| `CCreepingFire` | `SetReadyToBurn`, `TryToStartFireAtCoors`, `Update` |
| `CHandShaker` | `Process`, `Reset`, `SetDefaults` |
| `CMouseControllerState` | `CheckForInput`, `Clear`, Constructor |
| `COccluder` | `NearCamera`, `ProcessLineSegment`, `ProcessOneOccluder` |
| `CPlaneTrail` | `Init`, `RegisterPoint`, `Render` |
| `CPlantLocTri` | `Add`, `GetPlantDensity`, `Release` |
| `CStencilShadowObject` | Constructor, `Destroy`, Destructor |
| `CWeaponModelInfo` | `GetModelType`, `Init`, `SetClump` |
| `NodeNamePlugin`, `PipelinePlugin` | `Get`/`Set` y `PluginAttach` |
| `WaterCreature_c` | `Exit`, `Init`, `Update` |

**Diferencias reales corregidas:**
- `CMatrix::SetRotate` (fuera de la lista, usada en todo el juego): transcripción exacta del x87 del EXE, con `fsin`/`fcos` a precisión completa sin redondear antes de los productos.
- `CPlantLocTri::GetPlantDensity`: el EXE pasa el triángulo en EAX y devuelve en ST0. El hook era una función cdecl normal; ahora usa una envoltura `naked`.
- `CPlantLocTri::Add`:
  - centro `(p2+p1+p3)·0,33333334`;
  - orden de sumas del radio;
  - densidades truncadas con `_ftol2`.
- `CCreepingFire::TryToStartFireAtCoors`: la comprobación de la celda estaba invertida; el índice usa `_ftol2 & 31` para coordenadas negativas.
- `COccluder::ProcessOneOccluder`:
  - rotación `(RotY·RotX)·RotZ` con bytes sin signo;
  - órdenes de suma del EXE;
  - los desplazamientos de las caras usan el centro y la dirección sin normalizar;
  - la normal del plano único se normaliza.
- `COccluder::ProcessLineSegment` y `NearCamera`:
  - recorte por el plano cercano como el EXE (se deshace un cambio no marcado de la DLL);
  - órdenes de suma.
- `CHandShaker`:
  - orden de los `rand`;
  - escala `|ang/lim|·(max-min)+min`;
  - el factor lento solo con signos estrictamente iguales;
  - paso de tiempo con la constante 0x85A6EC (el float siguiente a 0,03, no `0.03f`);
  - límites superior y luego inferior.
- `CBouncingPanel::ProcessPanel`: órdenes de suma de los productos matriz-vector y amortiguación con el `_CIpow` del EXE.
- `CPlaneTrail::Render`:
  - vértices compactados en `TempBufferVertices`;
  - líneas solo con más de un vértice;
  - atenuación ×0,0001.
- `NodeNamePlugin`: `strncpy` sin el relleno 0xFE de `strncpy_s`.
- `PipelinePlugin::PipelinePluginAttach`: callback de escritura nulo como el EXE y cualquier resultado negativo es error.
- `WaterCreature_c`:
  - π exacto del EXE (el `PI` de `common.h` es impreciso);
  - signo del desplazamiento con padre;
  - tiradas de porcentaje y rangos enteros como el EXE;
  - `fsin`/`fcos`/`fpatan` x87;
  - distancia de dibujo estricta.

**Diferencia no reproducible (enmascarada):** `CHandObject::ProcessControl` copia una matriz temporal cuyo relleno es memoria de pila sin inicializar; se ignoran los cuatro dwords de relleno.

**Método:**
- Objetos falsos.
- Estados de render, Im3D, `CalcScreenCoors`, consultas del mundo, `StartFire`, creación de objetos y el registro de plugins de RW simulados y trazados (los callbacks se ejecutan sobre frames y atomics falsos).
- Todo el estado global tocado se restaura por ejecución.

Repetir: `python reversing/World52/build_probe.py` y `python reversing/run.py --case world52`. Evidencia: [validation-summary.json](validation-summary.json).
