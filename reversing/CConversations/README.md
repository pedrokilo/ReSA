<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CConversations y subsistema Conversations

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

`CConversations` conserva **10/10** entradas reconstruidas (eran 11/11 hasta el 01/10/2026: se retiró `EnableConversation`, que no existe como función propia en el EXE y cuya dirección caía dentro de `Clear`; ver [HooksActivados](../HooksActivados/README.md#direcciones-corregidas-2026-10-01)). `DoneSettingUpConversation` (0x43ADB0) fue validada con 2.048 pares original/candidato dentro de GTA. Coincidencia exacta de tablas, nodos temporales, referencias, estados y contadores. Partidas y estado del jugador restaurados. Hook habilitado despues de validar.

Ejecutar desde el unico PROBAR.cmd: PROBAR.cmd --case conversations; tambien incluido en --all. Sonda archivada, no una prueba conjunta de estabilidad del ASI actual.

Casos: 0-12 nodos, nombres duplicados, enlaces ausentes y diferencia de mayusculas, falta de nodos libres, distintos huecos de conversaciones y subtitulos. Se conserva la seleccion original del ultimo nombre coincidente y el fallback al nodo 0 cuando la tabla esta llena. Se requiere un hueco de conversacion, como en el original.

Limites: comprueba configuracion y referencias; no reproduccion de dialogos, audio, subtitulos ni estabilidad general. Evidencia, hashes y restauracion en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.


## Estado actual del subsistema

La auditoría de hooks mostró que el directorio `Conversations` contiene owners adicionales que antes no aparecían en el catálogo global. Ya están registrados y documentados:

| Owner | Reconstruidas / total | Pendientes |
| --- | ---: | --- |
| `CConversations` | 10/10 | 0 |
| `CConversationNode` | 1/1 | 0 (dirección de `ClearRecursively` corregida a 0x43A710 el 01/10/2026) |
| `CPedToPlayerConversations` | 3/3 | Ninguna |
| `CConversationForPed` | 2/2 | Ninguna |

Por tanto, **10/10 describe únicamente a `CConversations` y no a todo el subsistema Conversations**. Las dos funciones de `CConversationForPed` se completaron el 30 de septiembre de 2026: [4.702 pares y alcance](../ConversationForPed/README.md). `CPedToPlayerConversations::Update` también cuenta con su banco propio: [6.266 pares y alcance](../PedToPlayerConversations/README.md). Las cuatro clases suman 17/17 entradas reconstruidas; sus bancos separados no constituyen una prueba conjunta de todo el subsistema.

La integridad de este registro se comprueba con:

```powershell
python contrib/audit_hook_catalog.py --strict
```
