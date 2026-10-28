<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CConversationForPed — 2/2

`IsPlayerInPositionForConversation` (`0x43AC40`) y `Update` (`0x43C190`) reconstruidas y habilitadas el 30/09/2026. Las dos entradas quedan sin bloqueo. `Clear` ya estaba implementada y no se modifica.

## Comportamiento reconstruido

La referencia es el ejecutable local SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Se conserva el tamaño de la clase (`0x1C`) y se sustituyen las llamadas delegadas de ambas funciones.

La comprobación espacial exige distancia tridimensional máxima de 4, producto de orientaciones no positivo, velocidad horizontal máxima de `0.01f`, ausencia de daño reciente del jugador y disponibilidad de ambos participantes. Las conversaciones aleatorias además comprueban seguidores y pasan esa modalidad a la consulta de disponibilidad. Los tres cálculos conservan el orden de instrucciones x87 y su precisión activa: convertir todo a `float` o `double` cambia resultados en los límites.

`Update` conserva los plazos unsigned de 400, 4.000 y 10.000 ms, incluida su suma con desbordamiento. La respuesta negativa tiene prioridad; una rama ausente permite probar la otra respuesta. Las voces positivas usan los 16 bits bajos del contexto; las negativas solicitan audio de misión mediante `AwkwardSay`. Los subtítulos emplean las claves `Y`/`N`, las colas y los flags originales. Se conserva el orden de cambios de nodo, estado y tiempos.

## Evidencia y límites

[Resumen diferencial](validation-summary.json): **4.702 pares exactos dentro del motor**, con **2.414 de posición y 2.288 de Update**. Cada par ejecuta el original y el candidato por separado mediante conmutación de hooks; verifica las instrucciones originales de entrada, contadores 0/1, retornos, estado de conversación, buffer de texto y trazas ordenadas. Dos fixtures independientes mantienen intactos los participantes y los nodos. El banco restaura los hooks, nodos, texto, temporizador, control de precisión y estado del jugador real. Cierre normal con código 0, INI restaurados y partidas sin cambios por hash.

| Familia | Pares |
| --- | ---: |
| Distancia, orientación y velocidad por eje | 154 |
| Daño reciente y desbordamiento | 48 |
| Disponibilidad, seguidores y modalidad aleatoria | 32 |
| Posición sin matriz, descarte por distancia | 2 |
| Distancia y velocidad con ejes combinados | 2.160 |
| Casos adversos x87: 24, 53 y 64 bits | 18 |
| Estados, teclas y límites temporales | 1.200 |
| Ramas, subtítulos y contextos de voz | 1.024 |
| Voz de la pregunta | 32 |
| Desactivación por participante, habilitación o posición | 32 |

`Probe.hpp` controla las consultas de disponibilidad, seguidores, entrada, traducción, subtítulos y voz; original y candidato reciben los mismos proveedores. La comprobación geométrica y la máquina de estados se ejecutan completas; `Update` también utiliza la comprobación espacial correspondiente a cada versión. Las trazas incluyen el estado observable en cada llamada.

Esto acredita los contratos y escenarios descritos, no reproducción de sonido, dibujo de subtítulos, diálogos de misión completos, sesiones prolongadas ni independencia del EXE. Las entradas siguen dependiendo de servicios del juego. No se repite toda la suite. Una repetición del mismo banco no aumenta los 4.702 pares únicos. Los manifiestos fijan las fuentes compiladas, instrumentación, binarios y exclusiones; el registro vincula evidencia sin acreditar la vigencia de todas las dependencias del árbol.

Se usan coordenadas finitas y nodos válidos; no se ensayan NaN, infinito ni el contexto `INT_MIN`. Las matrices y los datos auxiliares del jugador son fixtures de lectura y no forman parte de la comprobación `inputs_intact` por bytes.

## Entrega normal

[Instalación verificada](installation-summary.json): **tres arranques de partida nueva, tres segundos en estado de juego por ejecución**, ambos saltos al ASI comprobados y tres cierres WM_CLOSE con código 0. ASI normal `cfb7ccd82ed7b4a289574699fa3a736c195f346e6abde04cd19d9333482949b9` y PDB `876716f9244e88fa45b38442ba041e16412c3442049bf9fd5692c8e0cbab01d4`, instalados en `bin/Debug` y exclusivamente en la copia de pruebas. El informe conserva los respaldos del ASI/PDB anterior. Los dos archivos fuente coinciden por bytes con la evidencia diferencial final, incluido el encabezado de progreso generado.

Los 4.702 casos se repitieron al incorporar ese encabezado; ambos logs tienen el mismo SHA-256. Es una repetición del mismo alcance, no 9.404 escenarios distintos. La comprobación de entrega cubre arranque, conexión de hooks y cierre, sin certificar una introducción completa ni estabilidad prolongada.

## Compilación y reproducción

Se mantienen las exclusiones de la entrega anterior: CustomBuildingRenderer y CustomBuildingDNPipeline desde `11fd6912f759d3bc93208a9def001b6dd751dbdf`, y TaskSimpleClimb desde `aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51`. `build.py` archiva manifiesto, fuentes y binarios anteriores y los restaura por bytes al terminar. La sonda se conecta temporalmente a `CGame::Process`; no forma parte de la compilación normal.

```powershell
python reversing/ConversationForPed/build.py --probe
PROBAR.cmd --case conversation-for-ped
python reversing/ConversationForPed/build.py
python reversing/ConversationForPed/check_normal.py --install
python contrib/audit_hook_catalog.py --strict
```

Usar un Python instalado si `python` resuelve al alias de WindowsApps. `latest-probe.json` y `latest-normal.json` señalan artefactos locales; recompilar si las fuentes cambiaron, porque el ejecutor rechaza sondas obsoletas. El analizador puede volver a auditar los logs archivados sin ejecutarlos. El caso usa el único `PROBAR.cmd` y el runner compartido.

Las pruebas usan exclusivamente `D:\JUEGOS\GTA San Andreas - CVehicle Test`, con partida nueva temporal (`SaveGameToLoad=0`). Se restauran los INI normales (`FastLoader=false`, `SaveGameToLoad=-2`). La instalación personal y las partidas se conservan. La incidencia previa de precarga de audio PROLOG1 sigue documentada en [PedToPlayerConversations](../PedToPlayerConversations/README.md); este trabajo no diagnostica ni corrige ese bloqueo.
