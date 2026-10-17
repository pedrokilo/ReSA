<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPlayerPed: GetButtonSprintResults

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruida y activada la entrada 0x60A820. CPlayerPed completa las 51 entradas del listado despues de la prueba dentro del juego.

El llamador original 0x60B430 conserva un puntero de animacion en EDX y escribe en [EDX+0x24] despues de la llamada. Un puente x86 conserva explicitamente EDX y limpia el argumento de la pila; el calculo mantiene las operaciones x87 para evitar las diferencias de redondeo detectadas con SSE. Los valores NaN siguen la salida cero del original.

Validacion aislada: 120.192 casos por compilacion (/Od y /O2), cuatro tipos de sprint, tres precisiones x87 y valores limite. Cero diferencias de bits, EDX conservado. Los controles negativos alteran EDX y la formula y deben fallar. validate.py extrae la funcion original del Compact verificado y la relocaliza para ejecutarla sin motor simulado.

Validacion dentro del juego: 4.064 pares original/candidato sobre el jugador real, los cuatro tipos de sprint, negativos, cero, umbral, valores finitos e infinitos y NaN. Llamada al punto original alternando el hook; retorno comparado bit por bit, contador de entrada del candidato y reproduccion de la escritura usando EDX. Todas las comparaciones pasaron. La velocidad del jugador fue restaurada y las partidas conservaron sus hashes. No hubo llamadas espontaneas de sprint antes de iniciar el banco ni revision visual manual de correr/nadar/BMX. No se certifica estabilidad general.

Evidencia: results.json (aislada), runtime/results.json (juego) y el registro local cuyo hash figura en el informe. Configuracion de juego: Audio y Global/SurfaceInfos_c desactivados por los errores ajenos documentados anteriormente.

Repetir sin IA: PROBAR.cmd --case cplayerped en la raiz. Utiliza la copia de pruebas, cierra automaticamente su proceso y restaura el ASI previo. Los binarios instrumentados permanecen locales en _baseline.
