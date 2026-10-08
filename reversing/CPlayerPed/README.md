<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPlayerPed: GetButtonSprintResults

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruida y activada la entrada 0x60A820. CPlayerPed completa las 51 entradas del listado despues de la prueba dentro del juego.

El llamador original 0x60B430 conserva un puntero de animacion en EDX y escribe en [EDX+0x24] despues de la llamada. Un puente x86 conserva explicitamente EDX y limpia el argumento de la pila; el calculo mantiene las operaciones x87 para evitar las diferencias de redondeo detectadas con SSE. Los valores NaN siguen la salida cero del original.

Validacion aislada: 120.192 casos por compilacion (/Od y /O2), cuatro tipos de sprint, tres precisiones x87 y valores limite. Cero diferencias de bits, EDX conservado. Los controles negativos alteran EDX y la formula y deben fallar. validate.py extrae la funcion original del Compact verificado y la relocaliza para ejecutarla sin motor simulado.

Validacion dentro del juego: 4.064 pares original/candidato sobre el jugador real, los cuatro tipos de sprint, negativos, cero, umbral, valores finitos e infinitos y NaN. Llamada al punto original alternando el hook; retorno comparado bit por bit, contador de entrada del candidato y reproduccion de la escritura usando EDX. Todas las comparaciones pasaron. La velocidad del jugador fue restaurada y las partidas conservaron sus hashes. No hubo llamadas espontaneas de sprint antes de iniciar el banco ni revision visual manual de correr/nadar/BMX. No se certifica estabilidad general.

Evidencia: results.json (aislada), runtime/results.json (juego) y el registro local cuyo hash figura en el informe. Configuracion de juego: Audio y Global/SurfaceInfos_c desactivados por los errores ajenos documentados anteriormente.

Repetir sin IA: PROBAR.cmd --case cplayerped en la raiz. Utiliza la copia de pruebas, cierra automaticamente su proceso y restaura el ASI previo. Los binarios instrumentados permanecen locales en _baseline.
