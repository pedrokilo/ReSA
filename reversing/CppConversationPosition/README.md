<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> SUPERADO el 11/10/2026: `ConversationMathCpp` ya no existe (los helpers pasaron a ConversationForPed.cpp) y la clase completa se valida con CppConversationForPed. Este banco y su corrida 20261011-135326-813627 quedan sólo como historia.

> Actualización 11/10/2026: este banco se compiló y ejecutó dentro del juego; la corrida 20261011-135326-813627 aprobó 2414 pares con cero diferencias (auditoría en runs/20261011-135326-813627/approval-audit.json). Las frases siguientes que dicen «no compilado ni ejecutado» describen el estado de cuando se escribió. Resultado y límites en [CppConversationStudy](../CppConversationStudy/README.md).

# CppConversationPosition: propuesta pendiente

**No compilado ni ejecutado.** Banco por pares dentro del juego para `CConversationForPed::IsPlayerInPositionForConversation` (0x43AC40): original = EXE con el hook conmutado a falso, candidato = DLL con `ConversationForPed.cpp` y los tres helpers C++ de `ConversationMathCpp.cpp` (`/O2 /Oi /arch:IA32 /fp:fast`) en lugar de los tres bloques `__asm` x87. Una sola función en la tabla, 2414 pares. No acredita nada hasta compilarse, ejecutarse y auditarse (`audit/audit_run.py`).

Estructura copiada de `../CppShadowSqrtStatic` (namespace `CppConversationPositionLive`, prefijo `cpp-conversation-position`, banco `CppConversationPosition`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `audit/audit_run.py`); contratos por lado al estilo `../CppIntegerWorld10`. Se redirige en memoria `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test`; se supone aplicado `../CppSqrtSmallStudy/IndependentReview/shadow-bank-clone-manifest.patch`. No se copia ningún `validation-summary.json`, `latest-*.json` ni aprobación previa.

## Fixtures heredados de `../ConversationForPed` (solo posición)

Mismos `Spec`, `Fixture::SetUp/Run` (dos fixtures independientes, jugador/peatón/nodos/`gString`/tiempo), mismos valores, orden y nombres de caso, comparación exacta (`original == candidate`) y restauración (nodos, texto, temporizador, palabra de control x87, bytes del jugador real, hooks).

| Familia | Pares |
| --- | ---: |
| Distancia, orientación y velocidad por eje (`distance` 84, `facing` 10, `speed` 60) | 154 |
| Daño reciente y desbordamiento (`damage`) | 48 |
| Disponibilidad, seguidores, aleatoria (`availability`) | 32 |
| Sin matriz (`placement-early-out`) | 2 |
| Ejes combinados (`mixed-distance` 1080, `mixed-speed` 1080) | 2160 |
| Adversos x87 24/53/64 bits (`x87-precision`) | 18 |
| **Total** | **2414** |

No se incluye ningún caso de `Update` (2288 pares históricos).

## Cambios inevitables respecto al banco histórico

1. El histórico instrumentaba el texto del candidato (`instrument.py`) para sustituir `FindPlayerPed`, `FindPlayerCoors`, `PedIsReadyForConversation` y el recuento de seguidores. `probe_build.py` no instrumenta fuentes, así que aquí los proveedores se parchean con `EntryPatch` en la dirección del EXE y también en la función de la DLL (como `CppShadowSqrtStatic` con `Generate`/`Free`). Requiere que esas funciones no estén expandidas en línea en el cuerpo candidato (Debug).
2. Los contadores `positionCalls` (0/1) que `instrument.py` insertaba en el cuerpo candidato ya no existen. Se sustituyen por el campo `switches` (original: bytes de entrada `83 EC 0C 56 57` y sin salto a la DLL; candidato: salto a la DLL), medido antes y después de cada ejecución.
3. `inputs_intact` pasa a exigirse en ambos lados (el histórico solo en el original); `fp_control` se registra en cada lado y se compara.
4. El histórico paraba en el primer fallo; aquí se siguen todos los casos y se cuentan fallos (como el modelo). Se añaden compuertas: tabla = 2414 casos generados, resultados `true` y `false` presentes, reparto por familias.
5. El candidato ahora ejecuta realmente `FindPlayerPed()->GetPlayerGroup().GetMembership()` (inline, lee el fixture en +0x480 y el pool real `CPedGroups`) antes del proveedor de seguidores; en el histórico esa cadena estaba sustituida por texto.
6. Se omiten los proveedores solo de `Update` (voz, teclas, mensajes, texto); el cuerpo de posición no los llama.
7. Se usa la configuración por defecto de `run_case` (como `CppShadowSqrtStatic`), no la partida nueva temporal `SaveGameToLoad=0` del histórico.

## Qué NO cubre

- Sin cobertura exhaustiva de estado/flags x87 ni NaN/infinito más allá de los 18 casos adversos históricos (el histórico no ensaya NaN, infinito ni valores no finitos).
- Sin contrato de registros crudos del cuerpo (EDX/ECX/flags al salir); el cuerpo candidato es código Debug y solo los helpers van optimizados.
- Sin cobertura de `Update`, ni de rutas con excepciones.
- Matrices y datos auxiliares del jugador son fixtures de lectura fuera de `inputs_intact` por bytes.
- No prueba independencia del EXE.

## Uso (cuando se autorice)

```
python reversing/CppConversationPosition/build_probe.py
python reversing/CppConversationPosition/run_local.py
python reversing/CppConversationPosition/audit/audit_run.py CppConversationPosition <carpeta-de-resultados> --archive
```
