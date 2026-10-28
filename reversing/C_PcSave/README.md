<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# C_PcSave / GenerateGameFilename

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruida la entrada 0x006190A0 y activado su hook. C_PcSave completa las cinco entradas del catalogo; no implica validacion completa del sistema de guardado.

## Evidencia

El original carga slot y out de la pila, incrementa slot, pasa la base de nombre en 0xC16F18 y las cadenas "%s%i%s" y ".b" a sprintf (0x821BB5), y retorna limpiando ocho bytes. No lee this ni recibe capacidad de salida. Se reprodujo esa operacion con std::sprintf. El codigo anterior declaraba una capacidad calculada sin conocer la capacidad real del destino y rechazaba un prefijo de 259 caracteres incluso con un destino suficientemente grande.

El llamador sigue siendo responsable de proporcionar espacio para prefijo, numero, extension y terminador. No se amplia ni garantiza la seguridad de los buffers del sistema completo de guardado; se conserva el contrato original para slots validos.

## Verificacion

- Ejecutable Compact del usuario, hash verificado por validate.py antes de extraer instrucciones.
- 2.080 casos: ocho slots, prefijos de longitud 0..259, caracteres variados, comparacion completa del buffer y centinelas.
- Mismos casos aprobados en /Od y /O2; compilacion x86 con /W4 /WX.
- Control negativo slot+2: diferencia detectada inmediatamente.
- Control negativo con cuerpo anterior: termina con error de CRT al exceder su capacidad declarada.
- Compilacion completa Debug x86 aprobada, registrada en build.log.

Las instrucciones originales se ejecutan relocalizadas y su llamada sprintf se redirige al CRT de MSVC del programa de prueba. No se ejecuta el CRT antiguo del juego. Las pruebas son en memoria: no escriben ni borran partidas. La prueba aislada se complemento posteriormente con 2.088 comparaciones dentro del juego; ver runtime/results.json. Sigue pendiente una validacion integral de guardar/cargar.

## Repetir sin IA

Doble clic en PROBAR.cmd --case pcsave-isolated, en la raiz del repositorio. Requiere el ejecutable y las herramientas locales indicadas en validate.py. No necesita Codex abierto, red ni tokens. results.json conserva resultados y hash de la fuente comprobada; test-build contiene los logs y ejecutables locales.

## Validacion dentro del juego (22/09/2026)

GenerateGameFilename se probo al existir el jugador en una sesion real de GTA. Se llamo a 0x6190A0 alternando el hook original y el reconstruido: 2.088 comparaciones, los ocho slots con la ruta real de pruebas y con prefijos de longitud 0..259. Coincidieron todos los bytes de salida y los centinelas. El contador verifico cero llamadas al candidato durante cada comparacion original y exactamente una durante cada comparacion reconstruida. Ademas hubo nueve llamadas al candidato durante el arranque normal del juego.

Se uso el CRT original de GTA para la funcion original, sin dobles. Los hooks Audio y Global/SurfaceInfos_c se desactivaron por los fallos ajenos ya documentados. Las partidas originales y de prueba conservaron sus hashes. El ejecutor cerro su proceso y restauro el ASI previo; despues se instalo el ASI normal recien compilado, sin instrumentacion temporal.

Solo despues de este resultado se actualizaron hooks.csv y ReversedClasses.md a 5/5. La prueba valida GenerateGameFilename; no certifica un ciclo completo de guardar/cargar ni la estabilidad general del mod.

Repetir dentro del juego sin IA: PROBAR.cmd --case pcsave, en la raiz. Cierra GTA antes de ejecutarlo y manten el equipo encendido. El ejecutor tiene limite de 120 segundos, no modifica los documentos de progreso y guarda informes en resultados-pcsave dentro de la instalacion de pruebas. Requiere los binarios instrumentados locales conservados en _baseline.
