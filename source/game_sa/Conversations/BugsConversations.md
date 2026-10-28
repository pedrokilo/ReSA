<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fallos del juego original en `source/game_sa/Conversations`

Revisión del 17 de octubre de 2026 por lectura del código reconstruido (`PedToPlayerConversations.cpp`, `ConversationForPed.cpp`, `ConversationNode.cpp`, `Conversations.cpp` y sus cabeceras).

**Alcance y honestidad de este documento**

- Las funciones de estas clases están validadas contra el EXE (hooks `Clear`, `Update`, `EndConversation`, `IsPlayerInPositionForConversation`, `ClearRecursively`, etc.). La reconstrucción repite el comportamiento del juego original **a propósito**, errores incluidos, porque el objetivo del proyecto es el resultado idéntico al EXE. Nada de lo que sigue se ha corregido en el código.
- Esto es una **lista de candidatos** obtenida leyendo el código, no una confirmación en ejecución. Cada entrada lleva su confianza:
  - **Confirmado:** se deduce directamente del código o de un comentario que dice que así lo hace el original.
  - **Probable:** el patrón es típico de GTA SA y el código lo encaja, pero no se ha provocado en el juego.
  - **Latente:** solo falla con ciertos datos de script o ciertos llamadores; hoy no tiene por qué ocurrir.
- Si se quisiera corregir alguno, debería hacerse como mejora opcional NOTSA (separada de la reconstrucción exacta, desactivable), y registrar la divergencia.

## Resumen

| # | Clase | Fallo | Confianza |
|---|---|---|---|
| 1 | `CPedToPlayerConversations` | El tema 7 (clima) no sale nunca | Confirmado |
| 2 | `CPedToPlayerConversations` | El tema de clima consulta el viento, no la lluvia | Confirmado |
| 3 | `CConversations` | La clave de la rama «sí» no se pasa a mayúsculas, así que una clave en minúsculas nunca coincide | Confirmado |
| 4 | `CConversations` | `AwkwardSay` en estado `LOADING` puede quedarse atascado para siempre y bloquear todas las conversaciones | Probable |
| 5 | `CPedToPlayerConversations`, `CConversations`, `CConversationForPed` | Referencias de entidad sin limpiar al sobrescribir o borrar el puntero | Probable |
| 6 | `CConversationNode` | `ClearRecursively` sin protección contra ciclos | Latente |
| 7 | `CConversations` | `SetUpConversationNode` sin límite en el buffer temporal (12 nodos) | Latente |
| 8 | `CConversations` | `DoneSettingUpConversation` sin slot libre: desreferencia nula o escritura en el nodo 0 | Latente |
| 9 | `CConversations` | `RemoveConversationForPed(nullptr)` y `FindConversationForPed(nullptr)` casan con slots libres | Latente |
| 10 | `CConversations` | `IsConversationAtNode` escribe en la cadena del llamador y desreferencia nulo | Latente |
| 11 | `CConversations` | Las claves de más de 6 caracteres se truncan en silencio y pueden colisionar | Latente |
| 12 | `CConversationForPed` | Documentación de `IsPlayerInPositionForConversation` que no coincide con el código | Confirmado |
| 13 | varias | Otros comportamientos dudosos menores | Varios |

## 1. El tema 7 (clima) no sale nunca — Confirmado

`CPedToPlayerConversations::Update`, estado `INACTIVE`:

```cpp
m_Topic = rivalGang ? CGeneral::GetRandomNumberInRange(8, 10) : CGeneral::GetRandomNumberInRange(0, 7);
```

La versión entera de `GetRandomNumberInRange` es `[min, max)` (ver `General.h`: `min + (int)(rand() * (1/32768) * (max - min))`). Con `(0, 7)` solo salen 0 a 6, de modo que el tema 7 (`CTX_GLOBAL_CONV_LIKE_WEATHER` / `CTX_GLOBAL_CONV_DISL_WEATHER` y su respuesta `CTX_GLOBAL_WEATHER_LIKE_REPLY`) es código inalcanzable. Con `(8, 10)` solo salen 8 y 9, lo que sí es coherente con los temas 8 y 9 del código.

Probable intención original: `(0, 8)`.

## 2. El tema de clima consulta el viento — Confirmado

Mismo método, `case 7`:

```cpp
if (CWeather::Wind < 0.1f) { ... LIKE_WEATHER ... }
else if (CWeather::Wind > 0.5f) { ... DISL_WEATHER ... }
```

El comentario del código dice «La variable consultada es Wind, no Rain». El tema trata del tiempo (se supone que de la lluvia), pero el original compara con el viento. Es además inalcanzable por el punto 1, así que hoy no tiene efecto visible.

## 3. La clave de la rama «sí» no se pasa a mayúsculas — Confirmado

`CConversations::SetUpConversationNode`:

