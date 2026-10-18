<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Miembros de grupos de peatones (lista de 18)

Validadas el 03/10/2026 con **10.112 pares original/candidato dentro de GTA**, todos exactos: 18 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedGroupMembership` | `AddFollower`, `AddMember`, `AppointNewLeader`, `CountMembers`, `CountMembersExcludingLeader`, `From`, `GetLeader`, `GetMember`, `GetObjectForPedToHold`, `IsFollower`, `IsLeader`, `IsMember`, `Process`, `RemoveAllFollowers`, `RemoveMember` (por índice y por peatón), `RemoveNFollowers`, `SetLeader` |

**Diferencias reales corregidas:**
- `RemoveMember` (por índice):
  - el hueco se pone a nulo al final, no al limpiar la referencia (al quitar al líder, `GetLeader()` todavía lo devuelve para la comprobación de `m_pPlayerData`);
  - el blip se borra con la referencia SCM del pool (`índice << 8 | estado`), no con el índice.
- `AddFollower`:
  - mira `m_pPlayerData` del líder, no su tipo;
  - el hueco libre se busca solo entre los seguidores (0..6): con los 7 llenos no ocupa el del líder.
- `Process`:
  - quita por distancia solo a los seguidores (0..6), si `r² < d²` (NaN se queda) **y** si están en la misma área que el líder;
  - el orden de la suma es z, y, x;
  - el líder jugador muerto se reconoce por el índice.
- `RemoveNFollowers`: contador con signo, se saltan los huecos nulos (antes se caía), recorre 0..6 sin cortar el bucle.
- `RemoveAllFollowers`, `IsFollower`, `SetLeader`, `AppointNewLeader` y `CountMembersExcludingLeader`: el líder se distingue por índice (hueco 7), no por puntero.
- `From`: llama a `AddMember` con los 8 huecos (registra referencias y cambia el decision maker) en lugar de copiar la memoria.
- `GetObjectForPedToHold`: `(rand() & 0xFFFF) / 32768 * 100` truncado, menos de 33 cigarro, menos de 66 bebida; los índices de modelo se leen sin signo.
- `AddMember` no comprueba que el hueco esté vacío y `GetMember` no comprueba el índice.

**Método:**
- 10 peatones falsos dentro de un pool de peatones falso; el puntero global 0xB74490 se cambia solo durante la llamada.
- Inteligencias, datos de jugador, grupo y pertenencia en buffers propios; las referencias de los huecos se registran en orden.
- Se simulan y trazan `CRadar::ClearBlipForEntity`, `CPed::GiveObjectToPedToHold` y el RNG; `MI_GANG_DRINK`/`MI_GANG_SMOKE` se cambian por caso y se restauran.
- Se comparan la pertenencia, los bytes de los peatones (sin `m_pReferences`), los decision makers, las listas de referencias, el retorno y la traza.
- Fuera de los fixtures (el EXE se cae):
  - peatón nulo en `AddFollower`, `SetLeader`, `AddMember` y `RemoveMember` por peatón;
  - hueco nulo en `RemoveMember` por índice;
  - origen con huecos nulos en `From`.

Repetir: `python reversing/Group18/build_probe.py` y `python reversing/run.py --case group18`. Evidencia: [validation-summary.json](validation-summary.json).
