<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente — DecisionRelationship6

Corrida 20261010-200529-431553. Auditoría exclusivamente de lectura: sin compilación, juego, modificación de fuente ni instrumentación.

**Resultado: 4 funciones, 6.272 pares, cero diferencias, cero NULL, todos accepted/equal y contratos/globalrestore/RH/ABI correctos. Hallazgos: 0.**

| Función | Pares | Casos consecutivos únicos |
|---|---:|---|
| CDecisionMakerTypes::RemoveDecisionMaker | 2048 | 0..2047 |
| CDecisionMakerTypes::AddDecisionMaker | 2048 | 0..2047 |
| CDecisionMakerTypes::LoadEventIndices | 128 | 0..127 |
| CPlayerRelationshipRecorder::RecordRelationshipWithPlayer | 2048 | 0..2047 |

Un único complete6272/state_restored=true, sin eventos error. Coinciden los conteos de logs, summary, validation-summary y approval-audit; las cuatro identidades apuntan exactamente a esta evidencia en approved.json. SHA descomprimido: `1e70cd5a9f7a53ecb80de1baf8088acb77e20c7085e59fcc9b7bd407ebd2c595`; summary y RESULTADO también coinciden por bytes. ASI: `e3747448ce7356cac3505e44b073457f0c17df101d258e926af60c7c8560fbcc`. Saves/configs restaurados se sustentan en los flags históricos del runner y sus hashes antes; no se inventa una comprobación retrospectiva de snapshots posteriores.

## Fidelidad de fuente contra EXE

Se compararon mecánicamente las instrucciones de cada cuerpo naked contra dis.sh, normalizando sólo espacios, etiquetas locales y nombres de puentes a sus direcciones originales. Los siete puentes son push dirección; ret y mantienen ABI/rutas reales. No se eliminó ninguna rama. AddDecisionMaker incluye además NOP6070BF: byte90 comprobado directamente en la sección PE del EXE; dis.sh filtra nop al presentar instrucciones, por lo que se excluye sólo esta instrucción neutral al comparar las listas. Las55 instrucciones restantes coinciden exactamente.

| Cuerpo | Instrucciones | Coincidencia |
|---|---:|---|
| RemoveDecisionMaker | 75 | True |
| AddDecisionMaker | 55 | True |
| LoadEventIndices | 5 | True |
| RecordRelationshipWithPlayer | 79 | True |

## Relación: seis recorridos y ramas 3/7

Se conservan el barrido sin coincidencia, la limpieza de primera coincidencia (incluidos duplicados), la salida sin tarea activa, primer Kill, Follow y segundo Kill con sus salidas/guardas. El código conserva las escrituras de tipo3 en61A258 y tipo7 en61A28E, con epílogos completos RET4 en61A264/61A29B. Los fixtures usan tareas reales de cuatro clases más ausencia, los cinco slots primary y las17 alternativas de posición (16 posiciones y ausente), con duplicados/slot0 nulo. No se necesitan clasificadores falsos para alcanzar escrituras imposibles.

Sea S=ESP después del prólogo Record. Primer Kill: a3=S+1C por61A20D, a2=S+15 por61A212 (ya hubo push); después de tres pushes61A225 lee[ESP+28]=S+1C, precisamente a3. Follow: a3=S+13 por61A230 y a2=S+16 por61A235;61A248 lee[ESP+1F]=S+13=a3. Segundo Kill: a3=S+14 por61A267 y a2=S+17 por61A26C;61A27F lee[ESP+20]=S+14=a3. Ninguna lectura observa a2.

EXE6985ED/6985F0 pone a2/a3 en0;6985F8..698606 sólo cambia a2 a1 para GetTaskType3FB..3FD. Follow698618/69861B escribe0 en ambos outputs. Fuente TaskCategories.cpp conserva esos mismos contratos. Por ello los tres tests reciben a3=0: tipo3/7 son inalcanzables con colaboradores originales. Las tres comparaciones, ambos caminos de escritura y sus guardas permanecen en el candidato sin simplificarlos. La validación dinámica prueba las ramas alcanzables y los virtuales reales; la conservación de ramas inalcanzables se acredita estáticamente, no se afirma ejecución de ellas.

Frecuencias por tarea [ausente,Fighting,Gun,Throw,StandStill]: `{0: 410, 1: 410, 2: 410, 3: 409, 4: 409}`; frecuencias por slotprimary: `{0: 410, 1: 410, 2: 410, 3: 410, 4: 408}`. Todas las alternativas0..16 de coincidencia están presentes.

## Restauración y colaboradores

Cada original/candidate exige route_ok, stack_restored, nonvolatile_preserved, seh_restored, contract_ok y globals_restored. Se restaura pedpool,20flags active/group, ocho grupos completos y sus flags, tiempo, referencias, directorio Windows y dos buffers CFileMgr antes de PumpMessages. Las tareas se retiran del manager antes del destructor real; no quedan instaladas en mundo. Finale verifica fichero real byte a byte y rutas RH/bytes iniciales. LoadEventIndices128casos usa PedEvent.txt real (1298 bytes), mismo bloque de índices en todos, contrato de handles y ausencia de cambios fuera C034..C1B4.

Las siete rutas colaboradoras declaradas por begin:
- 0x600b50: jumps_to_dll=True, bytes=[233, 242, 47, 223, 111].
- 0x5f7340: jumps_to_dll=True, bytes=[233, 241, 117, 222, 111].
- 0x6006b0: jumps_to_dll=True, bytes=[233, 141, 79, 221, 111].
- 0x5bb9f0: jumps_to_dll=False, bytes=[129, 236, 4, 2, 0].
- 0x681720: jumps_to_dll=True, bytes=[233, 58, 66, 216, 111].
- 0x6985e0: jumps_to_dll=False, bytes=[139, 68, 36, 12, 139].
- 0x698610: jumps_to_dll=False, bytes=[139, 68, 36, 8, 139].

Estas rutas permanecen reales compartidas; no se acredita independencia transitiva universal. Sin stubs según begin e inspección del banco. La prueba de globals_restored aparece en cada lado de los6.272pares, y no sólo en la cola.

## Manifiestos

Bank_sources coincide íntegramente con SHA actuales y approval-audit; tested_sources coincide entre los dos manifiestos históricos. Evoluciones posteriores de fuente (no se exige Main actual al histórico): ninguna.

## Hallazgos

Ninguna discrepancia encontrada.