```cpp
strncpy(node.m_Name, questionKey, 6u);
MakeUpperCase(node.m_Name);                 // nombre del nodo: mayúsculas
...
if (answerYesKey) {
    strncpy(node.m_NameNodeYes, answerYesKey, 6u);       // sin MakeUpperCase
} else {
    node.m_NameNodeYes[0] = '\0';
    MakeUpperCase(node.m_NameNodeYes);                    // se pasa a mayúsculas la cadena VACÍA
}
if (answerNoKey) {
    strncpy(node.m_NameNodeNo, answerNoKey, 6u);
    MakeUpperCase(node.m_NameNodeNo);                     // aquí sí
}
```

El código lo reconoce con el comentario «Como el original: la clave "sí" no se pasa a mayúsculas cuando existe». Después, `DoneSettingUpConversation` compara con `strcmp` la clave «sí» con el nombre (en mayúsculas) de los demás nodos. Una clave «sí» escrita en minúsculas nunca coincide y la rama queda en `-1` (conversación termina o no responde a «sí») sin ningún aviso. La rama «no» no tiene el problema. Las claves en mayúsculas funcionan, por eso pasa inadvertido.

## 4. `AwkwardSay` puede bloquear todas las conversaciones — Probable

`CConversations::Update`:

```cpp
case LOADING:
    if (AudioEngine.GetMissionAudioLoadingStatus(0) == 1) { PlayLoadedMissionAudio(0); m_AwkwardSayStatus = PLAYING; }
    break;   // las conversaciones normales NO se actualizan en este estado
```

Si el audio de misión del canal 0 nunca llega al estado 1 (por ejemplo porque la carga falla), `m_AwkwardSayStatus` se queda en `LOADING` y `Update` no vuelve a actualizar ninguna conversación. Además `AwkwardSay` usa el **canal 0** de audio de misión, que también emplean los scripts de misión: lo pisa sin comprobar si está en uso.

## 5. Referencias de entidad sin limpiar — Probable

El juego registra los punteros a entidades con `RegisterReference` para que se pongan a `nullptr` cuando la entidad muere. El patrón correcto exige limpiar la referencia anterior (`CleanUpOldReference`) antes de reasignar el puntero. Aquí no se hace:

- `CPedToPlayerConversations::Update` hace `m_pPed = ped; RegisterReference(m_pPed)` sin limpiar la referencia del peatón anterior. Cuando ese peatón antiguo muere, pone `m_pPed = nullptr` aunque ya apunte a otro, y la conversación en curso se corta (`!m_pPed` termina la conversación en `PEDHASOPENED` y `WAITINGFORFINALWORD`).
- Igual con `m_LastPlayerVehicle` (se re-registra en cada frame en que el jugador va en vehículo y nunca se limpia al cambiar de vehículo ni al terminar).
- `CConversationForPed::Clear` pone `m_pPed = nullptr` sin limpiar la referencia: el peatón conserva la dirección de ese slot. Si el slot se reutiliza para otro peatón y el viejo muere, borra el `m_pPed` de la conversación nueva.
- `CPedToPlayerConversations::EndConversation` y `Clear` tampoco limpian `m_pPed`.

## 6. `ClearRecursively` sin protección contra ciclos — Latente

`CConversationNode::ClearRecursively` recorre `m_NodeYes` y `m_NodeNo` sin conjunto de nodos visitados. El propio comentario avisa de que un ciclo provoca recursión indefinida. Los ciclos los puede crear un script: `DoneSettingUpConversation` resuelve las claves por nombre y no impide que un nodo apunte a sí mismo o a uno anterior (p. ej. un diálogo que repite la pregunta). Al borrar esa conversación (`RemoveConversationForPed`) se desbordaría la pila. Los grafos con un nodo alcanzable por dos caminos (rombos) se limpian dos veces, sin daño. Los índices tampoco se validan (se usa `data()` para evitar el `assert`).

## 7. Buffer temporal de 12 nodos sin límite — Latente

`SetUpConversationNode` escribe en `m_aTempNodes[m_SettingUpConversationNumNodes]` sin comprobar que sea menor que `MAX_NUM_TEMP_CONVERSATION_NODES` (12). Un script que defina más de 12 nodos escribe fuera del array. La nota del código lo reconoce («se espera que el llamador respete…»).

## 8. `DoneSettingUpConversation` sin slot libre — Latente

- Si los 14 slots de conversación están ocupados, `conversation` queda en `nullptr` y se desreferencia (`conversation->m_FirstNode`): caída.
- Si el pool de 50 nodos permanentes está lleno, el nodo recibe el slot 0 («fallback documentado»), con lo que **pisa un nodo ya usado por otra conversación** y no lo marca como reservado.
- Con cero nodos se usa `m_aTempNodes[0]` con datos antiguos.
- Si el peatón guardado en `m_SettingUpConversationPed` murió entre `Start` y `Done`, queda a `nullptr` por la referencia y se desreferencia al registrar la referencia nueva.
- Dos nodos temporales con el mismo nombre: la rama se resuelve al último que coincide.

