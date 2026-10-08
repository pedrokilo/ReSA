<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sostener objeto y jetpack (lista de 14)

Validadas el 02/10/2026 con **12.352 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleHoldEntity` | Constructores 1, 2 y 3, `Clone`, `MakeAbortable`, `ProcessPed`, `SetPedPosition`, `ReleaseEntity`, `CanThrowEntity`, `PlayAnim`, `FinishAnimHoldEntityCB`, `StartAnim`, `DropEntity`, `ChoosePutDownHeight` |
| `CTaskSimpleJetPack` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `RenderJetPack`, `ProcessThrust`, `ProcessAnims`, `ProcessControlInput`, `ApplyRollAndPitch`, `DoJetPackEffect`, `StopJetPackEffect` |

**Diferencias reales corregidas:**
- `CTaskSimpleHoldEntity`:
  - `DropEntity`: la velocidad del objeto parte de la del peatón (antes se sumaba a la suya), sin soltar con evento se pone a cero, y las tiradas son `rand() · (1/32767) · rango + mínimo` con la matriz del peatón;
  - `ProcessPed`: la rama de soltar la tarea secundaria es solo para `PUTDOWN` (antes cualquier tipo que no fuera `HOLD` ni `PICKUP`); rumbo con `fpatan` restando sin redondear; en `PICKUP`, `operator*(CMatrix, CVector)` (0x59C890), productos escalares `(z + y) + x` y `fpatan` para la mirada;
  - `SetPedPosition`: `Multiply3x3` (0x59C790) y `RotateZ` (0x59B390) transcritos (fcos sin redondear), `m_apBones.data()` y NaN en la altura de dejar actualiza;
  - `ChoosePutDownHeight`: alturas exactas del EXE (0x3EDDDDDE y 0x3EBBBBBC; antes 0.433333f y 0.366667f).
- `CTaskSimpleJetPack`:
  - `ProcessControlInput` reescrita: rumbo con el frente de la cámara (invertido mirando a otro lado, sin restar pi), `GetRadianAngleBetweenPoints(0, 0, -lr, ud)` menos la orientación de la cámara y `LimitRadianAngle` con el objetivo fijado, giro con un solo ajuste de ±2pi, interpolación del ángulo siempre en esa rama con `pow` del EXE, ángulo máximo 0x3FA78D36, `· 1/60`, empuje sin ratón solo con el botón, y la fuerza hacia el objetivo con `max(dist, 0.1)`, límite `|dz| · 0.2 + 3`, masa y paso;
  - `ProcessThrust` reescrita: fuerzas `((dir · 0.008) · mult) · masa · paso`, frenado `(-v / |v|) · fuerza · masa`, límite 0.75 a la z de la suma, altura `1 - (z - (suelo + 100)) · 0.05` con dos llamadas, y balanceo y amortiguación transcritos del x87 (`pow`, `fsin`, productos `(z + y) + x`, pi/4 exacto);
  - `ProcessPed`: la deriva lateral suma o resta 1 (la precedencia la dejaba en ±1), el giro usa `fsin` y `rand() · (1/32767)` del EXE, NaN en la salud controla como jugador y las armas se marcan cambiadas aunque no sea jugador;
  - `RenderJetPack`: comparaciones estrictas con pi del EXE, el giro de las bolas se suma en un lado y se resta en el otro (`ángulo · 180/pi ± delta · 180/pi · 10`), tiempo sin signo con un solo redondeo y `fsin`/`fcos`;
  - `DoJetPackEffect`: crea los dos efectos antes de colocarlos, el tiempo clave vuelve a 0 al crearlos y se acerca a su objetivo sin pasarse;
  - destructor pone a nulo el clump, `ApplyRollAndPitch` usa 180/pi del EXE (0x42652EE0), `ProcessAnims` sin `assert` y el constructor no escribe el tiempo clave (sin `{}`).

**Método:**
- 2 peatones (el 0 con inteligencia, datos de jugador, clump, huesos falsos y matriz; el 1 hace de entidad no objeto y de objetivo fijado), un `CObject` con vtable de trampas (`SetIsStatic` emulado y `TestCollision` trazado), tareas falsas con vtable trazada; la tarea probada de `CTaskSimpleHoldEntity` lleva una vtable falsa para elegir el tipo (`HOLD`, `PICKUP`, `PUTDOWN` u otro).
- Simulados con traza: RNG, animación (`BlendAnimation` x2, bloques, jerarquías, referencias), streaming, `ProcessVerticalLine`, lista de movimiento, modelo de colisión, `CEventGroup::Add` (eventos por campos), pad, RenderWare (jerarquías, matrices de huesos, `RwMatrixRotate/Translate`, `RtQuatRotate`, frames, clumps, skin), FX, audio, armas, `ApplyMoveForce` (con efecto determinista), suelo y dirección de la cámara, `GetWeaponSkill`, `ProcessPlayerWeapon` y `DropJetPack`.
- Globales cambiados y restaurados en cada ejecución: paso y reloj (también por encima de 2^24 y de 2^31), `nNoTempObjects`, ratón, cámaras, tabla de grupos de animación falsa, `ms_aAnimBlocks`, modelo e información de streaming del jetpack. Pool de tareas restaurado (slots libres con 0xCD); constructores sobre buffers con 0xCD comparados enteros; cambios de todos los buffers comparados palabra a palabra.
- Fuera del banco: `m_ThrustFwd = -1` en `DoJetPackEffect` y `RenderJetPack` (con `FIX_BUGS` el DLL solo cuenta 1 como empuje para la llama; el EXE cualquier valor distinto de 0).

Repetir: `python reversing/CTasks14b/build_probe.py` y `python reversing/run.py --case ctasks14b`. Evidencia: [validation-summary.json](validation-summary.json).
