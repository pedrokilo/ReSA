<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Adaptadores de planos de CColModel: aprobados y registrados

Se validaron `CCollision::CalculateTrianglePlanes-colModel` (0x418580) y `CCollision::RemoveTrianglePlanes-colModel` (0x4185A0). La propuesta fue aplicada por la coordinación. La primera ejecución de2048 pares no fue aprobada:384 diferencias eran exclusivamente una lectura incorrecta del observador de asignaciones. Banco corregido y repetido completo: **APROBADO y registrado**, corrida `20261010-202525-458974`, dos funciones, 2.048 pares, cero diferencias, cero original/candidate nulos, 4.096 observaciones crudas y estado observado restaurado. Partidas intactas y configuraciones restauradas. Evidencia: [approval-audit.json](runs/20261010-202525-458974/approval-audit.json), [registro completo comprimido](runs/20261010-202525-458974/comparisons.jsonl.gz) y [auditoría independiente](audit-runtime.md). SHA-256 del registro aprobado descomprimido: `91ea740f08b68b530c82d86e3bbf3aec6922a7eb3ba9bbe126742f2f4413162d`; ASI: `63cd3f02f7874168a14ceefd1f74d0882e8bc47a0772b1e4ce8eb4d26fefda99`. SHA-256 del registro fallido conservado: `2ee32881e5a37de305141750dfb55dd0d855f05d35d1e691841e7114a4c0ba2d`. La coordinación aplica, compila y prueba en serie antes de admitir entradas al catálogo.

## Reconstrucción y dominio

En 0x418584 / 0x4185A4 el EXE carga `CColModel::m_pColData` desde +0x2C. Si es nulo retorna en 0x418594 / 0x4185B4. Si existe, sustituye el argumento de pila por ese puntero y salta a 0x416330 / 0x416400. Los dos adaptadores son cdecl: el llamador retira cuatro bytes. La propuesta conserva ese condicional y llama al overload local correspondiente. Retira la aserción adicional de la fuente anterior: un array de triángulos puede existir con el contador cero y el original retorna sin crear planos.

Los overloads de datos, `CCollisionData::{CalculateTrianglePlanes,RemoveTrianglePlanes,SetLinkPtr,GetLinkPtr}` y `CColTrianglePlane::Set` son **colaboradores reconstruidos compartidos**. No se pretende aprobarlos otra vez ni comparar una implementación alternativa de ellos. Los siete permanecen RH ON y locked durante ambas rutas. `HelperHookRoute.hpp` demuestra que entrada, símbolo y cuerpo resuelven al mismo módulo ASI; conserva y restaura flags y bytes. El banco exige esas rutas antes y después de cada llamada. No sustituye colaboradores por stubs.

Los archivos `dis-adapters.txt`, `dis-cache.txt`, `dis-planes.txt`, `dis-plane-set.txt` y `dis-allocator.txt` contienen instrucciones del EXE original. `dis-callers.txt` recoge los seis llamadores directos externos identificados en `docs/MemoryMap/binary_calls.csv`; los otros dos registros son los saltos de estos adaptadores hacia sus colaboradores.

| Llamada | Uso posterior observado |
| --- | --- |
| 0x6DF438 → Remove | Recarga EAX en 0x6DF43D; retira argumento y vuelve a evaluar flags. |
| 0x70870E → Calculate | Recarga EAX en 0x708713; retira argumento, `test` y recarga ECX/EDX. |
| 0x70A082 → Calculate | Recarga EAX en 0x70A087; posteriormente prepara EDX/ECX para otro trabajo. |
| 0x71C352 → Calculate | Recarga EAX, retira argumento, `test`, luego ECX/EDX. |
| 0x71D0D7 → Calculate | `xor ebp`, retiro de argumento y comparación; EDX/EAX se recargan. El `lea ecx,[ecx]` es un relleno sin efecto, y ECX se reemplaza antes de su siguiente uso. |
| 0x73F800 → Calculate | Retira argumentos, compara contador y prepara ECX tras la rama. |

No se encontró consumo de un retorno ni dependencia de registros volátiles o flags conservados por los adaptadores. Cada caso mide ESP y los registros no volátiles EBX/ESI/EDI/EBP, además de usar la misma entrada del EXE con RH OFF/ON.

## Caché aislada y recursos reales

Antes de cada lado se guarda el bloque 0x96591C..0x96596F: caché de 52 bytes en 0x96592C y márgenes de 16 bytes. La prueba usa cuatro enlaces propios de 12 bytes y cinco modelos/datos privados. Los encabezados temporales sólo apuntan a esos enlaces; nunca recorren ni liberan recursos de la caché real. No llama a `CLinkList::Init` ni a `Shutdown`: prepara el layout de los encabezados y enlaces sin asignaciones auxiliares del juego.

La preparación de planos llama al método real reconstruido, y la retirada/expulsión ejecuta la liberación real. Observadores temporales en 0x72F420 y 0x72F430 registran las asignaciones propias y reenvían al **cuerpo original completo**, retirando su salto durante la llamada y comprobando los 16 bytes originales. No imitan el asignador ni cambian tamaños ni punteros devueltos. Malloc72F420 recibe y reenvía solamente size; no existe un segundo argumento hint en esta entrada PC. Un intento de liberar un puntero ajeno a los recursos del caso aborta antes de efectuar la liberación. El banco rechaza la configuración alternativa `MEMORY_MGR_USE_MEMORY_HEAP`.

Tras observar el resultado se libera cada array de planos aún vivo. Se comprueba que todas las asignaciones terminaron liberadas, se retiran los dos observadores y se restauran los bytes originales de las entradas, la caché, las cinco estructuras, las mallas, los enlaces y el control x87. La caché real no queda activa mientras se atienden mensajes; el registro se escribe después de restaurarla.

