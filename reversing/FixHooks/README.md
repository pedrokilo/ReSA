<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Hooks duplicados y virtuales con hook plano

Validadas el 03/10/2026 con **6.144 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexDiveFromAttachedEntityAndGetUp` | `ControlSubTask` (0x655710) |
| `CTaskComplexEvasiveDiveAndGetUp` | `ControlSubTask` (0x653810) |
| `CTaskComplexWanderFlee` | `ControlSubTask` (0x65B380) |
| `CTaskComplexCarSlowBeDraggedOutAndStandUp` | `GetTaskType` |
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` | `GetTaskType` |
| `CAEStreamingChannel` | `SynchPlayback` |
| `CAEDataStream` | `QueryInterface`, `AddRef`, `Release`, `Read`, `Write`, `Seek` (IStream), `SetSize`, `CopyTo`, `Commit`, `Revert`, `LockRegion`, `UnlockRegion`, `Stat`, `Clone` |

**Diferencias reales corregidas:**
- Dos hooks del EXE apuntaban a la misma función de la DLL (`dup_hooks.py`):
  - `CTaskComplexDiveFromAttachedEntityAndGetUp::ControlSubTask` (0x655710, copia propia en el EXE) usaba la de `CTaskComplexEvasiveDiveAndGetUp` (0x653810); la base deja de ser `final` y la derivada tiene su propia sobrescritura;
  - `CTaskComplexWanderFlee::ControlSubTask` (0x65B380 = `jmp 0x674C30`) usaba la de `CTaskComplexWander`; ahora tiene su propia sobrescritura que llama a la base sin despacho virtual, como el salto del EXE.
- Virtuales enganchadas con `RH_ScopedInstall` plano (el símbolo de la DLL es un thunk de llamada virtual: con la vtable del EXE entra en bucle); pasan a `RH_ScopedVMTInstall`:
  - `GetTaskType` de `CTaskComplexCarSlowBeDraggedOutAndStandUp` (hueco 4 de 0x86EF80) y `CTaskComplexGoToPointAndStandStillAndAchieveHeading` (hueco 4 de 0x86FE58);
  - `CAEStreamingChannel::SynchPlayback` (hueco 6 de 0x85F3F0);
  - los 14 métodos IStream de `CAEDataStream` (vtable 0x85F0BC, 14 huecos sin destructor virtual; la clase exporta ahora su vtable);
  - `CEntity::Add(const CRect&)` (hueco 1 de 0x863928; solo se comprueba el conmutador).
- Hooks bloqueados (`reversed=false`) también planos sobre virtuales, pasados a vtable sin cambiar su estado: `CTaskComplexCarSlowBeDraggedOutAndStandUp`, `CTaskComplexGoToPointAndStandStillAndAchieveHeading`, `CTaskComplexGangFollower`, `CTaskComplexGotoDoorAndOpen`, `CTaskComplexGoToPointAnyMeans`, `CTaskGangHassleVehicle`, `CTaskGoToVehicleAndLean`, `CTaskSimpleDead::ProcessPed` y `CBike::ProcessAI`.

**Método:**
- Todas las llamadas van por el hueco de la vtable del EXE sobre objetos falsos con la vptr del EXE; el conmutador se comprueba después de fijar el estado de cada ejecución (original: dirección exacta del EXE; candidato: fuera del EXE).
- Parejas que compartían destino: además se ejecuta el estado mixto (este hook igual y la pareja al contrario) y se comprueba que el hueco de la pareja no cambia.
- `CTaskComplexWander::ControlSubTask` se sustituye por una función trazada en la entrada del EXE (0x674C30) y en el cuerpo de la DLL.
- `CAEDataStream` usa `HANDLE` propios sobre `artifacts/input.bin`; `Release` con la última referencia sustituye `operator delete` (0x8214BD) sin reservar memoria y nunca se cierra un `HANDLE` dos veces. Se enmascaran la vptr y el valor del `HANDLE`.
- Comprobaciones sin pares: los 32 hooks bloqueados convertidos son de tipo V, siguen bloqueados y su hueco conserva la dirección original; `CEntity::Add-rect` conmuta su hueco; las parejas tienen destinos distintos en la DLL.
- Fuera de los fixtures: el comportamiento de `CEntity::Add(const CRect&)` (listas de sectores reales y un `assert` de la DLL con rectángulos vacíos).

Repetir: `python reversing/FixHooks/build_probe.py` y `python reversing/run.py --case fixhooks`. Evidencia: [validation-summary.json](validation-summary.json).
