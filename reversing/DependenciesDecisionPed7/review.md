<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión estática final de DecisionPed7

Revisión del banco ya aplicado y corregido, sin compilar ni ejecutar durante esta revisión. Aún pendiente de prueba. Sólo se han reforzado LiveComparison.hpp y README; no hubo nuevas mutaciones de producción después de mover GetRef a Pools.cpp.

Se extrajo de nuevo el desensamblado del EXE canónico y se compararon las instrucciones de producción, normalizando únicamente espacios, etiquetas internas y nombres de los dos puentes. Resultado exacto:

| Entrada | Instrucciones | Última instrucción | Diferencias |
| --- | ---: | --- | ---: |
| MakeDecision-Ped606E70 | 91 | 606F71: ret2C | 0 |
| MakeDecision-Group606F80 | 62 | 607041: ret20 | 0 |
| CPedPool::GetRef4442D0 | 14 | 4442F5: ret4 | 0 |
| EventScriptCommand::Destructor4B0A50 | 23 | 4B0A9D: ret | 0 |

Cada rango incluye margen posterior al RET; los NOP de alineación no se cuentan como cuerpo. Los puentes Decision7One/Many conducen a6040D0/606060 y conservan la pila mediante push dirección y ret explícitos. GetRef se declara en PedPool.h y su definición naked está fuera de clase en Pools.cpp, que ya estaba incluido en el manifiesto.

El gate entrega11/8/1/0 argumentos según función y no limpia argumentos thiscall una segunda vez. Captura EAX/ECX/EDX antes de llamadas auxiliares, compara ESP y los cuatro registros no volátiles, y FS:[0]. EAX inicial se fija idéntico para evitar diferencias artificiales en sus bits altos. Los nombres RH Ped/Group coinciden con los sufijos del catálogo de hooks. Registered es requisito de aceptación por llamada y del complete, evitando aprobar el fallback de EntryPatch si falta un RH.

El control anterior sólo observaba rutas de colaboradores en begin. Ahora helper_route_ok exige por llamada los bytes de entrada y cuerpo resuelto intactos, antes y después, y también al finalizar. Se verifican16 bytes nativos canónicos de6040D0/606060/821B1E/827B3D y el destino E8 de rand821B1E→827B3D. Los dos MakeDecision auxiliares permanecen originales: su wrapper C++ vacío no puede enmascarar la prueba. La semilla se toma de TLS+14 mediante el getptd original y se repone antes de cada lado; el estado resultante se compara. Los colaboradores pueden consumir cero o varias llamadas rand, y el contrato no presume una sola.

Los fixtures de decisión cubren los selectores reales B4/B8, las rutas player/random/mission/personal, eventos0..95 e índices0..40, seis choices, cuatro relaciones, ambos bool por choice, filtros y probabilidades nulas. El helper lee únicamente la decisión seleccionada, cuyos60 bytes se inicializan expresamente. Los objetos completos maker/ped/intel/group permanecen iguales; dos salidas int16 tienen canarios y Group no recibe esas salidas externas. Son objetos de almacenamiento privado con todos los campos leídos inicializados; no se simulan vtables ni se insertan peds incompletos en el mundo.

GetRef opera sobre siete posiciones válidas del pool real no propietario, stride7C4 y todos los valores de bitmap. Exige el resultado slot*256+bitmap, cabecera/almacenamiento/bitmap intactos y RNG sin consumo. Los punteros fuera del pool no pertenecen al contrato de esta tanda.

El destructor usa EventScriptCommand construido mediante placement y StandStill real creado con new, en un TaskPool real de cuatro slots de80h, sustituyendo temporalmente sólo el puntero global del pool. Se invoca una sola vez la entrada4B0A50 y no se vuelve a destruir el evento desde C++. La tarea se destruye por el slot0 real de su propia vtable; se captura y resuelve ese destino y se verifica intacto tras la llamada. También se capturan los cuerpos DLL directos de operator new/delete, porque la construcción C++ puede llamarlos directamente. Esto demuestra el destructor con ese colaborador real compartido, no reconstruye por transitividad el destructor de cada clase de tarea.

El contrato del destructor exige vptr final85AB38, objeto completo restante intacto, tarea presente en los casos impares, activación únicamente del bit libre del slot correspondiente, ID conservado, cabecera/cursor del pool sin variación y almacenamiento de tareas intacto fuera del vptr de la tarea destruida. En casos nulos no cambia el pool. Las huellas completas también se comparan entre original/candidate. Los pools privados no poseen sus buffers; Flush sólo rellena el almacenamiento privado y no destruye ni libera peds del mundo.

El handler83A018 sigue siendo el original del EXE y se verifica restitución de la cadena SEH por camino normal. No se fuerzan excepciones ni se afirma que ese handler pueda desenrollar una pila DLL en una excepción. La restitución del estado de coma flotante se amplió desde sólo PC a108 bytes x87 completos mediante FNSAVE/FRSTOR y MXCSR mediante STMXCSR/LDMXCSR; fpu_restored es obligatorio por llamada y al finalizar. Pool global, RNG, tiempo y cabeza de referencias se restituyen antes de Write/Pump; el helper de atención de ventana sólo observa mensajes y no despacha juego reentrante.

Verificación estática de preparación: scripts Python AST válidos; los33 archivos declarados del manifiesto existen. SHA-256 de LiveComparison.hpp al cerrar esta revisión: `77824787a9784fa42883c782b633c233eea3d44c5941d4350a2f20e3973cb47c`. La aprobación requiere aún las8.192 comparaciones, cero diferencias, objetos original/candidate no nulos y todas las restituciones/rutas verdaderas, además de partidas/configuraciones del runner intactas.
