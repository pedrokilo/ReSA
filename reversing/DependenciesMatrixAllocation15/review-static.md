<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Layout y grafo de efectos, revisión independiente

Alerta retirada: se confundió inicialmente0x1A0 con freeTail.prev. Offsets reales de CMatrixLinkList/CMatrixLink:

| Campo | Offset gestor | Dirección |
| --- | ---: | --- |
| freeHead | 0x150 | 0xB743D8 |
| freeHead.next (+0x50) | 0x1A0 | 0xB74428 |
| freeTail | 0x1A4 | 0xB7442C |
| freeTail.prev (+0x4C) | 0x1F0 | 0xB74478 |

AddToList1: 0x54E9D0 MOV EAX,[ECX+1A0]; 0x54E9D6 LEA EDX,[ECX+1A4]; 0x54E9DC CMP; 0x54E9DE JE 0x54EA07; vacío XOREAX/RET0x54EA09. No desreferencia centinela en vacío. Disponible: repara next.prev/prev.next (0x54E9E0..0x54E9F0), inserta después del head de lista1 (0x54E9F3..0x54EA02), retorna el nodo original0x54EA06.

AddToList2: 0x54EA10 lee el mismo freeHead.next y0x54EA16 obtiene el mismo freeTail. Vacío JE0x54EA50/XOREAX/RET0x54EA52. Disponible mueve el nodo a allocatedListHead=gestor+0xA8 (0x54EA33 lee head.next=+0xF8;0x54EA39 ADD ECX,A8), conserva EAX y retorna0x54EA4F. La fuente también usa freeHead.next. No hay divergencia de extremo.

AllocateMatrix: 0x54F563 lee this.matrix; 0x54F568 JNE0x54F5CB retorna si existe.0x54F56A lee B74428=freeHead.next y0x54F56F compara B7442C=freeTail. Con libre lo extrae y pone al frenteList1, asigna owner=this y this.matrix (0x54F576..0x54F5A9). Si lleno, lee oldest=List1.tail.prev B74328, owner+0x48 y llama RemoveMatrix0x54F3B0 (0x54F5AE..0x54F5B6); después AddToList1 con gestor B74288 (0x54F5BB..0x54F5C0) y asigna owner/this.matrix. La fuente realiza el mismo grafo mediante AddToList1 y operaciones enlazadas.

Grafo fuente: AllocateMatrix→IsFull inline→RemoveMatrix(si lleno)→MoveToFreeList→CMatrixLink::Remove/Insert; después AddToList1→MoveToList1→Remove/Insert. AddToList2→MoveToList2→Remove/Insert. Todos los hooks reales reciben OFF/ON por lado; el cuerpo candidato y su módulo se verifican. No se modifica ningún colaborador sólo para facilitar fixtures.

La prueba independiente mide efectos antes de restaurar: puntero, header508bytes, matrices modificadas84bytes, propietarios24bytes, restoCVehicle de sólo lectura. Los snapshots no constituyen un supuesto modelo de heap: es el pool fijo actual y sus propietarios reales. La lista llena temporal proviene de APIs nativas, conserva un oldest con owner real y no permite eviction de nodo temporal sinowner. El caller sólo existe dentro del tramo síncrono del banco, con rollback antes de volver al juego.

No compilado ni probado. No corrección fuente propuesta; la interpretación errónea quedó retirada antes de cualquier cambio de producción. El resultado futuro se conservará separado de evidencias históricas y sin créditoGAME fuera de la cohorte.
