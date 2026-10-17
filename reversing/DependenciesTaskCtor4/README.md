<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesTaskCtor4

Propuesta aplicada en producción y llamadas de las clases activadas en InjectHooksMain; validación final aprobada en corrida `20261010-192735-616118`: **2 funciones, 4.096 pares, 0 diferencias y 0 registros nulos**. Constructor SitInChair675C30..675C79 (ret0C) y WaitForPed683340..6833B2 (ret10h), 2048 pares cada uno, 4096 total. Se verificaron RET completos leyendo margen adicional del EXE.

Ambos métodos Constructor naked implementan toda la entrada nativa. No llaman constructores implícitos C++ ni inicializan padding por accidente. Integran las escrituras de base61A390/61A3B0 localmente; conservan vptr EXE para las virtuales todavía originales. RH_ScopedClass registra sólo Constructor y no requiere una vtable DLL parcial. No se acredita reconstrucción de las clases completas ni de sus destructores.

Sit buffer2Ch: info8, durationC, mode10, anim14=0, campo18=-1, timer1C/20=0 y flags24/25/28/29/2A=0. Padre4=0. Padding11..13,26..27,2B intacto. Argumentos info nulo/no nulo apuntan a registro propio sin dereferenciarlo. Todas las ramas de esta función están cubiertas (no tiene condiciones).

Wait buffer2Ch: padre4/subtask8=0,targetC,radius10,time14,rotate18,timer1C/20=0,flags24/25=0; padding19..1B/26..27 y frames28 intactos. Ped NO nulo en todos los casos: original llama571B70 sin guardia nula. Lista de referencias propia con8nodos y libres garantizados; casos vacía/preexistente/duplicada. Colaborador real por entradaEXE con estadoRH actual registrado en begin, sin stubs y sin poda de pools. Su SEH original se enlaza y restaura con las instrucciones del EXE.

Comparación exacta de2Ch bytes incluidos vptr/padding, retorno this, ECX/EDX/EAX, ESP, EBX/ESI/EDI/EBP y arena de referencias. Inicial EAXsentinel constante; rutasRH OFF/ON verificados y pEmptyList repuesto antesPumpMessages. No se construyen objetos mediante new ni se invocan destructores pendientes.

Coordinador: aplicar source-proposal.patch y activar las dos llamadas comentadas InjectHooksMain (SitInChair1015 y WaitForPed1281), incluir headerSit si falta. No cambiar hooks virtuales no probados. Ejecutar en serie: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh DependenciesTaskCtor4 3`.

El gate captura FS:[0] antes/después del call y exige seh_restored; no presupone que un epílogo naked lo restaure automáticamente.

Se conserva el handler SEH original del EXE `0x846798`. Las pruebas ejercitan el camino normal completo y comprueban la restauración de FS:[0]; no certifican el desenrollado por excepciones ni independencia del manejador original. La advertencia C4733 queda suprimida sólo en el Constructor de WaitForPed porque ese handler pertenece al EXE y no a la tabla SafeSEH de la DLL.

Evidencia final: [resumen](validation-summary.json), [auditoría](runs/20261010-192735-616118/approval-audit.json), [comparaciones](runs/20261010-192735-616118/comparisons.jsonl.gz) y [resultado](runs/20261010-192735-616118/RESULTADO.txt). SHA-256 JSONL descomprimido: `2a568acfefefe8a714823dc7c8138c94d74508aa8eb72fc03d24a3ba7d39567d`; ASI ejecutado: `67d4d85cb7b467fe7b92b7fbea9bebce802e63fc39c2864fb9ec5907a25b86a9`. Rutas RH OFF/ON, objetos/padding, retorno, ESP/no volátiles, FS:[0], referencias, restauración, partidas y configuraciones verificados. Se mantienen los límites SEH antes descritos.
