# CppTrigSwim: migración trigonométrica C++

Preparado offline, pendiente de compilación, ejecución y aprobación por coordinación. Derivado de Mix11 sin editar el banco histórico. Única identidad ejecutada: CTaskSimpleSwim::ProcessControlInput, 0x688A90; 2048 pares.

Conserva toda la tabla original de hooks, su conmutación OFF/ON y el índice f original de la semilla. Mantiene fixtures, trazas, snapshots y gates de igualdad, switches y clean. Registra ambos objetos laterales completos incluso en éxito o fallo; retorno void/nulo legítimo permanece dentro del objeto, nunca como registro lateral nulo. El analizador vuelve a exigir objeto completo, switches, clean, ret y trace.

Conserva restauración histórica de globals, referencias y pools; exige también enabled/locked finales de todos los hooks. Los colaboradores instrumentados y sus exclusiones son los documentados en reversing/Mix11/README.md y comentarios del HPP heredado. No certifica independencia transitiva del EXE ni prueba exhaustiva del mundo real.

Este banco heredado compara resultados y memorias numéricas observables; no contiene captura física FP108 ni comparación completa de flags x87, EFLAGS o ABI. No se introduce tolerancia ni normalización. La fidelidad adicional exige que coordinación compruebe código emitido de los helpers y equivalencia de los callers antes/después; no se da por demostrada aquí.

Manifiesto incluye fuentes históricas, TrigSimpleCpp.cpp/.h, CMake, InjectHooksMain y RH. Los scripts usan exclusivamente E:/JUEGOS/GTA San Andreas - CVehicle Test mediante configuración en memoria, sin editar runtime_runner.py. Solo coordinación compila y ejecuta en serie.
