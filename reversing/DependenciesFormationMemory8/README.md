<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesFormationMemory8 — aprobada y registrada

Dos dependencias de la cohorte game aprobadas y registradas mediante run **20261010-203532-167959**: **8256 pares, cero diferencias y cero nulos**, restauración completa y partidas/configuraciones conservadas. No reaplicar la propuesta histórica ni ejecutar prepare.py.

| Dirección | Identidad descriptiva | Pares previstos |
|---|---|---:|
| 69B1B0 | CFormation::FindNearestFreeDestination | 8192 |
| 745510 | Rs::psGetMemoryFunctions | 64 |

**8256 pares aprobados.** Formation es alias semántico por su llamador DistributeDestinations, sus datos C1A318/C1A324/C1A43C y su selección con FSQRT; no nombre exportado recuperado. psGetMemoryFunctions sí tiene identidad de fuente y pertenece al juego/integración: su contrato devuelve la tabla específica8D6228, aunque el tipo sea RwMemoryFunctions.

Desensamblado completo en disassembly.txt. Formation69B1B0..69B230 no contiene CALL; conserva operaciones x87, stores y comparaciones strict/unordered. Ignora destinos reclamados, calcula distancia3D, conserva el primer empate, devuelve -1 si no selecciona ninguno y **siempre suma al coste** el mejor valor o la constante86D270 (incluso lista vacía/todo reclamado). El nuevo hook se aloja en CFormation y sustituye la delegación en DistributeDestinations. psGetMemoryFunctions son MOV EAX,8D6228 y RET745515; no ejecuta callbacks, inventa tabla ni activa la familia MemoryMgr. Su hook se añade al RsInjectHooks ya existente, con declaración expuesta en platform.h. InjectHooksMain no necesita cambio.

Fixtures CPointList usan su layout real: tamaño13C, count0..24,24 CVector y24 flags. La matriz es la global real m_Destinations; se guarda completa y se instalan temporalmente puntos finitos válidos del contrato y patrones de flags (ninguno/todos/alternos/solo primer o último libre), distancias con componentes3D, coincidencia exacta, empates múltiples, fuera del límite y un punto unordered en casos específicos. No se crean actores ni pools falsos, ni colaboradores: estas dos funciones son hojas puras. Costes iniciales cero, positivos y negativos; outputs privados con guardas y comparación por bits. Se exige ruta sin selección y con selección, count0/24 y empate que elige0; un banco de listas vacías no puede aprobar.

Getter de tabla:64 pares de invariancia sin entradas y sin ramas, suficiente para su cuerpo completo de dos instrucciones. Comprueba puntero exacto8D6228 y16 bytes reales, no escribe la tabla ni invoca sus callbacks; sus cuatro punteros deben resolver a páginas ejecutables reales, no ser nulos. Esta evidencia valida el getter, **no** Malloc/Free/Realloc/Calloc ni su ABI. Esos destinos siguen fuera del alcance.

Cada llamada usa la misma entrada EXE con RH OFF original y ON candidata. Compara EAX completo, costes, ESP y EBX/ESI/EDI/EBP, top/control x87 y entradas readonly. Snapshot/restauración de CPointList, FNSAVE/FRSTOR, MXCSR y dos rutas antes de publicar resultados; la tabla permanece bajo comprobación readonly. Retorno -1 conserva coste nativo; original/candidate son objetos completos también en ramas vacías.

Propuesta aplicada y validada. Python UTF-8 explícito; manifest incluye Formation/PointList/Vector, platform/RW layout y HookRoute. No altera runtime_runner.py, documentos congelados, instalación personal ni instrumentos aprobados. La ejecución admitida cumple cero diferencias/NULL y restauración; auditoría independiente en [audit-runtime.md](audit-runtime.md).

Huellas log `e6a5950555d2d02265bdf3f8baa357cb24b4c93a9ddfd75febc5bc37ee887db9`; ASI `904f607590de333efdf4e09250b8b42d0ded0cb47e5f4385b1e64df5a190af8d`. Evidencia limitada a los fixtures descritos; no integración universal ni validación de callbacks de memoria.