**Límite de restauración:** se demuestra liberación balanceada de los recursos propios y restauración exacta de los bloques y rutas observados. No se afirma restauración byte a byte de metadatos internos del CRT/heap tras una asignación y liberación reales. Tampoco es una prueba de presión de memoria, hilos simultáneos, fallos del asignador ni del administrador de memoria alternativo.

## Casos y normalización

Son dos funciones × ocho perfiles × 128 casos = **2048 pares**. Cada perfil combina 16 geometrías/traslaciones, cuatro cantidades de triángulos y dos patrones de bytes de guarda. Los doce vértices comprimidos son válidos; los cuatro triángulos son finitos y no degenerados. Los modelos, datos, vértices, triángulos y enlaces tienen márgenes privados comprobados.

| Perfil | Calculate | Remove |
| --- | --- | --- |
| 0 | Modelo sin datos | Modelo sin datos |
| 1 | Datos sin triángulos, sin planos | Datos con triángulos, sin planos |
| 2 | Planos nuevos con caché vacía | Datos sin triángulos, sin planos |
| 3 | Planos nuevos con otras dos entradas | Retirada de la cabeza |
| 4 | Entrada ya en cabeza | Retirada del medio |
| 5 | Promoción del último enlace | Retirada de la cola |
| 6 | Caché llena: expulsión LRU real y nueva asignación | Retirada de la única entrada |
| 7 | Cero triángulos con planos existentes: no los promueve | Cero triángulos con planos existentes: los retira |

Se comprueban el orden esperado de la lista usada, consistencia bidireccional, cuatro enlaces en total, vínculo almacenado en cada array vivo, presencia/ausencia de los planos del objetivo, expulsión del recurso correcto, número de asignaciones y liberaciones, bloques protegidos y ABI. La comparación conserva los bytes de datos/enlaces/caché y los nueve campos escritos de cada plano. Tras liberar todos los recursos también compara los campos definidos y los cuatro bytes del enlace de **todas** las asignaciones, incluidas las retiradas por Remove, expulsión y limpieza. El analizador reconstruye esos campos de cada bloque crudo y coteja los dos lados independientemente. [Defecto encontrado y corrección previa a la prueba](review-normalization.md).

Los únicos valores definidos excluidos de la igualdad entre lados son direcciones de arrays recién asignados —sustituidas por identificadores de asignación— y bytes inicialmente indeterminados del bloque devuelto por el asignador. Esos bytes **no se borran ni se fuerzan en la memoria probada**. En 0x411660 el plano escribe bytes 0..8, dejando el byte 9 intacto. La reserva añade un plano de diez bytes para el enlace alineado; los otros bytes del trailer tampoco se inicializan. `AllocationTrace.hpp` guarda el bloque crudo al salir de Malloc y antes de Free. Exige que cada byte de padding/trailer fuera de campos y enlace siga idéntico a su propio valor inicial. Los eventos `raw_allocations` conservan direcciones y ambos bloques íntegros para auditar esa normalización. El analizador vuelve a comprobarla independientemente.

## Ruido del observador de Malloc en la primera ejecución

Verificación independiente del registro fallido:2048 pares,384 diferencias, exactamente128 en cada perfil Calculate2/3/6. Las diferencias se limitaban a hint en allocations/final_allocations; eliminar sólo esa palabra que no es argumento deja cero diferencias restantes. Todos los gates del registro fallido eran verdaderos. Esto clasifica el defecto del banco, no convierte aquella corrida en aprobación: Se recompiló y repitió completo con la instrumentación corregida; sólo la corrida posterior `20261010-202525-458974` fue admitida.

El EXE ejecuta72F420 mov eax,[esp+4],72F424 push eax,72F425 call824257,72F42A add esp,4 y72F42D ret. No lee [esp+8]. La ruta PC de MemoryMgr.cpp llama plugin::CallAndReturn72F420 con un único size y descarta nHint. El anterior TraceMalloc(size,hint) interpretaba [esp+8] como argumento aunque era una palabra ajena de la pila. Ahora TraceMalloc(uint32 size) reenvía exactamente un argumento y los registros/igualdad/analizador contienen sólo tamaño, identificador, fases, campos de planos, enlaces y recursos reales. No se captura ni se redefine esa segunda palabra como contrato. begin declara allocator_argument_count=1 y el analizador lo exige; registros del esquema anterior no se admiten como prueba de esta corrección. El documento review-normalization.md conserva la revisión anterior a esta ejecución; sus referencias a hint quedaron superadas por esta corrección documentada.

## Ejecución y puerta de aprobación

Después de aplicar la propuesta, la coordinación usa `build_probe.py` y `run_local.py`. El manifiesto enumera Collision, ColModel, CollisionData, ColTrianglePlane, layouts de malla/listas, MemoryMgr, Vector, instalación principal, RH Simple y los observadores nuevos. El constructor común sólo instrumenta Game.cpp temporalmente. Los scripts redirigen `runtime_runner.GAME` en memoria a la copia CVehicle Test; no editan `runtime_runner.py` ni la instalación personal.

`run_local.py --analyze ruta/comparisons.jsonl` exige dos funciones, 1024 casos únicos cada una, 2048 pares únicos, 4096 observaciones crudas, RH registrados, siete colaboradores reales, igualdad, todos los controles de ABI/recursos/guardas y final restaurado. Cualquier resultado nulo, excepción de acceso, fuga, liberación ajena, padding modificado o ruta inesperada impide aprobar. La corrida aprobada satisface esa puerta y ambas entradas fueron admitidas. La ejecución fallida anterior se conserva como evidencia de ruido del banco, sin reutilizarla como aprobación.
