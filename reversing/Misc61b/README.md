<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco 61b: CBmx, CQuadBike, CCarFXRenderer y CStencilShadows

Fecha: 05/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `build_probe.py` y `run_local.py`).

Compara dentro del motor el EXE original (hooks de las cuatro clases desactivados) contra el candidato (hooks activos) con
fixtures deterministas (semilla por caso) sobre objetos falsos: bici (0x838 bytes) y quad (0x9BC) con offsets crudos del EXE,
vtables falsas con trampas, conductor/jugador falsos, asociaciones de animación falsas, marcos RW, handling y modelo de
colisión falsos. Las llamadas a bases de la DLL/EXE (CAutomobile, CBike, CVehicle, CPhysical), animaciones, estadísticas,
RenderWare (RpClumpForAllAtomics, texturas, rasters, Im3D, estados de render), memoria y DrawRect se sustituyen por dobles
trazados que parchean tanto la dirección del EXE como el símbolo o cuerpo de la DLL. Antes de cada caso se escribe y vacía un
evento `case_start`; cada `Write` llama a `PumpMessages`. Pads, ratón, reloj, texturas de suciedad, buffers de sombras y
listas de objetos de sombra se guardan y se restauran; el evento final lleva `state_restored`.

## Funciones y casos (42 filas)

* CQuadBike: Fix, GetRideAnimData, PreRender (512), ProcessAI (1024), ProcessControl (1024), ProcessControlInputs (1024),
  ProcessDrivingAnims, ProcessSuspension, ResetSuspension, SetupDamageAfterLoad, SetupSuspensionLines (256 las restantes).
* CBmx: BurstTyre, FindWheelWidth, SetUpWheelColModel (256), LaunchBunnyHopCB (512), ProcessBunnyHop (1024), ProcessControl (512),
  PreRender (1024), ProcessAI (2048), ProcessDrivingAnims (2048).
* CCarFXRenderer: las 12 funciones, 256 casos cada una.
* CStencilShadows: GraphicsHighQuality, Init, Shutdown, RenderBuffer, SunSetPositionFromEntity, RenderStencilShadows (256),
  UpdateHierarchy, Process, Render, sub_710CC0 (512), RenderForObject y RenderForVehicle (1024).

Las funciones virtuales se comprueban también por el cambio de la ranura de la vtable del EXE (ProcessControl 10, PreRender 17,
etc.); las demás con `JumpsToDll`.

## Fuera del banco

* Constructores de CBmx y CQuadBike: exigen construir un vehículo completo (modelo, handling, colisión, ruedas); se verificaron
  solo por lectura contra el EXE.
* CStencilShadows::RegisterStencilShadows: recorre el pool de vehículos y los sectores del mundo reales, y crea objetos de sombra
  con malloc y referencias; no se puede aislar con seguridad en una partida viva. Se reconstruyó y se verificó solo por lectura.
* Dibujo real con GPU (Im3D, DrawRect, estados de render): siempre doble trazado, nunca se dibuja.

## Uso (no ejecutado en esta tanda)

`python reversing/Misc61b/build_probe.py` y después `python reversing/Misc61b/run_local.py`. Filtro opcional: fichero
`misc61b-only.txt` con `Clase::Función`.
