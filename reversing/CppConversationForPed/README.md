<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> Actualización 11/10/2026: este banco se compiló y ejecutó dentro del juego; la corrida 20261011-143612-800417 aprobó 4702 pares (2414 de posición y 2288 de Update) con cero diferencias (auditoría en runs/20261011-143612-800417/approval-audit.json). Las frases siguientes que dicen «no compilado ni ejecutado» describen el estado de cuando se escribió. Resultado y límites en [CppConversationStudy](../CppConversationStudy/README.md).

# CppConversationForPed: propuesta pendiente

**No compilado ni ejecutado.** Banco por pares dentro del juego para las dos funciones hookeadas de `CConversationForPed`, ahora que `source/game_sa/Conversations/ConversationForPed.cpp` se compila entero con `/O2 /Oi /arch:IA32 /fp:fast /Zi`, sin PCH ni unity (ver `source/CMakeLists.txt`): `IsPlayerInPositionForConversation` (0x43AC40, 2414 pares) y `Update` (0x43C190, 2288 pares), 4702 en total. Original = EXE con ambos hooks conmutados a falso; candidato = DLL con ambos hooks activos. No acredita nada hasta compilarse, ejecutarse y auditarse (`audit/audit_run.py`).

Estructura: `../CppConversationPosition` (banco de posición ya derivado) ampliado con los escenarios de `Update` de `../ConversationForPed`; build/audit al estilo de `../CppShadowSqrtStatic`. Namespace `CppConversationForPedLive`, prefijo `cpp-conversation-for-ped`, banco `CppConversationForPed`. Se redirige en memoria `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (se supone aplicado `../CppSqrtSmallStudy/IndependentReview/shadow-bank-clone-manifest.patch`). No se copia ningún `validation-summary.json`, `latest-*.json` ni aprobación previa.

## Qué cubre

Mismos `Spec`, `Fixture::SetUp/Run` (dos fixtures independientes: jugador, peatón, nodos, `gString`, tiempo), valores, orden y nombres de caso del histórico, comparación exacta (`original == candidate`, sin tolerancias) y restauración (nodos, texto, temporizador, palabra de control x87, bytes del jugador real, estado de los dos hooks). Las posiciones globales de los pares son las del histórico: primero 2414 de posición, luego 2288 de `Update`.

| Función | Familia | Pares |
| --- | --- | ---: |
| Posición | `distance` 84, `facing` 10, `speed` 60 | 154 |
| Posición | `damage` | 48 |
| Posición | `availability` | 32 |
| Posición | `placement-early-out` | 2 |
| Posición | `mixed-distance` 1080, `mixed-speed` 1080 | 2160 |
| Posición | `x87-precision` (24/53/64 bits) | 18 |
| Update | `state-timer` (estados, teclas, límites temporales) | 1200 |
| Update | `reply-branches` (ramas, subtítulos, contextos de voz) | 1024 |
| Update | `question-audio` | 32 |
| Update | `inactive-gates` | 32 |
| **Total** | | **4702** |

Proveedores controlados (EntryPatch en la dirección del EXE y en la función de la DLL): `FindPlayerPed`, `FindPlayerCoors`, `CPed::PedIsReadyForConversation`, `CPedGroupMembership::CountMembersExcludingLeader`, `CMessages::ClearSmallMessagesOnly/AddMessageJump/AddMessageQ`, `CText::Get`, `CPed::Say`, `CConversations::AwkwardSay`. Las trazas de esas llamadas (con el estado de la conversación en cada una) se comparan.

## Cambios inevitables respecto al banco histórico

1. El histórico instrumentaba el texto del candidato (`instrument.py`); `probe_build.py` no instrumenta fuentes, así que los proveedores se parchean por dirección en el EXE y en la DLL. Requiere que esas funciones no queden incrustadas en `ConversationForPed.cpp`: están en otras unidades y `build_probe.py` rechaza `GTASA_WITH_LTO` activo en `build/CMakeCache.txt`.
2. **Teclas Sí/No**: `CPad::ConversationYesJustDown/NoJustDown` son inline en `Pad.h` y `/O2` las incrusta, así que no hay función de la DLL que sustituir. Se fija el estado real de `CPad::GetPad(0)` (`DisablePlayerControls=0`, `NewState/OldState.DPadRight/DPadLeft`) idéntico para ambos lados, y se restaura al final. Consecuencia: desaparecen las entradas `yes`/`no` de las trazas de ambos lados; el orden y el cortocircuito de las lecturas de teclas solo se observan por sus efectos (estado, nodo, mensajes), no como llamadas.
3. Los contadores `updateCalls/positionCalls` (0/1) se sustituyen por el campo `switches`: en original ambas entradas (0x43AC40 y 0x43C190) conservan sus bytes originales y no saltan a la DLL; en candidato ambas saltan; medido antes y después de cada ejecución.
4. `inputs_intact` se exige en ambos lados (el histórico solo en el original) e incluye ahora los bytes de `CPad::Pads[0]`; `fp_control` se registra por lado y se compara.
5. El histórico paraba en el primer fallo; aquí se siguen todos los casos y se cuentan fallos por función. Compuertas: tabla = 2414 + 2288 casos, resultados `true` y `false` de posición, y en `Update` casos con y sin avance.
6. El campo `case` es el índice dentro de cada función (el nombre conserva el índice global del histórico).
7. Se usa la configuración por defecto de `run_case` (como `CppShadowSqrtStatic`), no la partida nueva temporal `SaveGameToLoad=0` del histórico.
8. En `Update` candidato, `IsPlayerInPositionForConversation` puede ir incrustada o llamarse directamente en la DLL; en ambos casos usa los mismos proveedores.

## Qué NO cubre

- Sin cobertura exhaustiva de estado/flags x87 ni NaN/infinito más allá de los 18 casos adversos históricos (el histórico no ensaya NaN, infinito, `INT_MIN` ni valores no finitos).
- Sin contrato de registros crudos (EDX/ECX/flags al salir) de ninguna de las dos funciones.
- `Clear(bool)` no tiene hook y no se prueba directamente; solo lo que ya ejerzan los fixtures (ninguno la llama).
- Sin reproducción de sonido, dibujo de subtítulos, diálogos de misión completos ni sesiones prolongadas; sin rutas con excepciones.
- Matrices y datos auxiliares del jugador son fixtures de lectura fuera de `inputs_intact` por bytes.
- No prueba independencia del EXE ni que el cuerpo compilado coincida instrucción a instrucción con el original.

## Uso (cuando se autorice)

```
python reversing/CppConversationForPed/build_probe.py
python reversing/CppConversationForPed/run_local.py
python reversing/CppConversationForPed/audit/audit_run.py CppConversationForPed <carpeta-de-resultados> --archive
```
