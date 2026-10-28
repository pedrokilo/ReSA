<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesVehicleStats11: ciclismo y compatibilidad de modificaciones

Estado: **aprobado y registrado** en la corrida **20261010-214000-559687**: **2 funciones, 4.096 pares, cero diferencias, cero registros con lados nulos**, rutas RH OFF/ON y restauración acreditadas. [Evidencia comprimida](runs/20261010-214000-559687/comparisons.jsonl.gz), [resumen](runs/20261010-214000-559687/summary.json), [auditoría de admisión](runs/20261010-214000-559687/approval-audit.json) y [revisión independiente](audit-runtime.md).

SHA256 del log descomprimido: `b1b68fc3cdf9ebb791bfefc2a90b48b692a785b04cf5bc7bf6a9574888b589dc`. ASI ejecutada: `86e088f3a778e2a62409747feb0f18655f7fbacf0745c141baf98c9f76cadee4`. Los cuatro instrumentos aprobados permanecen inmutables. La propuesta fue aplicada por raíz antes de la corrida; su archivo conserva el cambio propuesto y las instrucciones originales. El alcance sigue limitado a las fixtures y colaboradores documentados.

| Identidad C++ | Dirección EXE 1.0 US | Pares previstos |
| --- | --- | --- |
| `CStats::UpdateStatsWhenCycling(bool, CBmx*)` | `0x55C780` | 2.048 |
| función global `IsValidModForVehicle(uint32, CVehicle*)` | `0x49B010` | 2.048 |

La segunda función es **global**, declarada así en `Vehicle.h`. El propietario `CVehicle` del registro/banco es el contexto de `RH_ScopedGlobalInstall` dentro de `CVehicle::InjectHooks`; no se presenta como método C++.

## Reconstrucción aplicada

`source-proposal.patch` modifica exclusivamente `Stats.cpp` y `Entity/Vehicle/Vehicle.cpp`: cuerpos completos x86 y altas RH selectivas en sus inyectores existentes. Ningún encabezado ni `InjectHooksMain.cpp` cambia. Conserva sólo etiquetas referenciadas por saltos; las demás direcciones son comentarios para evitar C4102/WX. El parche UTF-8 LF pasa `git apply --check` normal. Los puentes `push dirección; ret` llaman colaboradores reales, preservan argumentos/registros y no sustituyen resultados.

`Cycling.dis.txt` conserva el cuerpo `55C780..55C98C`, incluidos sus tres RET. `VehicleMod.dis.txt` conserva `49B010..49B0DF`, cinco RET; `WheelHelpers.dis.txt` prueba lectura de cuatro contadores signed int16 en `B4E470`, y cuatro grupos de 15 IDs signed int16 en `B4E3F8` (`imul eax,eax,0Fh`, `4C8758`). No se deduce identidad por rango de dirección.

## Rutas, ABI y estado

Cada lado llama la misma entrada EXE con RH de producción OFF/ON, comprobando destino antes de ejecutar. La raíz aplicó el parche y registró ambos hooks antes de construir. Captura ESP, EBX/ESI/EDI/EBP, EAX/ECX/EDX sin normalización y ambas palabras físicas de argumentos. El argumento bool tiene byte inferior 0/1 y bits superiores variados. Ciclismo reutiliza su segundo argumento físico en `55C952: mov [esp+10h],eax`: el analizador exige el tick truncado sólo en la ruta de incremento de habilidad; en las demás, el puntero original. No impone una falsa invariancia del segundo argumento. En compatibilidad compara AL definido para bool, además de EAX completo observado, y exige argumentos inalterados.

Los trece colaboradores conservan su modo inicial y se comprueban antes/después: bytes del EXE/DLL/cuerpo, flags enabled/locked/reconstructed, resolución E9 y módulo ASI. FatMuscle `55C470`, Increment `55C180`, Display `55B980` y WheelCount/WheelModel `4C8740/4C8750` deben estar reconstruidos y ON en ambos lados. Son implementaciones compartidas previamente disponibles; este banco no vuelve a aprobarlas. FindPlayerInfo, HUD, GetStatValue, texto y StatsCheck mantienen sus rutas iniciales. Los CRT `_ftol` `821B40` y `ceil` `823820` permanecen colaboradores originales reales, no funciones admitidas de juego.

Snapshot de 24 bloques sin filtrado: estadísticas completas `B78200..B79537`, ayuda HUD, buffers de texto, ambos PlayerInfo, bandera pad, nueve controles auxiliares, fallback GXT, timestep, selector CPU CRT, tabla de ruedas con márgenes, dos slots de modelos y tres buffers de fixtures completos con padding/canaries. FNSAVE/FRSTOR de 108 bytes y MXCSR se restauran por lado, antes/después de escribir y al final. Cada ejecución vuelve al estado inicial, incluyendo código y flags de los hooks y colaboradores. `mod_globals_readonly` se exige en el dominio de la función global; es no aplicable al ciclismo, que modifica estadísticas deliberadamente.

