<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente estática — WorldWaterPath6

Estado: fuente aplicada; run 20261010-201708-953003 con 24576 pares iguales, todavía sin admisión. Esta revisión y ampliación del banco no ejecutaron ni compilaron GTA. La nueva ejecución debe confirmar todos los contratos.

## Caminos: instrucciones y contrato

El EXE compara con 8D0F80 = 32f en 5B20F3 (vector) y 5B2388 (escalar). FNSTSW seguido de TEST AH,41h y JP omite el avance cuando la duración es superior a 32: AH=0, resultado cero, paridad par. Inferior o igual producen AH=1/40h y paridad impar; pueden avanzar. La alerta preliminar que suponía avance con 750 ms fue retirada: era una interpretación equivocada de PF. En el run anterior los 4096 índices de cada helper permanecían iguales al de entrada, sin rango ilegal demostrado.

LoadPathSplines5B24D0 reserva 4+count*40/16 y escribe exactamente count en el header. Si P es ESP de entrada, la cuenta leída por sscanf está en P-4: [esp+10] antes del push de tamaño y [esp+14] en FILD5B257B después del push son el mismo local. No añade un nodo a la cuenta.

Process_FlyBy5B25F0 inicia índices 11/5 (5B2677..269D), avanza según siguiente tiempo <= elapsed (5B2750/27B0/2810), y selecciona 1+(count-1)*step al acabar (5B28CD..2934). El banco reproduce este contrato. Header y último tiempo se leen incondicionalmente; los controles llegan a índice esperado+6/+2 y comienzan en esperado-9/-3. El preflight comprueba esas lecturas, alineación e índice final antes de cualquier llamada. Nunca cuenta nodos de padding como almacenamiento válido.

Los intervalos internos son 16/32/33/750 ms; último intervalo 750 ms evita avanzar desde el último nodo corto. Cantidades 2..16 y tiempos antes, iguales, interiores y posteriores. Simulación float32: 520 duraciones <32, 216 ==32, 3360 >32 por helper; 736 avances y cero rangos ilegales. Además del rango se comprueban índice devuelto, canario inmediatamente posterior a la asignación declarada y todos los bytes de entrada. Son expectativas estáticas; el resumen dinámico exige las tres clases y avance efectivo.

CRT821B40 recibe ST0 y se usa su implementación real del EXE, verificada por módulo. No hay sustituto. Epílogos vector5B2321 y escalar5B2476/5B249A son explícitos; se conserva orden x87 y ABI original.

## Agua: atlas real y fronteras

Los helpers 6E5BB0/6E5E90 no contienen CALL. Leen atlas C22910 stride20, quads C21C90 y triángulos C22854; las cantidades reales tienen límites 1021/301/6. Sólo se copia un polígono cargado y se cambia el bit 4 documentado de profundidad limitada. El atlas entero se compara y restaura; no se generan vértices.

Constantes EXE858B44=6f y 858BA4=20f. Quad5E42..5E6C y triángulo616A..6192 comprueban profundidad tras escribir altura. Big!=NULL exige small!=NULL. Big=NULL permite small nulo/no nulo: quad sale antes de profundidad, triángulo todavía la comprueba. Se ejercen únicamente esas tres combinaciones legales y se exige que los outputs opcionales no cambien cuando son ignorados.

La ampliación selecciona polígonos reales con todos los vértices z=0 para fronteras -6/+20 y vecinos nextafter. El water.dat de la copia de pruebas contiene 256 quads y 6 triángulos no degenerados de altura cero, estos últimos de ambas orientaciones. Enumeración conservadora de interiores estrictos encuentra por quad/triángulo respectivamente: igualdad inferior48/18, rechazo inferior limitado16/6, igualdad superior48/18, rechazo superior48/18 y big=NULL superior64/24. El juego debe confirmar esas ramas; el resumen exige cada resultado y ambas rutas geométricas aceptadas. Sin atlas requerido el banco falla explícitamente.

RET quad6E5E77/6E5E85 y triangle6E619D/6E61A7 son explícitos. Comparación por bits de bool AL y outputs; orden FP se conserva. El agua no depende de mocks ni ramas vacías.

## Rutas y restauración

Cada función usa su entrada RH registrada, original OFF y reconstruida ON, con ruta EXE/DLL comprobada por lado. Se comparan ESP, EBX/ESI/EDI/EBP, pila/control x87, guardas, atlas y traslación global. FNSAVE/FRSTOR y MXCSR se restauran antes de escribir registros y al acabar, además de atlas/cantidades/traslación y estados RH. Original y candidate siempre contienen objetos completos; fixtures inválidos quedan rechazados.

No se modificó producción, runtime_runner.py ni documentación congelada durante esta revisión. No reaplicar el generador antiguo: la fuente ya incluye correcciones MASM de la raíz y el banco ampliado es el estado vigente.
