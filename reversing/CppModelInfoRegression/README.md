<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Regresión CModelInfo tras compilar ModelInfo.cpp con /Zi

APROBADO: corrida 20261011-010041-660495, 28 métodos, 5264 pares, cero diferencias y cero resultados nulos. Restauración, partidas y configuración verificadas por auditor independiente en audit/audit_run.py; evidencia archivada en runs.

Banco derivado de Small2 sin modificarlo. La campaña aprobada ejecutó exclusivamente sus 28 identidades CModelInfo, con 5264 pares: Initialise 8, ShutDown 64, ReInit2dEffects 8, cuatro consultas 1536, nueve Add 576 y doce Is 3072. Get2dEffectStore se mide en CppModelStore aparte: no se contabiliza como segunda migración.

Mantiene la tabla completa de 74 colaboradores y sus hooks OFF/ON, las semillas con el índice histórico original, los fixtures, las trazas y todos los campos que antes decidían igualdad. Se escriben objetos original/candidate completos tanto en éxito como en fallo; un retorno legítimamente nulo se conserva dentro del objeto y no reemplaza el registro lateral. El analizador exige switches verdaderos y campos ret/trace además de los gates heredados.

Mantiene los stubs de Small2: primitivas RW, memoria, Init2dEffects, SetColModel y SetTexDictionary. Initialise/ShutDown usan stores y vtables de prueba; consultas utilizan la tabla real de modelos del clon. Acredita resultados, memorias hash/contadores y orden de llamadas bajo estos colaboradores; no acredita independencia transitiva ni un contrato completo de EFLAGS, registros o x87. El banco Get2dEffectStore y la inspección del objeto cubren su migración propia.

La restauración final comprueba todos los rangos globales que conserva Small2, tiempo, cámara, área, foco, underwater y estados enabled/locked de los 74 hooks. No se rebaja el gate anterior. Como en Small2, el hash de stores cubre todo MI_BEGIN..MI_END y los datos se restauran antes de cada escritura/bombeo.

build_probe.py registra ModelInfo.cpp/.h, MemoryMgr.cpp/.h, InjectHooksMain.cpp, archivos RH, CMake y HookRoute; conserva PCH y las opciones por fuente aplicadas por el coordinador. Los scripts redirigen runtime_runner.GAME en memoria exclusivamente al clon E:/JUEGOS/GTA San Andreas - CVehicle Test. No editan runtime_runner.py ni la instalación original.

Solo el coordinador compila y ejecuta en serie. La evidencia dinámica de esta regresión está archivada y vinculada desde el registro de migración. No añade conversiones ni funciones a la cohorte.