## 9. `nullptr` casa con los slots libres — Latente

Un slot libre tiene `m_pPed == nullptr`. Por eso:

- `RemoveConversationForPed(nullptr)` llama a `Clear(false)` en **todos** los slots libres, y `Clear(false)` hace `m_Nodes[m_FirstNode].ClearRecursively()` con `m_FirstNode == -1`: lectura y escritura fuera del array (`m_Nodes[-1]`).
- `FindConversationForPed(nullptr)` devuelve el primer slot libre en vez de `nullptr`, y `EnableConversation(nullptr, ...)` modifica un slot libre.

## 10. `IsConversationAtNode` escribe en la cadena del llamador — Latente

```cpp
char* name = const_cast<char*>(pName);
MakeUpperCase(name);
```

Pasa a mayúsculas la cadena **del llamador en su sitio**. Si el llamador pasa un literal (memoria de solo lectura), escribe en ella. Además, `assert(conversation)` desaparece en la versión de lanzamiento y `FindConversationForPed` puede devolver `nullptr`, que se desreferencia justo después. Los comandos de script pasan un buffer temporal escribible, por eso funciona.

## 11. Las claves de más de 6 caracteres se truncan — Latente

`strncpy(..., 6u)` en los nombres. Dos claves que difieran después del sexto carácter (p. ej. `QUEST_1` y `QUEST_2`) pasan a ser iguales y la rama se resuelve al último nodo que coincida. Los nombres se guardan en 8 bytes; los dos últimos no se escriben nunca. Con claves cortas (las de los scripts del juego) no se nota.

## 12. Documentación que no coincide con el código — Confirmado

El comentario de `CConversationForPed::IsPlayerInPositionForConversation` (en la cabecera) dice que las conversaciones aleatorias también excluyen «seguidores y búsqueda policial». El método solo excluye a los **seguidores** (`CountMembersExcludingLeader() >= 1`); la búsqueda policial se comprueba en otro sitio (`CPedToPlayerConversations::Update`, no en este método).

## 13. Comportamientos dudosos menores

- **Distancia en 3D** (`ConversationTooFar`): cuenta la diferencia de altura; un peatón en un escalón o en un nivel superior a menos de 4 unidades en horizontal puede no poder conversar.
- **Orientación**: se descarta si el producto escalar de los dos vectores «hacia delante» es positivo, es decir, solo se admite que se miren de frente. Es coherente, pero no mira si el peatón está realmente de cara al jugador.
- **`CConversationForPed::Update` con `m_Enabled == false`** pone el estado a `INACTIVE` sin cancelar los subtítulos que estuvieran en pantalla (duran hasta 4 s).
- **`m_CurrentNode` no se reinicia** al alejarse el jugador: al volver, la conversación continúa desde el nodo en que estaba, no desde el primero, tras 10 s de ausencia.
- **`m_LastTimeWeWereCloseEnough` empieza en 0**: la primera `Update` después de 10 s de partida dispara la pregunta del peatón inmediatamente al acercarse.
- **Respuesta «sí» tras una apertura no positiva** (`CPedToPlayerConversations`): el jugador responde con un «descartar» y se pasa a esperar el final, sin que el peatón diga nada.
- **`m_bPositiveReply`** existe en memoria y no se usa en ningún punto del código reconstruido.
- **Plazos** en `uint32` con suma: al desbordar el temporizador del juego (≈ 49 días), las comparaciones del tipo `GetTimeInMS() > start + N` se invierten. Solo afecta a partidas de duración extrema.
- **`AttackPlayer`** crea una `CTaskComplexSequence` con `new` y un `CEventScriptCommand` en la pila que la recibe; si la cola de eventos clona la tarea, la secuencia original podría quedar sin liberar. No se ha comprobado cómo lo gestiona el EXE.
- **Probabilidad del gesto «mirando al jugador»** (`CTX_GLOBAL_EYEING_PLAYER`): se evalúa por cada peatón candidato que falla la apertura, por lo que su frecuencia efectiva depende del número de peatones inspeccionados (hasta 8 por llamada).

## Qué haría falta para comprobarlos

1. Para los puntos 1 a 3: bastaría un banco o una prueba dirigida en el juego que llame al método con una semilla fija y cuente los temas obtenidos (punto 1), y una conversación de script con claves en minúsculas (punto 3).
2. Para el punto 4: forzar un estado de carga del audio distinto de 1.
3. Para el punto 5: matar el peatón anterior con una conversación nueva en curso.
4. Para los latentes (6 a 11): construirlos con scripts de misión o con el banco, siempre en una copia del juego de pruebas.
