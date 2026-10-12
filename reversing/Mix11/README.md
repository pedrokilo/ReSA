<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helicóptero, ir a la puerta del coche y nadar (lista de 11)

Validadas el 02/10/2026 con **19.520 pares original/candidato dentro de GTA**, todos exactos: 33 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CHeli` | `AddHeliSearchLight`, `BurstTyre`, `Fix`, `InitHelis`, `Pre_SearchLightCone`, `Post_SearchLightCone`, `Render`, `RenderAllHeliSearchLights`, `SetUpWheelColModel`, `SwitchPoliceHelis`, `TestSniperCollision` |
| `CTaskComplexGoToCarDoorAndStandStill` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `CreateSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `ControlSubTask`, `IsVehicleInRange`, `ComputeRouteToDoor` |
| `CTaskSimpleSwim` | Constructor, `MakeAbortable`, `ProcessPed`, `ApplyRollAndPitch`, `ProcessSwimAnims`, `ProcessSwimmingResistance`, `ProcessEffects`, `ProcessControlAI`, `ProcessControlInput`, `CreateFxSystem`, `DestroyFxSystem` |

**Diferencias reales corregidas:**
- `CHeli`:
  - `InitHelis` solo inicializa los dos primeros focos (el bucle del EXE acaba en 0xC1CA30);
  - `RenderAllHeliSearchLights` dibuja solo los `NumberOfSearchLights` focos añadidos (la DLL dibujaba siempre los 4);
  - `TestSniperCollision` devuelve `bool` (al), usa `(z·z + x·x) + y·y`, sigue solo con `z < mitad` estricto y transforma el punto del rotor con el orden de sumas de 0x59C890;
  - `Fix` aplica `SetupDamageAfterLoad` en línea (sin pasar por la vtable); `AddHeliSearchLight` indexa sin comprobar límite y guarda las dos banderas como bytes crudos (la DLL las normalizaba a `bool`).
- `CTaskComplexGoToCarDoorAndStandStill`:
  - `IsVehicleInRange`: vehículo − peatón, `(z·z + x·x) + y·y < r·r` estricto; hook con envoltorio que conserva ecx/edx (los llamadores del EXE llaman a `CreateSubTask` justo después sin recargar ecx);
  - `CreateFirstSubTask` transcrito del EXE: arranca el temporizador de búsqueda como el EXE (la DLL nunca lo arrancaba; el EXE no lo hace con asiento fijo de conductor ni con la puerta de pasajero de espera) y el pasajero que bloquea es `bDontDragMeOutCar` (bit 29 de 0x46C), no `bThisPedIsATargetPriority`;
  - `ControlSubTask` (`GO_TO_POINT`): calcula antes la posición de la puerta, con asiento fijo usa la puerta actual (no la del asiento), copia el punto aunque no encuentre puerta, `bPushOtherPeds` al final; `FOLLOW_POINT_ROUTE` compara `Magnitude() > 0,01` (no el cuadrado `>=`);
  - `CreateNextSubTask` (`PAUSE`): `sqrt(x·x + y·y) < 0,5` estricto; las banderas de puerta alcanzada solo se escriben a cierto; los tipos desconocidos devuelven `nullptr` (no aborta) en las tres funciones;
  - `MakeAbortable` pide dos veces el tipo del evento; `ComputeRouteToDoor` calcula los lados antes que los planos (0x5F3660), proyecta con `(n.z·p.z + n.y·p.y) + n.x·p.x` y la ruta nueva solo escribe el contador; el temporizador ya no pone a cero su relleno; el destructor no llama a `operator delete` con la ruta nula.
- `CTaskSimpleSwim`:
  - `ProcessControlInput`: dirección `(−sin, cos, 0)` (no `(0, −sin, cos)`), producto `(y·ry + x·rx) + rz·0`, bajo −π suma 2π, el giro del torso usa el giro (no el eje X), `paso·0,02` / `paso·0,001` exactos, fpatan/fsin/fcos/asin/pow del EXE con ST0 completo y constantes π/80° exactas;
  - `ProcessSwimmingResistance`: pow del EXE con ST0 completo, z del nivel de agua sin la velocidad, `cos·forward·sy` en el orden del EXE, `paso·0,002` y `paso·0,02`, π/4 y 4/π exactos, un estado desconocido sigue con velocidad cero;
  - `ProcessSwimAnims`: en `CLIMB_JUMP` vuelve a flotar sin animación o con el agua 0,5 por encima (la DLL salía sin estadísticas) y sin nivel de agua no cambia el estado; ángulo de buceo de la global 0x8D2FC4 por 0x8595EC;
  - `ProcessControlAI`: si no es el líder del grupo mira la velocidad de la animación, distancia 2D > 5, `bCanClimbOntoBoat` (bit 13 de 0x478) en vez de `bIsDyingStuck`, escala global 0x8D2FF4 y grupo/tarea pedidos dos veces;
  - `ProcessPed`: el jadeo comprueba `m_bFinishedBlending` (no `m_pPed`), límite global 0x8D2FC0 y paso en ms `paso·0,02·1000`;
  - `ApplyRollAndPitch`: `RotateY`/`RotateX` del EXE (0x59B2C0/0x59B1E0, sin hook en la DLL; la versión de la DLL difería en 1 ULP);
  - `ProcessEffects`: oxígeno con signo y `·1/3`, aleatorio de 0x407180 y burbujas de la matriz 3 (BONE_JAW solo con FIX_BUGS).

**Método:**
- Original (hooks del lote desactivados) frente a candidato con fixtures idénticos; los métodos virtuales por la entrada de la vtable del EXE, los constructores en la dirección del EXE sobre un buffer con 0xCD (se compara el tipo en vez del vptr) y las tareas nuevas del pool se comparan sin vptr y se borran después.
- Helicópteros falsos del tamaño de `CHeli` (nunca vehículos reales) con vtable de trampas (`BlowUpCar`), matriz, nodos del rotor, colores y modelo real de vehículo; peatones falsos con clump de tipo 2, matriz, banderas, datos de jugador, armas, IK y tipo; subtarea, evento, mando, asociaciones, jerarquía de huesos (matrices 3 y 8 iguales por FIX_BUGS), sistemas de efectos y rutas (arena propia para `CPointRoute`).
- Se simulan y trazan las dependencias de dibujo, audio, efectos, animación, agua, mando, cámara, estadísticas, escalada, grupos y geometría, y el RNG; `JumpJustDown` (en línea en la DLL) se evalúa de verdad sobre el mando falso.
- Umbrales exactos: radio de búsqueda en el borde (`d·d == r·r`), distancia 0,8 al rotor, 0,01 a la puerta, 0,5 al punto, alturas de agua `z + 2`, `z + 0,5`, `z + 0,65`, giros ±0,01, cambio de estado 0,001/0,025/0,05, `z = −69` y sus vecinos.
- Se guardan y restauran en cada ejecución las globales de `CHeli`, reloj, paso, fotogramas, cámara (ratón, orientación y frente activo), el grupo y bloque de animación de nadar, el `FxPrtMult_c` estático del EXE, el pool de tareas y la lista libre de referencias.
- Fuera de los fixtures: `ProcessControlAI` con `bCanClimbOntoBoat` y `m_pPed` nulo (el EXE lo lee sin comprobar), más de 4 focos en `AddHeliSearchLight` (el EXE escribe fuera del array) y `ApplyRollAndPitch` sin objeto RW (el EXE adjunta una matriz nula).

Repetir: `python reversing/Mix11/build_probe.py` y `python reversing/run.py --case mix11`. Evidencia: [validation-summary.json](validation-summary.json).
