<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuesta a la colisión de un peatón con el jugador (grupo G)

Validadas el 07/10/2026 con **2.048 pares original/candidato dentro de GTA**, todos exactos: 1 función de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputePedCollisionWithPlayerResponse` |

**Diferencias reales corregidas:**
- `ComputePedCollisionWithPlayerResponse` estaba sin reconstruir (el hook llamaba al EXE con `{ .reversed = false }`). Reconstruida desde 0x4BE7D0 sin `FIX_BUGS`:
  - sin peatón chocado no se escribe nada; "delante" es `dot >= 0` y "detrás" es `dot <= 0` con el producto escalar `(z + y) + x` en float (NaN falsos);
  - sale sin escribir nada si hay una tarea de grupo (`0xF3`) sin subtarea y el grupo del jugador tiene 2 miembros o menos;
  - `CPed::Say(CTX_GLOBAL_BUMP)` siempre se llama antes de elegir respuesta; la tarea de decir (`CTaskSimpleSay`) se guarda antes de la mirada (`LookAt`);
  - con la tarea activa de seguir al líder en formación (`0x391`): tiradas de azar (`< 0.25`, luego `< 0.33` mira o `< 0.66` dice), y la rama de grupo si el otro peatón es el líder;
  - en el resto: paso evasivo perpendicular (`CTaskComplexEvasiveStep`, dirección `(-fy, fx, 0)` normalizada), o ruta alrededor del otro peatón (`CTaskComplexFollowPointRoute`) cuando el peatón se mueve (estados 4, 6, 7), y ajuste del seguidor de pandilla (radio `+0.1`, bit de salir, copia de la dirección si la distancia es `< 8`).

**Método:**
- Original (hook desactivado) frente a candidato con fixtures idénticos; la función se llama en la dirección del EXE con `this` = manejador propio de 0x34 bytes con centinelas en las tareas de respuesta, de decir y parcial.
- Tres peatones falsos (vtable de trampas, matriz propia o posición en la colocación, inteligencia), nueve tareas falsas de 0x80 bytes (vtable propia: destructor, `GetSubTask`, `IsSimple`, `GetTaskType`, `MakeAbortable`) y un evento escrito a mano.
- Las tareas nuevas del pool de tareas y las rutas nuevas del pool de `CPointRoute` se siguen por hueco (`m_LastFreeSlot` restaurado y huecos libres con 0xCD): se compara el tipo, los bytes y las referencias registradas, y se borran después comprobando que ambos pools quedan como al principio.
- Se simulan y trazan `CPed::Say`, `IKChainManager_c::LookAt`, `ComputeRouteRoundEntityBoundingBox` (rellena toda la ruta), `CPedGroupMembership::CountMembers` y el generador aleatorio (rand 0x821B1E y `CGeneral::GetRandomNumber`).
- Se guardan y restauran el reloj y las referencias de las entidades.

Repetir: `python reversing/EvHG/build_probe.py` y `python reversing/run.py --case evhg`. Evidencia: [validation-summary.json](validation-summary.json).
