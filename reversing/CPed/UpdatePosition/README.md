<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPed::UpdatePosition (0x5E1B10)

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruida y activada tras pasar 2.534 comparaciones dentro de GTA. El progreso documentado se actualizo solamente despues de comprobar el informe final.

La funcion actualiza orientacion y velocidad del peaton a partir del desplazamiento de animacion, la velocidad de su apoyo, la pendiente y los limites de aceleracion. Respeta las salidas tempranas durante replay, estando adjunto y en el aire; en este ultimo caso solo ajusta orientacion para natacion, jetpack o cuerda SWAT. Reproduce el ajuste de barcos para peatones no jugadores, el descenso sobre pendientes, las restricciones al morir y al caer, y los limites de automoviles y la clase base de motos/bicicletas.

El primer ensayo detecto un error real: IsBMX no equivale a la comprobacion original del tipo principal 9. La clase base correcta es IsBike, que incluye la BMX. El banco se detuvo en el caso 696 sin modificar el progreso. Se corrigio y se agrego una motocicleta real a los casos. La constante del limite de velocidad es sq(0.2f), conservando el valor binario del ejecutable.

## Prueba final dentro del juego

- 12 casos de salidas tempranas y tareas reales de natacion, jetpack y cuerda, sin procesar esas tareas como una sesion de juego completa.
- 216 casos de suelo y pendiente, normal nula o no nula, movimiento en ambos sentidos o cambio cero, caida y tres pasos de tiempo (0, 0.5, 2).
- 2.304 casos de apoyo en automovil 560, barco 473, BMX 481 y moto 522; jugador/peaton, vivo/muriendo, combinaciones de flags fisicos, apoyo adjunto/no adjunto, velocidades inferiores, iguales y superiores al umbral y tres pasos de tiempo.
- 2 casos con tipo de entidad objeto sobre un vehiculo real para comprobar esa rama por tipo. No se afirma que se haya construido un CObject.
- Todas las familias se ejecutaron con matriz y sin matriz del peaton.

Se alterna el hook y se llama al punto de entrada original; se restaura la entrada antes de ejecutar el candidato. Se comparan posicion, bases de la matriz o transformacion simple, velocidad lineal/angular, orientacion actual/objetivo y campos discretos de estado/apoyo. El contador confirma cero entradas al candidato en la llamada original y exactamente una en la llamada reconstruida.

Tolerancia fijada: 0.00001. Error numerico maximo observado: 0.0. No hubo diferencias discretas. Los hashes de las partidas originales y de prueba no cambiaron. No se hizo revision visual manual ni se certifica estabilidad general de CPed. Audio y Global/SurfaceInfos_c estaban desactivados por los fallos ajenos documentados.

La compilacion completa Debug x86 sin instrumentacion paso. El ASI normal se instalo en la copia de pruebas, conservando el anterior en _baseline. Los ejecutables del juego no se modificaron.

## Repetir sin IA

Doble clic en PROBAR.cmd --case cped-position, en la raiz del repositorio. Cierra GTA antes. El ejecutor no necesita Codex ni servicios de IA; tiene limite de 180 segundos, cierra solo su proceso y restaura el ASI instalado previamente. Requiere el binario instrumentado local conservado en _baseline. No actualiza documentos de progreso por si mismo.

Informes: resultados-updateposition dentro de la copia de pruebas. runtime/results.json conserva hashes, cobertura y resultado final; runtime/regression-detected.json documenta el fallo detectado. Los registros completos son locales. El analizador tambien rechazo registros vacios, incompletos, duplicados, con error y con salida numerica alterada.
