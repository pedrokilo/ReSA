# Búsqueda de entidad y sentarse en escritorio (lista de 9)

Validadas el 02/10/2026 con **13.568 pares original/candidato dentro de GTA**, todos exactos: 36 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexSeekEntityStandard` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskComplexSeekEntityRadiusAngleOffset` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskInteriorSitAtDesk` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `FinishAnimCB`, `StartRandomLoopAnim`, `StartRandomOneOffAnim` |

**Diferencias reales corregidas:**
- `CTaskComplexSeekEntity<XYOffset>`: el hook de `CreateSubTask` estaba en 0x496DC0, que es el `GetSeekPos` de `RadiusAngleOffset` (lo llama su `ControlSubTask` en 0x496B9A); la función es 0x665270.
- Plantilla `CTaskComplexSeekEntity` (las tres clases comparten el mismo código del EXE):
  - `MakeAbortable` (0x46ACF0): con prioridad de ocio anula el intervalo de búsqueda (+0x10), no el de escaneo, y el escaneo se rearranca con intervalo 0 (no -1);
  - `CreateFirstSubTask` (0x46F6A0): el escaneo arranca sin mirar el signo y la búsqueda salvo con -1 exacto; solo se alcanza la entidad con distancia estrictamente menor (igual o NaN sigue lejos);
  - `CreateNextSubTask` (0x46EFF0): sin entidad termina; tras `STAND_STILL` vuelve a `CreateFirstSubTask` si el peatón no está enganchado; tipo desconocido devuelve `nullptr`;
  - `CreateSubTask`: tipo desconocido devuelve `nullptr` (antes `NOTSA_UNREACHABLE`).
- `ControlSubTask` (0x46F800):
  - el barco común solo se busca si la entidad es un peatón del mismo grupo (`AreInSameGroup(ped, entidad)`), y solo entonces se desengancha al peatón;
  - si la primera entidad es un vehículo común que no es barco no se mira la segunda;
  - se engancha al barco cuando `MakeAbortable` **sí** aborta (estaba invertido), por la dirección peatón→barco y con la Z de `FindZCoorForPed` solo si encuentra suelo;
  - otras subtareas devuelven la subtarea (no `nullptr`), el escaneo se rearranca sin mirar el signo, NaN en los radios mínimos sigue el camino del EXE y la frase de seguimiento no se comprueba al cambiar de ruta.
- `CTaskInteriorSitAtDesk`:
  - la animación se suelta con `SetDeleteCallback` (0x4CEBC0, tipo borrado), no con la de fin, en el destructor, `MakeAbortable` y al empezar otra;
  - `StartRandomLoopAnim`: tiradas `(int)(rand · 1/32768 · 100)` como el EXE, escribir dura 7000..12000 ms y las otras 2000..5000 ms (estaba al revés), >10 es `IDLE_LOOP` y el resto `BORED_LOOP`; los aleatorios salen antes de mezclar;
  - `ProcessPed`: π exacto (0x858CB8), `CMatrix * CVector` con el orden de 0x59C890, módulo `(z² + y²) + x²`, paso `d < 0.02 ? d : 0.02` y productos `(y + z) + x`.
- `CVector::FromMultiply3x3` (0x59C6D0, la usa el enganche al barco): orden de sumas del EXE.

**Método:**
- Original con los hooks del lote desactivados frente a candidato con fixtures idénticos en búferes propios: 4 peatones (el 3 hace de jugador), 2 vehículos (vtable falsa, el tipo decide si son barcos), tareas falsas con vtable trazada, asociaciones de animación falsas e `InteriorInfo_t` falso.
- Simulados y trazados: `ComputeEntitySeekPos` de los tres calculadores, `AreInSameGroup`, `IsFollower`, `FindPlayerPed`, `Say`, `AttachPedToEntity`, `DettachPedFromEntity`, `FindZCoorForPed` (firmas distintas en EXE y DLL), `CTaskComplexFollowNodeRoute::SetTarget`, `CTaskSimpleStandStill::ProcessPed`, `BlendAnimation`, `RpAnimBlendClumpGetAssociation`, `AreAnimsLoaded` y el RNG.
- Pools de tareas, rutas de puntos y rutas de nodos restaurados en cada ejecución (slots libres con 0xCD); reloj y contador de fotogramas guardados y restaurados.
- Se compara el objeto (sin vptr; las vtables de calculador se traducen a nombres y tras el destructor no se compara la del calculador destruido), los cambios de todos los búferes, las tareas y rutas nuevas, las referencias, el retorno y la traza.
- Fuera de los fixtures: el constructor de `CTaskInteriorSitAtDesk` con un byte de `bDoInstantly` distinto de 0/1 (el DLL lo normaliza a `bool`).

Repetir: `python reversing/CTasks9d/build_probe.py` y `python reversing/run.py --case ctasks9d`. Evidencia: [validation-summary.json](validation-summary.json).
