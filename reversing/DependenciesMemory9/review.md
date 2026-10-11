<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión independiente previa a ejecución — Memory9

Sólo lectura del banco/propuesta estable tras coordinación con su autor. Sin cambios a instrumentos, producción, CRT, registro, build ni juego. No constituye admisión.

## Cuerpos y dominios

Comparación directa dis.sh: Malloc72F420..42D cinco instrucciones; Realloc72F440..452 siete; MallocAlign72F4C0..4E2 quince; FreeAlign72F4F0..4FB cuatro con tail JMP. La propuesta conserva instrucciones/orden/limpieza, normalizando CALL/JMP a puentes PUSH dirección/RET al CRT real. Hint adicional no se lee. Todos los retornos naked son explícitos y el tail mantiene el slot de argumento corregido. Bajo MEMORY_MGR_USE_MEMORY_HEAP queda ruta alternativa excluida explícitamente del banco.

Alineación potencia2 >=4 cubre dominio legítimo de guardar puntero en result-4; size+align no desborda para fixtures0..4096/4..256. No null freealign ni align0/no potencia, overflow ni OOM artificial. CRT original conserva realloc(NULL,0)→malloc0 y realloc(nonNULL,0)→free+NULL; los seis perfiles están exigidos por analyzer. Una reserva pequeña fallida rechaza o aborta, no aprueba rama de fallo simplificada.

## Rutas, datos y recursos

Canonical bytes malloc18/realloc429/free113 y módulo EXE/Resolve(entry)==entry verifican implementación real de colaboradores antes/después/final. No se parchea CRT ni se sustituye allocator. RH registrado Body del ASI, OFF original/ON candidata y snapshot de entradas/ILThunk restaurado. ABI mide ESP y no volátiles; FreeAlign captura su slot físico transformado en base. No iguala direcciones distintas ni memoria sin inicializar. Realloc sólo compara min(old,new) bytes definidos antes de completar el nuevo payload. No lee tras liberar. Bloques propios se liberan con el CRT que los reservó.

Restauración FNSAVE108/MXCSR, rutas/bytes y equilibrio de vidas propias. El contador allocations/frees representa recursos que el banco adquiere/libera por su lógica; no es una instrumentación del heap ni prueba su disposición interna. FreeAlign supone que el tail nativo libera: la transcripción exacta justifica el efecto, no el contador por sí solo. Canario64 independiente se comprueba antes de free; no es un redzone contiguo al payload ni certifica ausencia universal de overflow.

## Límite de observación del tamaño y propuesta no invasiva

**requested_size proviene del fixture: no observa el tamaño solicitado por la candidata al CRT.** Payload correcto y un guardián separado podrían pasar frente a sobreasignación; la granularidad del heap puede ocultar pequeñas diferencias. No llamarlo observación dinámica del request ni usarse como única prueba de fidelidad del wrapper. También una subasignación errónea podría permitir que el banco escriba fuera del tamaño realmente adquirido antes de detectarlo.

La revisión estática de los cuerpos propuestos confirma MOV/PUSH exactos y ADD size+align, pero todavía no hay evidencia de instrucciones efectivamente ejecutadas del ASI porque no está compilado. Propuesta no invasiva: después del build, resolver Body RH y desensamblar/verificar el cuerpo candidato completo y puente real; registrar su prueba de instrucciones/bytes con module y manifest. Eso demuestra el cálculo y la posición física del argumento sin modificar CRT ni usar mocks. El banco ya captura el argumento físico exterior, pero eso no ve el argumento interno que se apila para CRT.

Un _msize del **mismo CRT del EXE**, identificado por instrucciones y contrato real, podría medir capacidad antes de escribir y distinguir sobre/subasignaciones grandes. No usar _msize del CRT de la DLL sobre bloques de otro allocator, ni asumir dirección sin identificar. Incluso el _msize auténtico no demuestra request exacto si dos tamaños caen en la misma granularidad. No se ha identificado/callado dicho servicio en esta revisión. Cualquier capacidad observada debe normalizarse con cuidado; heaps pequeños y Windows pueden redondear de maneras diferentes. La prueba del cuerpo ejecutado es la vía recomendada para exactitud del request, complementando los efectos definidos existentes.

## Resultado

Sin diferencia de fuente detectada en la propuesta frente a las cuatro hojas EXE. Dominio, normalización y vidas privadas son razonables. Antes de admitir debe constar qué acredita request exacto: instrucción real de la candidata más puente CRT, no requested_size fixture. El analyzer exige todas las puertas/casos/perfiles y restauración final, pero no mide el tamaño interno por sí mismo. Alcance de CRT, heap y fallos excepcionales permanece limitado y explícito.
