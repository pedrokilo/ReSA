<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigSwim: migración trigonométrica C++

Preparado offline, pendiente de compilación, ejecución y aprobación por coordinación. Derivado de Mix11 sin editar el banco histórico. Única identidad ejecutada: CTaskSimpleSwim::ProcessControlInput, 0x688A90; 2048 pares.

Conserva toda la tabla original de hooks, su conmutación OFF/ON y el índice f original de la semilla. Mantiene fixtures, trazas, snapshots y gates de igualdad, switches y clean. Registra ambos objetos laterales completos incluso en éxito o fallo; retorno void/nulo legítimo permanece dentro del objeto, nunca como registro lateral nulo. El analizador vuelve a exigir objeto completo, switches, clean, ret y trace.

Conserva restauración histórica de globals, referencias y pools; exige también enabled/locked finales de todos los hooks. Los colaboradores instrumentados y sus exclusiones son los documentados en reversing/Mix11/README.md y comentarios del HPP heredado. No certifica independencia transitiva del EXE ni prueba exhaustiva del mundo real.

Este banco heredado compara resultados y memorias numéricas observables; no contiene captura física FP108 ni comparación completa de flags x87, EFLAGS o ABI. No se introduce tolerancia ni normalización. La fidelidad adicional exige que coordinación compruebe código emitido de los helpers y equivalencia de los callers antes/después; no se da por demostrada aquí.

Manifiesto incluye fuentes históricas, TrigSimpleCpp.cpp/.h, CMake, InjectHooksMain y RH. Los scripts usan exclusivamente E:/JUEGOS/GTA San Andreas - CVehicle Test mediante configuración en memoria, sin editar runtime_runner.py. Solo coordinación compila y ejecuta en serie.
