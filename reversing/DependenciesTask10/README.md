<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesTask10 — tres hojas aprobadas y registradas

Aprobado y registrado: corrida `20261010-211257-052931`, CPedGroups::AddGroup 0x5FB800, Init 0x5FB8C0 y CleanUpForShutDown 0x5FB930. **3 funciones, 6144 pares (2048 por función), cero diferencias, cero registros pasantes con objetos nulos y restauración final verificada**, partidas intactas y configuraciones restauradas. La propuesta se aplicó solamente a PedGroups.cpp; Main ya instala CPedGroups. El intento `20261010-211114-219602` fue rechazado antes de iniciar el juego por falta de espacio: cero pares, configuración/partidas restauradas y ninguna evidencia admitida.

## Contrato binario

Se revisaron los rangos originales completos, con margen posterior al RET: AddGroup5FB800..5FB870 contiene38 instrucciones hasta RET5FB862, Init5FB8C0..5FB930 contiene36 hasta RET5FB929 y CleanUpForShutDown5FB930..5FB990 contiene30 hasta RET5FB981. La transcripción naked conserva ramas, registros, orden de efectos y todos los RET. Dos puentes PUSH dirección/RET llaman a RemoveMember5F80D0 y FlushIntelligence5F7350 reales; no reemplazan sus cuerpos. AddGroup devuelve el primer grupo inactivo o -1, marca actividad, quita los ocho miembros que existan, limpia inteligencia y bandera de misión. Init sólo vacía grupos activos y además pone los ocho índices de referencia SCM a1. CleanUpForShutDown vacía los ocho grupos y conserva sus banderas de actividad e índices SCM.

Los tres destinos son cdecl sin argumentos. El gate captura EAX inmediatamente, ESP, EBX/ESI/EDI/EBP y cadena SEH. Sólo interpreta EAX como retorno para AddGroup; los otros dos son void. Cada comparación invoca la misma entrada EXE con RH producción apagado y activado; todos deben estar registrados y se restaura su estado inicial al final.

## Fixture íntegro y aislamiento real

Se respalda la matriz global completa de ocho CPedGroup, actividad, índices SCM, guardas vecinas y todos los bytes del jugador real, del pool real de tareas y de los3000 nodos originales CReference. Se usan constructores CPedGroup reales sobre los ocho destinos respaldados, inicializando a NULL los eventos que ese constructor original deja sin tocar; no existen eventos ni allocators dinámicos artificiales.

Se cubren los256 patrones de actividad, cuatro distribuciones de miembros (ninguno, uno, alternos, todos) y presencia/ausencia de tareas:2048 combinaciones únicas por función. Las membresías sólo apuntan al actor FindPlayerPed real. Se aparta temporalmente su cadena original de referencias, conservándola intacta: esto evita colisión entre los mismos slots globales preexistentes y los slots de fixture. El head libre se dirige a64 nodos privados CReference reales y se ejecutan RegisterReference y CleanUpOldReference reales. El banco comprueba que cadenas usada/libre particionan esos64 nodos sin ciclos ni duplicados y que ningún nodo original cambia. Limpia todos los slots restantes, verifica jugador sin referencias privadas, restaura la cadena original, el head libre y las arenas antes de cualquier PumpMessages. No promete independencia transitiva de esos colaboradores ni prueba la rama no-jugador de RemoveMember.

Las tareas son CTaskSimpleStandStill completas, construidas por la entrada62F310 real después de usar el allocator61A5A0 real. Se usa CTaskPool privado válido de16 slots, storage dimensionado por CTaskSimpleSlideToCoord y owner=false, sustituyendo sólo su puntero global durante el fixture. Cada grupo recibe una tarea en uno de los32 pares de inteligencia; se cubren las cuatro tablas y todos sus índices. Se comprueba slot ocupado válido, destructor virtual real, bitmap después de la llamada, número de tareas restantes esperado y limpieza de las supervivientes mediante delete real. No hay stubs de allocator ni destructor. Se restituyen header, bitmap, storage, puntero global y bytes del pool original. Guardas en ambos extremos de storage, bitmap y referencias, más las guardas globales de grupos, se observan antes de reponer las arenas.

La inmutabilidad del pool original incluye más que cabecera y puntero: se respaldan **todos sus capacity×0x80 bytes de almacenamiento** —slots ocupados y libres— y **los capacity bytes de bitmap** mediante los punteros reales de su cabecera. El gate `original_task_pool_intact` compara cabecera, storage y bitmap antes de preparar cada lado, después de construir el fixture pero antes de llamar al target, después de la llamada, después de limpiar los recursos privados y al final. Los bytes originales sólo se leen; no se reponen para ocultar una alteración. Begin registra capacidad y tamaños de los snapshots, y el analizador exige esa cobertura completa, el gate en cada lado y la comprobación terminal. La precondición0<capacity<=4096 evita leer un fixture inválido; el pool original normal tiene500 slots. Esto demuestra inmutabilidad del almacenamiento, bitmap y cabecera observados durante la prueba, sin atribuirla a otros pools ni al heap general.

Doce colaboradores tienen rutas resueltas, módulos, bytes de entrada y64 bytes del cuerpo capturados y comprobados antes/después y al final; sus estados RH/locked iniciales se conservan. Las entradas virtuales de cada tarea tienen la misma prueba y aparecen por lado en el log. Los colaboradores conservan el mismo modo para ambos lados. No se admite su reconstrucción de forma transitiva. Toda restitución se comprueba antes de registrar cada par. El estado x87 completo108 bytes y MXCSR se conserva y restaura por lado.

La observación compara todos los bytes del grupo y sus índices/actividad contra un contrato independiente, sin modificar padding ni campos no escritos por el original; incluye punteros reales idénticos porque las arenas privadas son las mismas para ambos lados. Se rechaza cualquier objeto nulo, diferencia, fallo ABI, referencia rota, cambio en estado original, recurso sin liberar o cobertura incompleta.

## Preparación reservada a raíz

```text
python -B reversing/DependenciesTask10/build_probe.py
python -B reversing/DependenciesTask10/run_local.py
python -B reversing/DependenciesTask10/run_local.py --analyze <comparisons.jsonl>
```

El script de build verifica previamente que todos los paths del manifiesto existen. Usa exclusivamente la copia desechable CVehicle Test; no modifica runtime_runner.py, documentos congelados ni el juego personal. No hay aprobación ni admisión hasta compilar, ejecutar y auditar la evidencia completa.

## Evidencia admitida

[Auditoría de admisión](runs/20261010-211257-052931/approval-audit.json), [comparaciones completas](runs/20261010-211257-052931/comparisons.jsonl.gz), [resumen](runs/20261010-211257-052931/summary.json) y [auditoría independiente](audit-runtime.md).

Log descomprimido SHA-256: `99f8aba128bc48a25992148c699baff964eb3273801bd494711beff3482da23f`.
ASI ejecutada SHA-256: `e481a394f1f46ed8ad6f61a5abdd71a3cde42ded8de9d18687aa1b51c1261aed`.
Los tres instrumentos admitidos permanecen inmutables. La prueba no certifica independencia transitiva de los colaboradores originales compartidos ni restauración de metadatos internos del heap.
