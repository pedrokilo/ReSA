# CConversations y subsistema Conversations

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

`CConversations` conserva **11/11** entradas reconstruidas. `DoneSettingUpConversation` (0x43ADB0) fue validada con 2.048 pares original/candidato dentro de GTA. Coincidencia exacta de tablas, nodos temporales, referencias, estados y contadores. Partidas y estado del jugador restaurados. Hook habilitado despues de validar.

Ejecutar desde el unico PROBAR.cmd: PROBAR.cmd --case conversations; tambien incluido en --all. Sonda archivada, no una prueba conjunta de estabilidad del ASI actual.

Casos: 0-12 nodos, nombres duplicados, enlaces ausentes y diferencia de mayusculas, falta de nodos libres, distintos huecos de conversaciones y subtitulos. Se conserva la seleccion original del ultimo nombre coincidente y el fallback al nodo 0 cuando la tabla esta llena. Se requiere un hueco de conversacion, como en el original.

Limites: comprueba configuracion y referencias; no reproduccion de dialogos, audio, subtitulos ni estabilidad general. Evidencia, hashes y restauracion en validation-summary.json. Compilacion normal conjunta instalada solo en la copia de pruebas; respaldo y hashes de partidas verificados.


## Estado actual del subsistema

La auditoría de hooks mostró que el directorio `Conversations` contiene owners adicionales que antes no aparecían en el catálogo global. Ya están registrados y documentados:

| Owner | Reconstruidas / total | Pendientes |
| --- | ---: | --- |
| `CConversations` | 11/11 | 0 |
| `CConversationNode` | 1/1 | 0 |
| `CPedToPlayerConversations` | 2/3 | `Update` |
| `CConversationForPed` | 0/2 | `Update`, `IsPlayerInPositionForConversation` |

Por tanto, **11/11 describe únicamente a `CConversations` y no a todo el subsistema Conversations**. Las dos funciones de `CConversationForPed` y `CPedToPlayerConversations::Update` siguen delegando al ejecutable original y se contabilizan como pendientes en `docs/hooks.csv`.

La integridad de este registro se comprueba con:

```powershell
python contrib/audit_hook_catalog.py --strict
```
