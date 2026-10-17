<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPointLights — Init y RemoveLightsAffectingObject

Reconstrucción y banco dentro del juego completo GTA SA 1.0 US. **APROBADO** en la corrida final [20261010-182800-435674](runs/20261010-182800-435674/summary.json): **dos funciones / 1.024 pares, cero diferencias, cero observaciones nulas y estado restaurado por cada par**, con RH de producción OFF/ON y comprobaciones de ESP/registros no volátiles correctas. Guardados intactos y configuraciones restauradas. Evidencia y fuentes enlazadas en [approval-audit.json](runs/20261010-182800-435674/approval-audit.json) y [comparisons.jsonl.gz](runs/20261010-182800-435674/comparisons.jsonl.gz).

EXE original SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`; sonda ejecutada `b8bc4bc10b1e9d88cd27370200b09eba23c74cf5df121a783130a1883e5811a6`; log sin comprimir `9a7ae11fb20ee6ba2cdbd6faf215b140df83d3d022615c344e8a50f7e0176530`.

| Función | Dirección | Pares aprobados | Evidencia EXE y comportamiento |
|---|---|---:|---|
| Init | 0x6FFB40 | 512 | 0x6FFB47–0x6FFB51: 32 dwords cero en 0xC3F050; 0x6FFB77: NextCachedValue=0; 0x6FFB86–0x6FFB99: 32 posiciones cacheadas cero. **No reinicia NumLights ni aLights.** |
| RemoveLightsAffectingObject | 0x6FFFE0 | 512 | 0x6FFFE0–0x6FFFEC carga Scene.m_pRpWorld, llama a 0x7359E0 y retorna cdecl. El colaborador 0x7359E0–0x735A1D ignora world, escribe cero en byte +2 de seis RpLight y en contador 0xC88708. Se expandieron esos efectos en la fuente, sin volver a llamar al EXE ni utilizar stubs. |

**Total: dos funciones y 1.024 pares**, con pair global 0..1023 y case 0..511 por función. RH registrados OFF frente a ON, llamando siempre a la entrada original. El banco exige ambos hooks registrados, devuelve cada uno al estado inicial y verifica bytes de EXE/símbolo/cuerpo. No modifica docs/hooks.csv ni el registro de validaciones.

Fixtures: 256 patrones de bytes repetidos en dos familias de punteros (seis objetos diferentes o alias de uno/tres objetos). Las seis entradas siempre apuntan a RpLight falsos válidos, con tipo rpLIGHT, byte de flags inicial 0..255 y guardas antes/después. El contador de luces extra varía 0..8; NumLights 0..32. Los buffers, padding, contador/lista/cachés y guardas se comparan sin máscara. Además de comparar ambos lados, se exige el efecto exacto esperado: sólo las regiones/bytes indicados por el EXE pueden cambiar.

Se guardan/restauran antes de cada Write la región 0xC3F040..0xC3F86F, los seis punteros/contador de luces extra y guardas 0xC886E0..0xC88717, Scene, los objetos falsos y el control de FPU. El puntero world es un token opaco que el colaborador original ignora; Scene.camera conserva su valor real. No se ejecutan funciones RenderWare, dibujo, audio ni desreferencias de entidades reales. Cada registro original/candidate contiene un objeto completo, incluso al fallar.

Ambas funciones son cdecl sin argumentos. El banco mide ESP y conservación de EBX/ESI/EDI/EBP en cada llamada original/candidata. Los seis call sites exportados de Remove (0x553382, 0x55339D, 0x5533B9, 0x5533D9, 0x5533F9 y 0x553E21) retornan, saltan al entry o continúan con llamadas 0x735D30/0x735C70, sin consumir EAX/ECX/EDX ni flags inmediatamente después. Los registros volátiles y flags no se comparan como parte del contrato cdecl. No se afirma haber probado todos los callers indirectos.

## Pendiente fuera de este banco

**GenerateLightsAffectingObject (0x6FFBB0) sigue delegado al EXE y no se registra aquí.** Su cuerpo 0x6FFBB0–0x6FFE68 filtra por tipo/radio, calcula distancia/atenuación x87, actualiza iluminación acumulada y multiplicador anti-light; la rama directional calcula un producto particular y llama a max 0x420800 en 0x6FFDE0; en 0x6FFE43 llama a 0x735840 con world, dirección normalizada y RGB. El colaborador 0x735840 selecciona/configura luces extra y accede a más estado del renderer. Probar sólo casos de descarte no cubriría el método. Requiere reconstrucción y banco propio que trace esa colaboración o la ejecute sobre un mundo/luces/cámara falsos coherentes, con restauración de toda su reserva, antes de declarar equivalencia. No se fuerza su cobertura ni se altera su fuente.

Comando del coordinador, en serie: `bash reversing/_handoff/bench.sh DependenciesPointLights2 3`.