## Fixtures y límites reales

Son **fixtures sintéticas de memoria**, no vehículos construidos, no elementos de pools y no modelos cargados. Se reserva un cuerpo de 0xA00 bytes con márgenes para los offsets usados y dos layouts de modelo de 0x320 bytes con márgenes. El EXE y colaboradores sólo leen esos campos; ninguno ejecuta un virtual ni recibe el BMX de ciclismo. Se comprueba que los tres buffers permanecen íntegros. No se atribuye cobertura de constructor ni gameplay vivo a estas fixtures.

La función global necesita resolver ambos modelos en la tabla real. El banco busca dos slots NULL distintos entre 19000 y 19999 y aborta si no existen. Instala únicamente esos dos punteros durante cada llamada síncrona y restaura antes de procesar mensajes. Los IDs caben en int16 positivo; grupos de rueda 0..3 y cantidades 0..15 preservan dominio de los helpers. No hay asignaciones ni liberaciones simuladas, ni colaboradores mock. La demostración de slots NULL se registra en begin y la restauración final verifica sus cuatro bytes originales.

Ciclismo combina 16 perfiles ×128 valores: bool, pedal positivo/negativo/cero/NaN, contactos 0/1/2, velocidad a ambos lados y exactamente en 0.05 y 0.2, NaN, factores 0/1/1.5/3, límites negativos/cero/vecinos/inf/NaN, contadores con bit de signo y wrap y timestep negativo/cero/fraccional. El oráculo externo calcula acumulación unsigned con precisión x87 de 24 bits, truncado, ceil y wrap; comprueba ambos contadores contra los bytes del snapshot. Exige todas las ramas de acumulación/concesión/omisión y fronteras activas mediante inputs y efectos, sin afirmar traza de instrucciones.

Compatibilidad cubre ruedas vacías/primer/último/acierto/fallo, los cuatro grupos, lista de 18 mejoras primer/medio/último/ausencia y sentinelas -1, bit100 con tipo no-wheel, tipo wheel sin bit100, tipo nitro con bit100 y las cuatro decisiones nitro (bloqueo, otro tipo, flag sí/no). AL se verifica con un oráculo independiente de listas y flags. Todos los bloques se comparan completos y deben permanecer intactos en esta función.

No es producto cartesiano de todos los inputs posibles. No incluye punteros NULL, índices negativos de modelos ni grupos fuera de dominio, porque el EXE dereferencia sin protección. No incluye alt-tab real, interacción concurrente o vehículos construidos. 4.096 pares completos no nulos son el requisito previo a cualquier admisión.

## Callers originales

`6C19CE: xor ecx,ecx; 6C19D0: mov cl,[esi+614h]; 6C19D6: shr cl,5; 6C19D9: push esi; 6C19DA: and ecx,FFFFFF01h; 6C19E0: push ecx; 6C19E1: call 55C780; 6C19E6: add esp,8; 6C19E9: mov ecx,edi`. El caller real prepara un bool y BMX, descarta ambos argumentos y sustituye ECX inmediatamente. El banco observa también la reutilización de su slot en la callee.

`49BD82: push 0; 49BD84: push -1; 49BD86: call 56E0D0; 49BD8B: push eax; 49BD8C: push esi; 49BD8D: call 49B010; 49BD92: add esp,10h; 49BD95: test al,al`. Su retorno contractual es AL, el caller limpia argumentos cdecl. Ambos callers constan en `docs/MemoryMap/binary_calls.csv` y fueron desensamblados con `dis.sh`.

## Comprobación offline y ejecución pendiente

AST de ambos scripts correcto; las 45 fuentes del manifiesto existen. Generación offline de los 2.048 casos por función confirma todas las categorías exigidas por el analizador. Estas comprobaciones no son prueba dentro del juego. La raíz ejecuta `build_probe.py`, después `run_local.py`, archiva la evidencia y sólo admite tras pasar rutas/restauración/oráculos y revisión independiente. El analizador rechaza diferencias, resultados NULL, casos/pares repetidos, cobertura ausente, canaries alterados, ruta incorrecta o estado no restaurado. No cambia `runtime_runner.py`, catálogos ni documentos congelados.

## Reparación previa a ejecutar

La primera compilación detectó C2664 porque `InstallFailureCapture` requiere un prefijo const char*, no un stream. Se corrigió exclusivamente el inicio del instrumento: protocolo igual al Stats10 aprobado (`GetModuleFileName`, marker `dependenciesvehiclestats11-runtime.start`, salida `dependenciesvehiclestats11-runtime.jsonl`, evento `waiting_for_player`, instalación de captura por prefijo, espera del jugador y ausencia de cutscene antes de marcar done). No cambia la fuente propuesta, fixtures, ABI, oráculos ni las rutas de colaboradores. La repetición posterior aprobó la corrida 20261010-214000-559687 indicada arriba.
